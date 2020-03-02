#include "SkyLightManager.h"

//static JobManager::SJobState g_JobState; 


SkyLightManager::SkyLightManager(osgViewer::Viewer* viewer )
: m_pSkyLightNishita( new SkyLightNishita )
, m_pViewer(viewer)
//, m_pSkyDomeMesh( 0 )
, m_curSkyDomeCondition()
, m_updatingSkyDomeCondition()
, m_numSkyDomeColorsComputed( SSkyLightRenderParams::skyDomeTextureSize )
, m_curBackBuffer( 0 )
, m_lastFrameID( 0 )
, m_curSkyHemiColor()
, m_curHazeColor( 0.0f, 0.0f, 0.0f )
, m_curHazeColorMieNoPremul( 0.0f, 0.0f, 0.0f )
, m_curHazeColorRayleighNoPremul( 0.0f, 0.0f, 0.0f )
, m_skyHemiColorAccum()
, m_hazeColorAccum( 0.0f, 0.0f, 0.0f )
, m_hazeColorMieNoPremulAccum( 0.0f, 0.0f, 0.0f )
, m_hazeColorRayleighNoPremulAccum( 0.0f, 0.0f, 0.0f )
, m_bFlushFullUpdate(false)
, m_renderParams()
{
	//MEMSTAT_CONTEXT(EMemStatContextTypes::MSC_Other, 0, "SkyLightManager");
    //m_updateThread = new UpdateThread(this);
	InitSkyDomeMesh();

	m_updateRequested[0] = m_updateRequested[1] = 0;

	// init textures with default data
	m_skyDomeTextureDataMie[ 0 ].resize( SSkyLightRenderParams::skyDomeTextureSize);
	m_skyDomeTextureDataMie[ 1 ].resize( SSkyLightRenderParams::skyDomeTextureSize);
	m_skyDomeTextureDataRayleigh[ 0 ].resize( SSkyLightRenderParams::skyDomeTextureSize);
	m_skyDomeTextureDataRayleigh[ 1 ].resize( SSkyLightRenderParams::skyDomeTextureSize);

	// init time stamps
    m_skyDomeTextureTimeStamp[ 0 ] = m_pViewer->getFrameStamp()->getFrameNumber();
	m_skyDomeTextureTimeStamp[ 1 ] = m_pViewer->getFrameStamp()->getFrameNumber();

	// init sky hemisphere colors and accumulators
	memset(m_curSkyHemiColor, 0, sizeof(m_curSkyHemiColor));
	memset(m_skyHemiColorAccum, 0, sizeof(m_skyHemiColorAccum));

	// set default render parameters
	UpdateRenderParams();
}

SkyLightManager::~SkyLightManager()
{
    m_pSkyLightNishita.release();
}

void SkyLightManager::InitSkyDomeMesh()
{
}

void SkyLightManager::PushUpdateParams()
{
	//pushes the update parameters, explicite call since engine requests asynchronously
	memcpy(&m_reqSkyDomeCondition[0], &m_reqSkyDomeCondition[1], sizeof(SSkyDomeCondition));
	m_updateRequested[0] = m_updateRequested[1];
	m_updateRequested[1] = 0;
}

void SkyLightManager::SetSkyDomeCondition( const SSkyDomeCondition& skyDomeCondition )
{
	m_reqSkyDomeCondition[1] = skyDomeCondition;
	m_updateRequested[1] = 1;
}


// 一次更新所有
void SkyLightManager::FullUpdate()
{
	//Sync();
	PushUpdateParams();
	/*TSkyJob job( C3DEngine::GetMainFrameID(), SSkyLightRenderParams::skyDomeTextureSize, (int)1 );
	job.SetClassInstance(this);
	job.RegisterJobState(&g_JobState);
	job.Run();*/
	m_needRenderParamUpdate = true;
	m_bFlushFullUpdate = true;

    UpdateInternal(m_pViewer->getFrameStamp()->getFrameNumber(), SSkyLightRenderParams::skyDomeTextureSize, (int)1 );
}

