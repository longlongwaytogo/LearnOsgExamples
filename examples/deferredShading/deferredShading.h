#ifndef __DEFERRED_SHADING_H__
#define __DEFERRED_SHADING_H__

// reference from  https://bitbucket.org/kornerr/osg-deferred-shading/downloads/ by  Michael Kapelko

#include <osg/Camera>
#include <osg/Group>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/LightSource>
#include <osgShadow/ShadowedScene>

 #define  USE_EFFECT_COMPOSITOR
   
    struct Pipeline
    {

        int _textureSize; // 纹理大小
        osg::ref_ptr<osg::Group>  _graph;
        osg::Texture* _pass1Shadows;        // 阴影通道
        osg::Texture* _pass2Colors;         // 颜色通道，即：diffuse
        osg::Texture* _pass2Normals;        // 法线
        osg::Texture* _pass2Positions;      // 位置
        osg::Texture* _pass3Final;          // 合成通道
    };
    class DeferredShading
    {
  
    public:

        /* 创建Float类型的纹理区域
        */
        static osg::TextureRectangle* createFloatTextureRectangle(int textureSize);
    
        /* 创建Hud相机
        */
        static osg::Camera* createHudCamera(double left = 0, double right = 1.0,
                                        double bottom = 0, double top = 1.0 );
        /* 创建灯光
        */
        static osg::ref_ptr<osg::LightSource> createLight(const osg::Vec3& pos);

        /* 创建渲染合成器
        @scene：场景根节点
        @shadowScene： 阴影节点
        @lightPos：灯光位置
        */
        static Pipeline createPipelineEffectCompositor(osg::ref_ptr<osg::Group> scene,
                        osg::ref_ptr<osgShadow::ShadowedScene> shadowScene,
                        const osg::Vec3 lightPos);

        /* 创建渲染平台
        @scene：场景根节点
        @shadowScene： 阴影节点
        @lightPos：灯光位置
        */
        //Pipeline createPipelinePlainOSG(osg::ref_ptr<osg::Group> scene, osg::ref_ptr<osgShadow::ShadowedScene> shadowScene, const osg::Vec3 lightPos)
       static  Pipeline createPipelinePlainOSG(osg::ref_ptr<osg::Group> scene,
                        osg::ref_ptr<osgShadow::ShadowedScene> shadowScene,
                        const osg::Vec3 lightPos);
    
        /* 创建RTT(Render to Target) 相机
        */
        static osg::Camera* createRTTCamera(osg::Camera::BufferComponent buffer,
                        osg::Texture* tex,
                        bool isAbsolute = false);
   
        /* 创建四边形区域
        */
        static osg::Geode* createScreenQuad(float width, float height,
                        float scale = 1, osg::Vec3 corner = osg::Vec3());
        /* 创建纹理
        */
        static osg::Texture2D * createTexture(const std::string& fileName);

        /* 创建用于显示到屏幕的纹理
        */
        static osg::ref_ptr<osg::Camera> createTextureDisplayQuad(const osg::Vec3& pos,
                        osg::StateAttribute* tex, float scale,
                        float width = 0.3, float height = 0.2);

        /* 设置着色器程序
        */
        static osg::ref_ptr<osg::StateSet> setShaderProgram(osg::ref_ptr<osg::Camera> pass,
                        std::string vert, std::string frag);

    };

 

#endif // __DEFERRED_SHADING_H__