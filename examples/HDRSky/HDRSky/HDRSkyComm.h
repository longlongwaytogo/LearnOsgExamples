#ifndef __RENDER_HDRSKYCOMM_H__
#define __RENDER_HDRSKYCOMM_H__

#include <osg/Vec3>
#include <osg/Vec4>

struct SSkyLightRenderParams
{
    static const int skyDomeTextureWidth = 64;
    static const int skyDomeTextureHeight =32;
    static const int skyDomeTextureSize = 64 * 32;

    static const int skyDomeTextureWidthBy8 = 8;
    static const int skyDomeTextureWidthBy4Log = 4; // = log2(64/4)
    static const int skyDomeTextureHeightBy2Log = 4; // = log2(32/2)

    SSkyLightRenderParams()
      //  : m_pSkyDomeMesh( 0 ),
        : m_pSkyDomeTextureDataMie( 0 )
        , m_pSkyDomeTextureDataRayleigh( 0 )
        , m_skyDomeTexturePitch( 0 )
        , m_skyDomeTextureTimeStamp( -1 )
        , m_partialMieInScatteringConst( 0.0f, 0.0f, 0.0f, 0.0f )
        , m_partialRayleighInScatteringConst( 0.0f, 0.0f, 0.0f, 0.0f )
        , m_sunDirection( 0.0f, 0.0f, 0.0f, 0.0f )
        , m_phaseFunctionConsts( 0.0f, 0.0f, 0.0f, 0.0f )
        , m_hazeColor( 0.0f, 0.0f, 0.0f, 0.0f )
        , m_hazeColorMieNoPremul( 0.0f, 0.0f, 0.0f, 0.0f )
        , m_hazeColorRayleighNoPremul( 0.0f, 0.0f, 0.0f, 0.0f )
        , m_skyColorTop( 0.0f, 0.0f, 0.0f )
        , m_skyColorNorth( 0.0f, 0.0f, 0.0f )
        , m_skyColorEast( 0.0f, 0.0f, 0.0f )
        , m_skyColorSouth( 0.0f, 0.0f, 0.0f )
        , m_skyColorWest( 0.0f, 0.0f, 0.0f )
    {
    }

    // Sky dome mesh
   // IRenderMesh* m_pSkyDomeMesh;

    // temporarily add padding bytes to prevent fetching Vec4 constants below from wrong offset
    unsigned int dummy0;
    unsigned int dummy1;

    // Sky dome texture data
    const void* m_pSkyDomeTextureDataMie;
    const void* m_pSkyDomeTextureDataRayleigh;
    size_t m_skyDomeTexturePitch;
    int m_skyDomeTextureTimeStamp;

    int pad;//Enable 16 byte alignment for Vec4s

    // Sky dome shader constants
    osg::Vec4 m_partialMieInScatteringConst;
    osg::Vec4 m_partialRayleighInScatteringConst;
    osg::Vec4 m_sunDirection;
    osg::Vec4 m_phaseFunctionConsts;
    osg::Vec4 m_hazeColor;
    osg::Vec4 m_hazeColorMieNoPremul;
    osg::Vec4 m_hazeColorRayleighNoPremul;

    // Sky hemisphere colors
    osg::Vec3 m_skyColorTop;
    osg::Vec3 m_skyColorNorth;
    osg::Vec3 m_skyColorEast;
    osg::Vec3 m_skyColorSouth;
    osg::Vec3 m_skyColorWest;
};


struct SSkyDomeCondition
{
    SSkyDomeCondition()
        : m_sunIntensity( 20.0f, 20.0f, 20.0f )
        , m_Km( 0.001f )
        , m_Kr( 0.00025f )
        , m_g( -0.99f )
        , m_rgbWaveLengths( 650.0f, 570.0f, 475.0f )
        , m_sunDirection( 0.0f, 0.707106f, 0.707106f )
    {
    }

    osg::Vec3 m_sunIntensity;
    float m_Km;
    float m_Kr;
    float m_g;
    osg::Vec3 m_rgbWaveLengths;
    osg::Vec3 m_sunDirection;
};


#endif // __RENDER_HDRSKYCOMM_H__