// 渐进更新
void SkyLightManager::IncrementalUpdate( f32 updateRatioPerFrame )
{
	//Sync();

	//FUNCTION_PROFILER_3DENGINE;
	
	// get current ID of "main" frame (no recursive rendering), 
	// incremental update should only be processed once per frame
	if( m_lastFrameID != m_pViewer->getFrameStamp()->getFrameNumber() )
	{
		int32 numUpdate( (int32) ( (f32) SSkyLightRenderParams::skyDomeTextureSize * updateRatioPerFrame / 100.0f + 0.5f ) );
        numUpdate = osg::clampBetween( numUpdate, 1, SSkyLightRenderParams::skyDomeTextureSize );
		if(m_needRenderParamUpdate)
			UpdateRenderParams();			// update render params

		PushUpdateParams();
        // todo
	/*	TSkyJob job( C3DEngine::GetMainFrameID(), numUpdate, (int)0 );
		job.SetClassInstance(this);
		job.RegisterJobState(&g_JobState);
		job.Run(); */
	}
}

#if defined(_CPU_X86) && defined(_MSC_VER)
// calculates the cosine and sine of the given angle in radians 
ILINE void cry_sincosf (f32 angle, f32* pSin, f32* pCos) {
	__asm {
		FLD         DWORD PTR       angle
			FSINCOS
			MOV         eAX,pCos
			MOV         eDX,pSin
			FSTP        DWORD PTR [eAX]	//store cosine
			FSTP        DWORD PTR [eDX]	//store sine
	}
}

// calculates the cosine and sine of the given angle in radians 
ILINE void cry_sincos (f64 angle, f64* pSin, f64* pCos) {
	__asm {
		FLD         QWORD PTR       angle
			FSINCOS
			MOV         eAX,pCos
			MOV         eDX,pSin
			FSTP        QWORD PTR [eAX]	//store cosine
			FSTP        QWORD PTR [eDX]	//store sine
	}
}

#else

ILINE void cry_sincosf (f32 angle, f32* pSin, f32* pCos) {	*pSin = f32(sin(angle));	*pCos = f32(cos(angle));	}
ILINE void cry_sincos  (f64 angle, f64* pSin, f64* pCos) {	*pSin = f64(sin(angle));  *pCos = f64(cos(angle));	}
#endif 

ILINE void sincos_tpl(f64 angle, f64* pSin,f64* pCos) { cry_sincos(angle,pSin,pCos); }
ILINE void sincos_tpl(f32 angle, f32* pSin,f32* pCos) { cry_sincosf(angle,pSin,pCos); }

