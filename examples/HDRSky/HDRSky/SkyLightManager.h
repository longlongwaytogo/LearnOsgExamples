
#ifndef _SKY_LIGHT_MANAGER_H_
#define _SKY_LIGHT_MANAGER_H_

#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <osg/Referenced>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include "HDRSkyComm.h"
#include "SkyLightNishita.h"
#include <osgViewer/Viewer>
#include <OpenThreads\Thread>
#include <OpenThreads\Block>
#include "HDRSky.h"

class SkyLightNishita;
 

class UpdateThread;
class SkyLightManager : public osg::Referenced
{

public:

    // 对外接口，暂时放到SkyLightManager中，后续可能移出去
     void SetSkyLightParameters( const osg::Vec3& sunDir, const osg::Vec3& sunIntensity, float Km, float Kr, float g, const osg::Vec3& rgbWaveLengths, bool forceImmediateUpdate )
    {
         SSkyDomeCondition skyCond;

        skyCond.m_g = g;
        skyCond.m_Km = Km;
        skyCond.m_Kr = Kr;
        skyCond.m_sunIntensity = sunIntensity;
        skyCond.m_rgbWaveLengths = rgbWaveLengths;
        skyCond.m_sunDirection = sunDir;

        SetSkyDomeCondition( skyCond );
        if (forceImmediateUpdate)
            FullUpdate();
    }

public:
    SkyLightManager(osgViewer::Viewer* viewer);
    ~SkyLightManager();

    // sky dome condition
    void SetSkyDomeCondition( const SSkyDomeCondition& skyDomeCondition );
    void GetCurSkyDomeCondition( SSkyDomeCondition& skyCond ) const;

    // controls updates
    void FullUpdate();
    void IncrementalUpdate( float updateRatioPerFrame );
    void SetQuality( int quality );

    // rendering params
    const SSkyLightRenderParams* GetRenderParams() const;

    

    void InitSkyDomeMesh();
    void ReleaseSkyDomeMesh(); 

    void SyncFullUpdate();

    void UpdateInternal( int newFrameID, int numUpdates, int callerIsFullUpdate = 0 );
private:
    typedef std::vector<osg::Vec4> SkyDomeTextureData;

private:	
    bool IsSkyDomeUpdateFinished() const;

    int GetFrontBuffer() const;
    int GetBackBuffer() const;
    void ToggleBuffer();
    void UpdateRenderParams();
    void PushUpdateParams();

private:
    SSkyDomeCondition m_curSkyDomeCondition;			//current sky dome conditions
    SSkyDomeCondition m_reqSkyDomeCondition[2];		//requested sky dome conditions, double buffered(engine writes async)
    SSkyDomeCondition m_updatingSkyDomeCondition; //sky dome conditions the update is currently processed with
    int m_updateRequested[2];										//true if an update is requested, double buffered(engine writes async)
    osg::ref_ptr<SkyLightNishita> m_pSkyLightNishita;

    SkyDomeTextureData m_skyDomeTextureDataMie[ 2 ];
    SkyDomeTextureData m_skyDomeTextureDataRayleigh[ 2 ];
    int m_skyDomeTextureTimeStamp[ 2 ];

    bool m_bFlushFullUpdate;

    //IRenderMesh* m_pSkyDomeMesh;

    int m_numSkyDomeColorsComputed;
    int m_curBackBuffer;

    int m_lastFrameID;
    int m_needRenderParamUpdate;

    osg::Vec3 m_curSkyHemiColor[5];
    osg::Vec3 m_curHazeColor;
    osg::Vec3 m_curHazeColorMieNoPremul;
    osg::Vec3 m_curHazeColorRayleighNoPremul;

    osg::Vec3 m_skyHemiColorAccum[5];
    osg::Vec3 m_hazeColorAccum;
    osg::Vec3 m_hazeColorMieNoPremulAccum;
    osg::Vec3 m_hazeColorRayleighNoPremulAccum;

    SSkyLightRenderParams m_renderParams;

    osgViewer::Viewer* m_pViewer;
    UpdateThread* m_updateThread;
};


class UpdateThread :public OpenThreads::Thread
{
public:
    UpdateThread(SkyLightManager* skyLightManger);
   virtual void run();
 
void   aliveThread(int frameNum, int TexSize, int callerIsFullUpdate );

private:
   osg::ref_ptr<SkyLightManager> m_skyLightManager;
   bool m_bRun;
};


class SkyLightEventHandler :public osgGA::GUIEventHandler
{
public:
    SkyLightEventHandler(SkyLightManager* skyLightManager)
    {
        _skyLightManager = skyLightManager;
        _sunDir = osg::Vec3( 0.0f, 0.707106f, 0.707106f );
    }
 virtual bool SkyLightEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*);
private:
    osg::ref_ptr<SkyLightManager> _skyLightManager;
    osg::Vec3 _sunDir;
};


//  用于更新纹理数据
 
//class HDRSky;
 class UpdateTexCallBack: public osg::NodeCallback
 {
     osg::ref_ptr<SkyLightManager> _slm;
 public:
     void setSkyLightManager(SkyLightManager* slm)
     {
         _slm = slm;
     }
     
        /** Callback method called by the NodeVisitor when visiting a node.*/
        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
        {
            HDRSky* pHDRSky = dynamic_cast<HDRSky*>(node);
            if(pHDRSky)
            {
                _slm->IncrementalUpdate(1);
                pHDRSky->Update();
              
            }

        }
 };

#endif // #ifndef _SKY_LIGHT_MANAGER_H_
