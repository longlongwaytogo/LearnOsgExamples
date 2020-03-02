#include "HDRSky.h"
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osg/Program>
#include <osg/Shader>
#include <osgDB/ReadFile>
#include <osg/Image>
#include <osg/Depth>
#include "../SkyBox"

HDRSky::HDRSky()
: m_pRenderParams(0)
, m_skyDomeTextureLastTimeStamp(-1)
, m_frameReset(0)
//, m_pStars(0)
, m_pSkyDomeTextureMie(0)
, m_pSkyDomeTextureRayleigh(0)
, m_bDebug(true)
{
/*	mfSetType(eDATA_HDRSky);
	mfUpdateFlags(FCEF_TRANSFORM);*/
	
    Init();
   
}

HDRSky::~HDRSky()
{

}

 osg::Geode* HDRSky::CreateHDRSkyDome()
    {
        const unsigned int c_numRings( 20 );
	    const unsigned int c_numSections( 20 );
	    const unsigned int c_numSkyDomeVertices( ( c_numRings + 1 ) * ( c_numSections + 1 ) );
	    const unsigned int c_numSkyDomeTriangles( 2 * c_numRings * c_numSections );
	    const unsigned int c_numSkyDomeIndices( c_numSkyDomeTriangles * 3 );

	//std::vector<  unsigned int > skyDomeIndices;
	//std::vector< SVF_P3F_C4B_T2F > skyDomeVertices;

    osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> texCoordArray = new osg::Vec2Array;
    osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
    osg::ref_ptr<osg::UIntArray> indicesArray = new osg::UIntArray;
    osg::ref_ptr<osg::DrawElementsUInt> skyDomeIndiceArray = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,0);
    osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array;


	// setup buffers with source data
	//skyDomeVertices.reserve( c_numSkyDomeVertices );
	//skyDomeIndices.reserve( c_numSkyDomeIndices );

	// calculate vertices
    float sectionSlice( osg::DegreesToRadians( 360.0f / (float) c_numSections ) );
	float ringSlice( osg::DegreesToRadians( 180.0f / (float) c_numRings ) );
    for( unsigned int a = 0 ; a <= c_numRings; ++a )
	{
		float w( sinf( a * ringSlice ) );
		float z( cosf( a * ringSlice ) );

		for( unsigned int i = 0; i <= c_numSections; ++i )
		{
			//SVF_P3F_C4B_T2F v;
			
			float ii( i - a * 0.5f ); // Gives better tessellation, requires texture address mode to be "wrap" 
																// for u when rendering (see v.st[ 0 ] below). Otherwise set ii = i;		
            vertexArray->push_back(osg::Vec3(cosf( ii * sectionSlice ) * w, sinf( ii * sectionSlice ) * w, z));
            normalArray->push_back(osg::Vec3(cosf( ii * sectionSlice ) * w, sinf( ii * sectionSlice ) * w, z));
			//assert( fabs( v.xyz.GetLengthSquared() - 1.0 ) < 1e-2/*1e-4*/ );		// because of FP-16 precision
            texCoordArray->push_back(osg::Vec2(ii / (float) c_numSections, 2.0f * (float) a / (float) c_numRings));
			
		}
	}

	// build faces
	for( unsigned int a = 0 ; a < c_numRings; ++a )
	{
		for( unsigned int i( 0 ); i < c_numSections; ++i )
		{
			skyDomeIndiceArray->push_back( (unsigned int) ( a * ( c_numSections + 1 ) + i + 1 ) );
			skyDomeIndiceArray->push_back( (unsigned int) ( a * ( c_numSections + 1 ) + i ) );
			skyDomeIndiceArray->push_back( (unsigned int) ( ( a + 1 ) * ( c_numSections + 1 ) + i + 1 ) );

			skyDomeIndiceArray->push_back( (unsigned int) ( ( a + 1 ) * ( c_numSections + 1 ) + i ) );
			skyDomeIndiceArray->push_back( (unsigned int) ( ( a + 1 ) * ( c_numSections + 1 ) + i + 1 ) );
			skyDomeIndiceArray->push_back( (unsigned int) ( a * ( c_numSections + 1 ) + i ) );
		}
	}

	// sanity checks
    assert( vertexArray->size() == c_numSkyDomeVertices );
    assert( skyDomeIndiceArray->size() == c_numSkyDomeIndices );

 


	//m_needRenderParamUpdate = 1;

        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	    osg::ref_ptr<osg::Geometry> geo = new osg::Geometry;

      
      

        geo->setVertexArray(vertexArray);
        geo->setTexCoordArray(0,texCoordArray, osg::Array::Binding::BIND_PER_VERTEX);
        geo->setNormalArray(normalArray,osg::Array::Binding::BIND_PER_VERTEX);
       
          // osg test normal
        /*{
             osg::ref_ptr<osg::Geometry> normalGeo = new osg::Geometry;
            osg::ref_ptr<osg::Vec3Array> normalLines = new osg::Vec3Array;

            for(int i = 0; i < vertexArray->size(); ++i)
            {
                normalLines->push_back(osg::Vec3(0,0,0));
                normalLines->push_back(vertexArray->at(i)*1.5);
            }
            normalGeo->setVertexArray(normalLines);
            normalGeo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,normalLines->size()));
            geode->addDrawable(normalGeo.get());
         }*/
       

      /*  colorArray->push_back(osg::Vec4(0,1,0,1.0));
        geo->setColorArray(colorArray,osg::Array::Binding::BIND_OVERALL);*/
	    ////设置颜色
	    //geo->setColorArray(color.get());
	    //geo->setColorBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);//设置颜色绑定

	    geo->addPrimitiveSet(skyDomeIndiceArray);
        
        geode->addDrawable(geo.get());
        {
           
           // geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere()));
        }

        {
                    
             osg::ref_ptr<osg::StateSet> ss = new osg::StateSet;
             std::string path = "F:\\Project\\2018\\LearnOsgExamples\\examples\\HDRSky\\HDRSky\\";
             std::string texMie = path + "SkyDomeMie.HDR";
             std::string texRayleigh = path + "SkyDomeRayleigh.HDR";
             std::string texMoom = path + "Moon.dds";

             osg::ref_ptr<osg::Texture2D> tex0 = new osg::Texture2D;
             osg::ref_ptr<osg::Texture2D> tex1 = new osg::Texture2D;
             osg::ref_ptr<osg::Texture2D> tex2 = new osg::Texture2D;
            
             osg::ref_ptr<osg::Image> img2 = osgDB::readImageFile(texMoom);

#if 0 // test default tex
             osg::ref_ptr<osg::Image> img0 = osgDB::readImageFile(texMie);
             osg::ref_ptr<osg::Image> img1 = osgDB::readImageFile(texRayleigh);
             tex0->setImage(img0);
             tex1->setImage(img1);
            // tex0 = m_pSkyDomeTextureMie;
             //tex1 = m_pSkyDomeTextureRayleigh;
#else
               tex0 = m_pSkyDomeTextureMie;
               tex1 = m_pSkyDomeTextureRayleigh;
#endif 
             tex2->setImage(img2);
         
            ss->setTextureAttributeAndModes(0,tex0.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
            ss->setTextureAttributeAndModes(1,tex1.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
            ss->setTextureAttributeAndModes(2,tex2.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
            {
                osg::ref_ptr<osg::Program> program = new osg::Program;
                std::string vsFile = path +"HDRSky.vert";
                std::string fsFile = path + "HDRSky.frag";
                osg::ref_ptr<osg::Shader> vsShader = osgDB::readRefShaderFile(osg::Shader::VERTEX,vsFile);
                osg::ref_ptr<osg::Shader> fsShader = osgDB::readRefShaderFile(osg::Shader::FRAGMENT,fsFile);
           
                program->addShader(vsShader);
                program->addShader(fsShader);
                ss->setAttribute(program);

            }
           // osg::ref_ptr<osg::Depth> depth = new osg::Depth;
           // depth->setRange(0.9,1.0);
           // depth->setWriteMask(false);
            //ss->setAttribute(depth);
            geode->setStateSet(ss);
        }
        return geode.release();

    }

 osg::ref_ptr<osg::Node> createQuad()
 
{
     osg::ref_ptr<osg::Geode> geode = new osg::Geode;
     osg::ref_ptr<osg::Geometry> geom=new osg::Geometry;
    
     //首先定义四个点
     osg::ref_ptr<osg::Vec3Array> v=new osg::Vec3Array;
     geom->setVertexArray(v.get());
     v->push_back(osg::Vec3(-100.f,0.f,-100.f));
     v->push_back(osg::Vec3(100.f,0.f,-100.f));
     v->push_back(osg::Vec3(100.f,0.f,100.f));
     v->push_back(osg::Vec3(-100.f,0.f,100.f));

     //定义颜色数组
     osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array;
     geom->setColorArray(c.get());
     geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
     c->push_back(osg::Vec4(1.f,0.f,0.f,1.f));
     c->push_back(osg::Vec4(0.f,1.f,0.f,1.f));
     c->push_back(osg::Vec4(0.f,0.f,1.f,1.f));
     c->push_back(osg::Vec4(1.f,1.f,1.f,1.f));
     //设置定点关联方式
     geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
     geode->addDrawable(geom);
     return geode;

 }

#include <osg/MatrixTransform>
 void HDRSky::Init()
 {
     // generate texture
     GenerateSkyDomeTextures(SSkyLightRenderParams::skyDomeTextureWidth, SSkyLightRenderParams::skyDomeTextureHeight);

     /*osg::Geode* geode = CreateHDRSkyDome();
     addChild(geode);*/
    // addChild(createQuad());
#if 1
     osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
     mt->setMatrix(osg::Matrix::scale(100,100,100));
#else 
 osg::ref_ptr<SkyBox> mt = new SkyBox;
#endif 
     mt->addChild(CreateHDRSkyDome());
     addChild(mt);
    // if(m_bDebug)
         m_debugNode =  createDebugView();//new osg::Node();//
     //addChild(m_debugNode.get());
    

 }

 void HDRSky::GenerateSkyDomeTextures(int width, int height)
 {
     m_pSkyDomeTextureMie = new osg::Texture2D;
     m_pSkyDomeTextureRayleigh = new osg::Texture2D;
     
     m_pSkyDomeTextureMie->setTextureSize(width,height);
     m_pSkyDomeTextureMie->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
     m_pSkyDomeTextureMie->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
     m_pSkyDomeTextureMie->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
     m_pSkyDomeTextureMie->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);
     m_pSkyDomeTextureMie->setInternalFormat( GL_RGBA16F_ARB );
     m_pSkyDomeTextureMie->setSourceType( GL_FLOAT );
     m_pSkyDomeTextureMie->setSourceFormat(GL_RGBA);

     m_pSkyDomeTextureRayleigh->setTextureSize(width,height);
     m_pSkyDomeTextureRayleigh->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
     m_pSkyDomeTextureRayleigh->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
     m_pSkyDomeTextureRayleigh->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
     m_pSkyDomeTextureRayleigh->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);
     m_pSkyDomeTextureRayleigh->setInternalFormat( GL_RGBA16F_ARB );
     m_pSkyDomeTextureRayleigh->setSourceType( GL_FLOAT );
     m_pSkyDomeTextureRayleigh->setSourceFormat(GL_RGBA);


     unsigned char* imageData = NULL;
     // todo udpate imageData
     const int bufSize = SSkyLightRenderParams::skyDomeTextureWidth*SSkyLightRenderParams::skyDomeTextureHeight*4*sizeof(float);
     imageData = new unsigned char[bufSize];

     memset(imageData,0,bufSize);
     if(imageData)
     {
         osg::Image* image = new osg::Image;
         image->setImage(width, height, 1,
             m_pSkyDomeTextureMie->getInternalFormat(), m_pSkyDomeTextureMie->getSourceFormat(),
             m_pSkyDomeTextureMie->getSourceType(), imageData, osg::Image::USE_NEW_DELETE );
         m_pSkyDomeTextureMie->setImage( 0, image );
     }
  
     imageData = new unsigned char[bufSize];
     if(imageData)
     {
         osg::Image* image = new osg::Image;
         image->setImage(width, height, 1,
             m_pSkyDomeTextureRayleigh->getInternalFormat(), m_pSkyDomeTextureRayleigh->getSourceFormat(),
             m_pSkyDomeTextureRayleigh->getSourceType(), imageData, osg::Image::USE_NEW_DELETE );
         m_pSkyDomeTextureRayleigh->setImage( 0, image );
     }

 }

 static void FillSkyTextureData(osg::Texture2D* pTexture, const void* pData, const int width, const int height, const unsigned int pitch)
{
    assert(pTexture && pTexture->getTextureWidth() == width && pTexture->getTextureHeight() == height);
	 
 
    osg::Image* img = pTexture->getImage(0);
    if(img)
    {
       // img->setData(pData,osg::Image::AllocationMode::USE_NEW_DELETE);
        unsigned char* pImgData = img->data();
        int n = img->getTotalDataSize();
        memcpy(pImgData,(unsigned char*)pData,width*height*sizeof(osg::Vec4));
        img->dirty();
    }
	 
}

 void HDRSky::Update()
 {
     
	{
		// re-create sky dome textures if necessary
		bool forceTextureUpdate(false);
		if (!m_pSkyDomeTextureMie|| ! m_pSkyDomeTextureRayleigh )
		{
			GenerateSkyDomeTextures(SSkyLightRenderParams::skyDomeTextureWidth, SSkyLightRenderParams::skyDomeTextureHeight);
			forceTextureUpdate = true;
		}

		//// dyn tex data lost due to device reset?
		//if (m_frameReset != rd->m_nFrameReset)
		//{
		//	forceTextureUpdate = true;
		//	m_frameReset = rd->m_nFrameReset;
		//}

        // todo 
        
		// update sky dome texture if new data is available
		if (m_skyDomeTextureLastTimeStamp != m_pRenderParams->m_skyDomeTextureTimeStamp || forceTextureUpdate)
		{
			FillSkyTextureData(m_pSkyDomeTextureMie, m_pRenderParams->m_pSkyDomeTextureDataMie, SSkyLightRenderParams::skyDomeTextureWidth, SSkyLightRenderParams::skyDomeTextureHeight, m_pRenderParams->m_skyDomeTexturePitch);
			FillSkyTextureData(m_pSkyDomeTextureRayleigh, m_pRenderParams->m_pSkyDomeTextureDataRayleigh, SSkyLightRenderParams::skyDomeTextureWidth, SSkyLightRenderParams::skyDomeTextureHeight, m_pRenderParams->m_skyDomeTexturePitch);

			// update time stamp of last update
			m_skyDomeTextureLastTimeStamp = m_pRenderParams->m_skyDomeTextureTimeStamp;
		}
	}
 }

 void HDRSky::UpdateUniform()
 {
     osg::StateSet* ss = getOrCreateStateSet();


     // shader constants -- phase function constants
     static std::string Param3Name("SkyDome_PartialMieInScatteringConst");
     static std::string Param4Name("SkyDome_PartialRayleighInScatteringConst");
     static std::string Param5Name("SkyDome_SunDirection");
     static std::string Param6Name("SkyDome_PhaseFunctionConstants");
     osg::Uniform* uf3 = ss->getOrCreateUniform(Param3Name,osg::Uniform::FLOAT_VEC4,1);
     uf3->set(m_pRenderParams->m_partialMieInScatteringConst);

     osg::Uniform* uf4 = ss->getOrCreateUniform(Param4Name,osg::Uniform::FLOAT_VEC4,1);
     uf4->set(m_pRenderParams->m_partialRayleighInScatteringConst);

     osg::Uniform* uf5 = ss->getOrCreateUniform(Param5Name,osg::Uniform::FLOAT_VEC4,1);
     uf5->set(m_pRenderParams->m_sunDirection );

     osg::Uniform* uf6 = ss->getOrCreateUniform(Param3Name,osg::Uniform::FLOAT_VEC4,1);
     uf6->set(m_pRenderParams->m_phaseFunctionConsts );


     // shader constants -- night sky relevant constants
	/*	osg::Vec3 nightSkyHorizonCol;
		p3DEngine->GetGlobalParameter( E3DPARAM_NIGHSKY_HORIZON_COLOR, nightSkyHorizonCol );
		Vec3 nightSkyZenithCol;
		p3DEngine->GetGlobalParameter( E3DPARAM_NIGHSKY_ZENITH_COLOR, nightSkyZenithCol );
		float nightSkyZenithColShift( p3DEngine->GetGlobalParameter( E3DPARAM_NIGHSKY_ZENITH_SHIFT ) );	
		const float minNightSkyZenithGradient( -0.1f );

		static CCryNameR Param7Name("SkyDome_NightSkyColBase");
		static CCryNameR Param8Name("SkyDome_NightSkyColDelta");
		static CCryNameR Param9Name("SkyDome_NightSkyZenithColShift");*/

		/*Vec4 nsColBase( nightSkyHorizonCol, 0 );
		ef->FXSetPSFloat(Param7Name, &nsColBase, 1 );
		Vec4 nsColDelta( nightSkyZenithCol - nightSkyHorizonCol, 0 );
		ef->FXSetPSFloat(Param8Name, &nsColDelta, 1 );
		Vec4 nsZenithColShift( 1.0f / ( nightSkyZenithColShift - minNightSkyZenithGradient ),  -minNightSkyZenithGradient / ( nightSkyZenithColShift - minNightSkyZenithGradient ) , 0, 0 );
		ef->FXSetPSFloat(Param9Name, &nsZenithColShift, 1 );

		{
			Vec3 mr;
			p3DEngine->GetGlobalParameter(E3DPARAM_SKY_MOONROTATION, mr);
			float moonLati = -gf_PI + gf_PI * mr.x / 180.0f;
			float moonLong = 0.5f * gf_PI - gf_PI * mr.y / 180.0f;
			{
				float sinLonR = sinf(-0.5f * gf_PI);
				float cosLonR = cosf(-0.5f * gf_PI);
				float sinLatR = sinf(moonLati + 0.5f * gf_PI);
				float cosLatR = cosf(moonLati + 0.5f * gf_PI);
				Vec3 moonTexGenRight(sinLonR * cosLatR, sinLonR * sinLatR, cosLonR);

				Vec4 nsMoonTexGenRight(moonTexGenRight, 0);
				static CCryNameR ParamName("SkyDome_NightMoonTexGenRight");
				ef->FXSetVSFloat(ParamName, &nsMoonTexGenRight, 1);
			}
			{
				float sinLonU = sinf(moonLong + 0.5f * gf_PI);
				float cosLonU = cosf(moonLong + 0.5f * gf_PI);
				float sinLatU = sinf(moonLati);
				float cosLatU = cosf(moonLati);
				Vec3 moonTexGenUp(sinLonU * cosLatU, sinLonU * sinLatU, cosLonU);

				Vec4 nsMoonTexGenUp(moonTexGenUp, 0);
				static CCryNameR ParamName("SkyDome_NightMoonTexGenUp");
				ef->FXSetVSFloat(ParamName, &nsMoonTexGenUp, 1);
			}
		}

		Vec3 nightMoonDirection;
		p3DEngine->GetGlobalParameter( E3DPARAM_NIGHSKY_MOON_DIRECTION, nightMoonDirection );
		float nightMoonSize( 25.0f - 24.0f * clamp_tpl( p3DEngine->GetGlobalParameter( E3DPARAM_NIGHSKY_MOON_SIZE ), 0.0f, 1.0f ) );
		Vec4 nsMoonDirSize( nightMoonDirection, nightMoonSize );
		static CCryNameR Param10Name("SkyDome_NightMoonDirSize");
		ef->FXSetVSFloat(Param10Name, &nsMoonDirSize, 1 );
		ef->FXSetPSFloat(Param10Name, &nsMoonDirSize, 1 );

		Vec3 nightMoonColor;
		p3DEngine->GetGlobalParameter( E3DPARAM_NIGHSKY_MOON_COLOR, nightMoonColor );
		Vec4 nsMoonColor( nightMoonColor, 0 );
		static CCryNameR Param11Name("SkyDome_NightMoonColor");
		ef->FXSetPSFloat(Param11Name, &nsMoonColor, 1 );

		Vec3 nightMoonInnerCoronaColor;
		p3DEngine->GetGlobalParameter( E3DPARAM_NIGHSKY_MOON_INNERCORONA_COLOR, nightMoonInnerCoronaColor );
		float nightMoonInnerCoronaScale( 1.0f + 1000.0f * p3DEngine->GetGlobalParameter( E3DPARAM_NIGHSKY_MOON_INNERCORONA_SCALE ) );	
		Vec4 nsMoonInnerCoronaColorScale( nightMoonInnerCoronaColor, nightMoonInnerCoronaScale );
		static CCryNameR Param12Name("SkyDome_NightMoonInnerCoronaColorScale");
		ef->FXSetPSFloat(Param12Name, &nsMoonInnerCoronaColorScale, 1 );

		Vec3 nightMoonOuterCoronaColor;
		p3DEngine->GetGlobalParameter( E3DPARAM_NIGHSKY_MOON_OUTERCORONA_COLOR, nightMoonOuterCoronaColor );
		float nightMoonOuterCoronaScale( 1.0f + 1000.0f * p3DEngine->GetGlobalParameter( E3DPARAM_NIGHSKY_MOON_OUTERCORONA_SCALE ) );
		Vec4 nsMoonOuterCoronaColorScale( nightMoonOuterCoronaColor, nightMoonOuterCoronaScale );
		static CCryNameR Param13Name("SkyDome_NightMoonOuterCoronaColorScale");
		ef->FXSetPSFloat(Param13Name, &nsMoonOuterCoronaColorScale, 1 );*/

 }

 void HDRSky::SetEnableDebug(bool bDebug)
 { 
     m_bDebug = bDebug;
     if(bDebug)
     {
       m_debugNode->setNodeMask(0xFFFFFFFF);
     }
     else
     {
         m_debugNode->setNodeMask(0x0);

     }
 }

 osg::Node* HDRSky::createDebugView()
 {
     osg::ref_ptr<osg::Camera> camera = new osg::Camera;
     camera->setRenderOrder(osg::Camera::POST_RENDER,19);
     camera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
     camera->setClearMask(GL_DEPTH_BUFFER_BIT);
     // camera->setProjectionMatrixAsOrtho2D(0,1000,0,1000);
     // camera->setProjectionMatrix(osg::Matrix::ortho2D(0,600,0,600));//正交投影  
     osg::Node* texMieNode = osg::createTexturedQuadGeometry(osg::Vec3(-0.5,-0.5,0),osg::Vec3(0.5,0,0),osg::Vec3(0.0,0.5,0.0));
     osg::Node* texRayleighNode = osg::createTexturedQuadGeometry(osg::Vec3(0.5,-0.5,0),osg::Vec3(0.5,0,0),osg::Vec3(0.0,0.5,0.0));

     camera->setProjectionMatrix( osg::Matrix::ortho2D(-2.0, 2.0, -2.0, 2.0) );
     camera->setViewMatrix( osg::Matrix::identity() );
     //camera->addChild( createScreenQuad(1.0, 1.0) );
     camera->addChild(texMieNode);
     camera->addChild(texRayleighNode);
     camera->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);
     texMieNode->getOrCreateStateSet()->setTextureAttributeAndModes(0,m_pSkyDomeTextureMie);
     texRayleighNode->getOrCreateStateSet()->setTextureAttributeAndModes(0,m_pSkyDomeTextureRayleigh);


     //camera->setAllowEventFocus( false );



     //osg::ref_ptr<osg::Node> axisNode = osgDB::readNodeFile("cow.osgt");
     //camera->addChild(axisNode);

     // return axisNode.release();
     return camera.release();

 }