void SkyLightManager::UpdateInternal( int32 newFrameID, int32 numUpdates, int callerIsFullUpdate )
{
  
	//__cache_range_write_async(m_pSkyLightNishita, (char*)m_pSkyLightNishita + sizeof(CSkyLightNishita));
	// update sky dome if requested -- requires last update request to be fully processed!
	int procUpdate = callerIsFullUpdate;
	procUpdate |= (int)IsSkyDomeUpdateFinished();
	procUpdate &= m_updateRequested[0];
	if( procUpdate ) 
	{
		// set sky dome settings
    memcpy(&m_updatingSkyDomeCondition, &m_reqSkyDomeCondition[0], sizeof(SSkyDomeCondition));
		m_pSkyLightNishita->SetSunDirection( m_updatingSkyDomeCondition.m_sunDirection );
		m_pSkyLightNishita->SetRGBWaveLengths( m_updatingSkyDomeCondition.m_rgbWaveLengths );
		m_pSkyLightNishita->SetAtmosphericConditions( m_updatingSkyDomeCondition.m_sunIntensity, 
			1e-4f * m_updatingSkyDomeCondition.m_Km, 1e-4f * m_updatingSkyDomeCondition.m_Kr, m_updatingSkyDomeCondition.m_g );	// scale mie and rayleigh scattering for more convenient editing in time of day dialog

		// update request has been accepted
		m_updateRequested[0] = 0; 
		m_numSkyDomeColorsComputed = 0;

		// reset sky & haze color accumulator
		m_hazeColorAccum = osg::Vec3( 0.0f, 0.0f, 0.0f );
		m_hazeColorMieNoPremulAccum = osg::Vec3( 0.0f, 0.0f, 0.0f );
		m_hazeColorRayleighNoPremulAccum = osg::Vec3( 0.0f, 0.0f, 0.0f );
		memset(m_skyHemiColorAccum, 0, sizeof(m_skyHemiColorAccum));
	}

	// any work to do?
	if( false == IsSkyDomeUpdateFinished() )
	{
		if( numUpdates <= 0 )
		{
			// do a full update
			numUpdates = SSkyLightRenderParams::skyDomeTextureSize;
		}

		// find minimally required work load for this incremental update
		numUpdates = std::min( SSkyLightRenderParams::skyDomeTextureSize - m_numSkyDomeColorsComputed, numUpdates );

		// perform color computations
		SkyDomeTextureData& skyDomeTextureDataMie( m_skyDomeTextureDataMie[ GetBackBuffer() ] );
		SkyDomeTextureData& skyDomeTextureDataRayleigh( m_skyDomeTextureDataRayleigh[ GetBackBuffer() ] );
		//__cache_range_write_async(&skyDomeTextureDataMie[0], &skyDomeTextureDataMie[SSkyLightRenderParams::skyDomeTextureSize-1]); 
		//__cache_range_write_async(&skyDomeTextureDataRayleigh[0], &skyDomeTextureDataRayleigh[SSkyLightRenderParams::skyDomeTextureSize-1]);
		//huge optimization here for SPUs: optical lut is small but accessed the most, cache onto stack





		int32 numSkyDomeColorsComputed( m_numSkyDomeColorsComputed );
		for( ; numUpdates > 0; --numUpdates, ++numSkyDomeColorsComputed )
		{
			// calc latitude/longitude
			int lon( numSkyDomeColorsComputed / SSkyLightRenderParams::skyDomeTextureWidth );
			int lat( numSkyDomeColorsComputed % SSkyLightRenderParams::skyDomeTextureWidth );

            float lonArc( osg::DegreesToRadians( (float) lon * 90.0f / (float) SSkyLightRenderParams::skyDomeTextureHeight ) );
            float latArc( osg::DegreesToRadians( (float) lat * 360.0f / (float) SSkyLightRenderParams::skyDomeTextureWidth ) );

			float sinLon(0); float cosLon(0);
			sincos_tpl(lonArc, &sinLon, &cosLon);
			float sinLat(0); float cosLat(0);
			sincos_tpl(latArc, &sinLat, &cosLat);

			// calc sky direction for given update latitude/longitude (hemisphere)
			osg::Vec3 skyDir( sinLon * cosLat, sinLon * sinLat, cosLon );

			// compute color
			//Vec3 skyColAtDir( 0.0, 0.0, 0.0 );
			osg::Vec3 skyColAtDirMieNoPremul( 0.0, 0.0, 0.0 ); 
			osg::Vec3 skyColAtDirRayleighNoPremul( 0.0, 0.0, 0.0 );
			osg::Vec3 skyColAtDirRayleigh( 0.0, 0.0, 0.0 );



			m_pSkyLightNishita->ComputeSkyColor( skyDir, 0 , &skyColAtDirMieNoPremul, &skyColAtDirRayleighNoPremul, &skyColAtDirRayleigh );

			// store color in texture
            osg::Vec4 hf = osg::Vec4( skyColAtDirMieNoPremul.x(), skyColAtDirMieNoPremul.y(), skyColAtDirMieNoPremul.z(), 1.0f );






      skyDomeTextureDataMie[ numSkyDomeColorsComputed ] = hf;
			//skyDomeTextureDataMie[ numSkyDomeColorsComputed ] = CryHalf4( 0.25f, 0.5f, 0.75f, 1.0f );
			hf = osg::Vec4( skyColAtDirRayleighNoPremul.x(), skyColAtDirRayleighNoPremul.y(), skyColAtDirRayleighNoPremul.z(), 1.0f );






      skyDomeTextureDataRayleigh[ numSkyDomeColorsComputed ] = hf;

			// update haze color accum (accumulate second last sample row)
			if( lon == SSkyLightRenderParams::skyDomeTextureHeight - 2 )
			{
				m_hazeColorAccum += skyColAtDirRayleigh;
				m_hazeColorMieNoPremulAccum += skyColAtDirMieNoPremul;
				m_hazeColorRayleighNoPremulAccum += skyColAtDirRayleighNoPremul;
			}

			// update sky hemisphere color accumulator
			int y(lon >> SSkyLightRenderParams::skyDomeTextureHeightBy2Log);
			int x(((lat+SSkyLightRenderParams::skyDomeTextureWidthBy8) & (SSkyLightRenderParams::skyDomeTextureWidth-1)) >> SSkyLightRenderParams::skyDomeTextureWidthBy4Log);
			int skyHemiColAccumIdx(x*y + y);
			assert(((unsigned int)skyHemiColAccumIdx) < 5);
			m_skyHemiColorAccum[skyHemiColAccumIdx] += skyColAtDirRayleigh;
		}

		m_numSkyDomeColorsComputed = numSkyDomeColorsComputed;

		// sky dome update finished?
		if( false != IsSkyDomeUpdateFinished() )
		{
			// update time stamp
			m_skyDomeTextureTimeStamp[ GetBackBuffer() ] = newFrameID;

			// get new haze color
			const float c_invNumHazeSamples( 1.0f / (float) SSkyLightRenderParams::skyDomeTextureWidth );
			m_curHazeColor = m_hazeColorAccum * c_invNumHazeSamples;
			m_curHazeColorMieNoPremul = m_hazeColorMieNoPremulAccum * c_invNumHazeSamples;
			m_curHazeColorRayleighNoPremul = m_hazeColorRayleighNoPremulAccum * c_invNumHazeSamples;

			// get new sky hemisphere colors
			const float c_scaleHemiTop(2.0f / (SSkyLightRenderParams::skyDomeTextureWidth * SSkyLightRenderParams::skyDomeTextureHeight));
			const float c_scaleHemiSide(8.0f / (SSkyLightRenderParams::skyDomeTextureWidth * SSkyLightRenderParams::skyDomeTextureHeight));
			m_curSkyHemiColor[0] = m_skyHemiColorAccum[0] * c_scaleHemiTop;
			m_curSkyHemiColor[1] = m_skyHemiColorAccum[1] * c_scaleHemiSide;
			m_curSkyHemiColor[2] = m_skyHemiColorAccum[2] * c_scaleHemiSide;
			m_curSkyHemiColor[3] = m_skyHemiColorAccum[3] * c_scaleHemiSide;
			m_curSkyHemiColor[4] = m_skyHemiColorAccum[4] * c_scaleHemiSide;

			// toggle sky light buffers
			ToggleBuffer();
		}
	}

	// update frame ID
	m_lastFrameID = newFrameID;
}

