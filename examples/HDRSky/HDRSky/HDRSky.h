#ifndef __RENDER_HDRSKY_H__
#define __RENDER_HDRSKY_H__

#include <osg/Referenced> 
#include <osg/Node>
#include <osg/Geometry>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/vec4>
#include <osg/Vec4b>
#include <assert.h>
#include <osg/Group>
#include <osg/Geode>    

 
 
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
private:
	void Init();

private:
	int m_skyDomeTextureLastTimeStamp;
	int m_frameReset;
	//class CStars* m_pStars;
};


#endif  // __CRESKY_H__
