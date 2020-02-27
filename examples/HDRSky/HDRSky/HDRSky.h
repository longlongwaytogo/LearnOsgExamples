#ifndef __RENDER_HDRSKY_H__
#define __RENDER_HDRSKY_H__

#include <osg/Referenced> 
#include <osg/ref_ptr>
#include <osg/Node>
#include <osg/Geometry>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/vec4>
#include <osg/Vec4b>
#include <assert.h>
#include <osg/Group>
#include <osg/Geode>    
#include <osg/Texture2D>
#include "HDRSkyComm.h"

 
 
class HDRSky : public osg::Group
{
public:
	HDRSky();
	virtual ~HDRSky();
	//virtual void mfPrepare(bool bCheckOverflow);
	 
	/*virtual float mfDistanceToCameraSquared(Matrix34& matInst);
 

	void GenerateSkyDomeTextures(int32 width, int32 height);*/

public:
	/*const SSkyLightRenderParams* m_pRenderParams;*/
	int m_moonTexId;
	/*class CTexture* m_pSkyDomeTextureMie;
	class CTexture* m_pSkyDomeTextureRayleigh;*/
    osg::Geode* CreateHDRSkyDome();

    void UpdateUniform();
    void Update();
    void GenerateSkyDomeTextures(int width, int height);
    void SetEnableDebug(bool bDebug);
    bool IsEnableDebug(){ return m_bDebug;}
    osg::Node* getDebugNode() { return m_debugNode;}
private:
	void Init();
    osg::Node* createDebugView();
     

public:
	int m_skyDomeTextureLastTimeStamp;
	int m_frameReset;

    osg::ref_ptr<osg::Texture2D> m_pSkyDomeTextureMie;
    osg::ref_ptr<osg::Texture2D> m_pSkyDomeTextureRayleigh;
	//class CStars* m_pStars;

    const SSkyLightRenderParams* m_pRenderParams;
    bool m_bDebug;
    osg::ref_ptr<osg::Node> m_debugNode;
};




#endif  // __CRESKY_H__