void SkyLightManager::SetQuality( int32 quality )
{
	if( quality != m_pSkyLightNishita->GetInScatteringIntegralStepSize() )
	{
		//Sync(); todo
		// when setting new quality we need to start sky dome update from scratch...
		// ... to avoid "artifacts" in the resulting texture
		m_numSkyDomeColorsComputed = 0; 
		m_pSkyLightNishita->SetInScatteringIntegralStepSize( quality );
	}
}

const SSkyLightRenderParams* SkyLightManager::GetRenderParams() const
{
	return &m_renderParams;
}

void SkyLightManager::UpdateRenderParams()
{
	// sky dome mesh data
    // todo 
	//m_renderParams.m_pSkyDomeMesh = m_pSkyDomeMesh;

	// sky dome texture access
	m_renderParams.m_skyDomeTextureTimeStamp = m_skyDomeTextureTimeStamp[ GetFrontBuffer() ];
	m_renderParams.m_pSkyDomeTextureDataMie = (const void*) &m_skyDomeTextureDataMie[ GetFrontBuffer() ][ 0 ];
	m_renderParams.m_pSkyDomeTextureDataRayleigh = (const void*) &m_skyDomeTextureDataRayleigh[ GetFrontBuffer() ][ 0 ];
	m_renderParams.m_skyDomeTexturePitch = SSkyLightRenderParams::skyDomeTextureWidth * sizeof( osg::Vec4 );

	// shader constants for final per-pixel phase computation
	m_renderParams.m_partialMieInScatteringConst = m_pSkyLightNishita->GetPartialMieInScatteringConst();
	m_renderParams.m_partialRayleighInScatteringConst = m_pSkyLightNishita->GetPartialRayleighInScatteringConst();
	osg::Vec3 sunDir( m_pSkyLightNishita->GetSunDirection() );
	m_renderParams.m_sunDirection = osg::Vec4( sunDir.x(), sunDir.y(), sunDir.z(), 0.0f );
	m_renderParams.m_phaseFunctionConsts = m_pSkyLightNishita->GetPhaseFunctionConsts();
	m_renderParams.m_hazeColor = osg::Vec4( m_curHazeColor.x(), m_curHazeColor.y(), m_curHazeColor.z(), 0 );
	m_renderParams.m_hazeColorMieNoPremul = osg::Vec4( m_curHazeColorMieNoPremul.x(), m_curHazeColorMieNoPremul.y(), m_curHazeColorMieNoPremul.z(), 0 );
	m_renderParams.m_hazeColorRayleighNoPremul = osg::Vec4( m_curHazeColorRayleighNoPremul.x(), m_curHazeColorRayleighNoPremul.y(), m_curHazeColorRayleighNoPremul.z(), 0 );

	// set sky hemisphere colors
	m_renderParams.m_skyColorTop = m_curSkyHemiColor[0];
	m_renderParams.m_skyColorNorth = m_curSkyHemiColor[3];
	m_renderParams.m_skyColorWest = m_curSkyHemiColor[4];
	m_renderParams.m_skyColorSouth = m_curSkyHemiColor[1];
	m_renderParams.m_skyColorEast = m_curSkyHemiColor[2];

	// copy sky dome condition params
	m_curSkyDomeCondition = m_updatingSkyDomeCondition;

	m_needRenderParamUpdate = 0;
}

void SkyLightManager::GetCurSkyDomeCondition( SSkyDomeCondition& skyCond ) const
{
	skyCond = m_curSkyDomeCondition;
}

bool SkyLightManager::IsSkyDomeUpdateFinished() const
{
	return( SSkyLightRenderParams::skyDomeTextureSize == m_numSkyDomeColorsComputed );
}


int SkyLightManager::GetFrontBuffer() const
{
	assert( m_curBackBuffer >= 0 && m_curBackBuffer <= 1 );
	return( ( m_curBackBuffer + 1 ) & 1 );
}


int SkyLightManager::GetBackBuffer() const
{
	assert( m_curBackBuffer >= 0 && m_curBackBuffer <= 1 );
	return( m_curBackBuffer );
}


void SkyLightManager::ToggleBuffer()
{
	assert( m_curBackBuffer >= 0 && m_curBackBuffer <= 1 );
	//better enforce cache flushing then making PPU wait til job has been finished
	m_curBackBuffer = ( m_curBackBuffer + 1 ) & 1;
	m_needRenderParamUpdate = 1;
}

UpdateThread::UpdateThread(SkyLightManager* skyLightManager):m_skyLightManager(skyLightManager)
{

}
void UpdateThread::run()
{
    while(!testCancel())
	{

        // do 
        if(m_skyLightManager)
        {
            //m_skyLightManager->UpdateInternal(m_newFrameNum,m_numUpdates,m_bCallerIsFullUpdate);
        }

    }
}

float g_angle = 90.0;

 bool SkyLightEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*) 
    {
        if(ea.getEventType() == osgGA::GUIEventAdapter::KEYUP)
        {

            if(ea.getKey() == osgGA::GUIEventAdapter::KEY_F3)
            {
                // update sky param
                std::cout << "update sky param" << std::endl;

                osg::Vec3 sunDir = osg::Vec3( 0.0f, 0.707106f, 0.707106f );
                osg::Vec3 sunIntensity = osg::Vec3(200.0f, 200.0f, 20.0f );
                float km = 0.001f;
                float kr = 0.00025f;
                float g = -0.99f;
                osg::Vec3 waveLength = osg::Vec3(650.0f, 570.0f, 475.0f );
                bool bForceImmediateUpdate = true;

     
                _skyLightManager->SetSkyLightParameters(sunDir,sunIntensity,km,kr,g,waveLength,bForceImmediateUpdate);
            }
            if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)
            {
 
                // update sky param
                std::cout << "left down:" << g_angle << std::endl;
                g_angle +=3.0;
                float a = osg::DegreesToRadians(g_angle);
                _sunDir = osg::Vec3(cos(a),0,sin(a));
                _sunDir.normalize();
                osg::Vec3 sunDir = _sunDir;
                osg::Vec3 sunIntensity = osg::Vec3(200.0f, 200.0f, 20.0f );
                float km = 0.001f;
                float kr = 0.00025f;
                float g = -0.99f;
                osg::Vec3 waveLength = osg::Vec3(650.0f, 570.0f, 475.0f );
                bool bForceImmediateUpdate = true;

     
                _skyLightManager->SetSkyLightParameters(sunDir,sunIntensity,km,kr,g,waveLength,bForceImmediateUpdate);
           
            }
            if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
            {
                   
                g_angle -=3.0;
                std::cout << "right down:" <<g_angle << std::endl;
                float a = osg::DegreesToRadians(g_angle);
                _sunDir = osg::Vec3(cos(a),0,sin(a));
                _sunDir.normalize();
                osg::Vec3 sunDir = _sunDir;
                osg::Vec3 sunIntensity = osg::Vec3(200.0f, 200.0f, 20.0f );
                float km = 0.001f;
                float kr = 0.00025f;
                float g = -0.99f;
                osg::Vec3 waveLength = osg::Vec3(650.0f, 570.0f, 475.0f );
                bool bForceImmediateUpdate = true;

     
                _skyLightManager->SetSkyLightParameters(sunDir,sunIntensity,km,kr,g,waveLength,bForceImmediateUpdate);

            }

        }
        return false;
    }