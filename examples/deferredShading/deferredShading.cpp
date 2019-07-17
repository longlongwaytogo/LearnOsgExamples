#include "deferredShading.h"
#include <Utils.h>
#include <osg/Vec4>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgDB/ReadFile>


#ifdef USE_EFFECT_COMPOSITOR
//#include <osgFX/EffectCompositor>
#include "../EffectCompositor/EffectCompositor"
#endif 

    osg::TextureRectangle* DeferredShading::createFloatTextureRectangle(int textureSize)
    {
        osg::ref_ptr<osg::TextureRectangle> tex2D = new osg::TextureRectangle;
        tex2D->setTextureSize(textureSize,textureSize);
        tex2D->setInternalFormat(GL_RGBA16F_ARB);
        tex2D->setSourceFormat(GL_RGBA);
        tex2D->setSourceType(GL_FLOAT);
        return tex2D.release();
    }

    osg::Camera* DeferredShading::createHudCamera(double left /*= 0*/, double right /*= 1.0*/, double bottom /*= 0*/, double top /*= 1.0 */)
    {
        osg::ref_ptr<osg::Camera> camera = new osg::Camera;
        camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        camera->setClearMask(GL_DEPTH_BUFFER_BIT);
        camera->setRenderOrder(osg::Camera::POST_RENDER);
        camera->setAllowEventFocus(false);
        camera->setProjectionMatrix(osg::Matrix::ortho2D(left,right,bottom,top));
        camera->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        return camera.release();
    }

    osg::ref_ptr<osg::LightSource> DeferredShading::createLight(const osg::Vec3& pos)
    {
        osg::ref_ptr<osg::LightSource> light = new osg::LightSource;
        light->getLight()->setPosition(osg::Vec4(pos.x(),pos.y(),pos.z(),1));
        light->getLight()->setAmbient(osg::Vec4(0.2,0.2,0.2,1));
        light->getLight()->setDiffuse(osg::Vec4(0.8,0.8,0.8,1));
        return light;
    }

    Pipeline DeferredShading::createPipelineEffectCompositor(osg::ref_ptr<osg::Group> scene, osg::ref_ptr<osgShadow::ShadowedScene> shadowScene, const osg::Vec3 lightPos)
    {
        Pipeline p;
#ifdef USE_EFFECT_COMPOSITOR
         std::string mediaPath = Utils::Helper::Ins().getMediaPath();
        p._graph = new osg::Group;
        
        // Pass 1 
        osgFX::EffectCompositor* pass1 = osgFX::readEffectFile( mediaPath + "pass1.xml");
        p._pass1Shadows = pass1->getTexture("pass1Shadows");

        // Take texture size from the effect compositor
        // Width must be equal to height.
        p._textureSize = p._pass1Shadows->getTextureWidth();
        pass1->addChild(shadowScene.get()); // 将场景添加到阴影节点中

        // Pass 2 （positions ,normal, colors) and 3 (final combination)
        osgFX::EffectCompositor* pass2_3 = osgFX::readEffectFile(mediaPath + "passes2_3.xml");
        pass2_3->addChild(scene.get()); // 添加场景节点
        osgFX::EffectCompositor::PassData pass3;
        pass2_3->getPassData("pass3",pass3);

        // Pass 1st pass texture to the 3rd pass
        // 将第一pass传递给第三个pass
        osg::StateSet* ss = pass3.pass->getOrCreateStateSet();
        ss->setTextureAttributeAndModes(3,p._pass1Shadows);
        ss->addUniform(new osg::Uniform("shadowMap",3)); // 传递阴影图到第三号纹理单元中

        // Light position
        ss->addUniform(new osg::Uniform("lightPos",lightPos));

        // Graph 
        p._graph->addChild(pass1);
        p._graph->addChild(pass2_3);

        // Textures 
        p._pass2Colors = pass2_3->getTexture("pass2Colors");
        p._pass2Normals = pass2_3->getTexture("pass2Normals");
        p._pass2Positions = pass2_3->getTexture("pass2Positions");
        p._pass3Final = pass2_3->getTexture("pass3Final");
#endif 
        return p;
    }


    Pipeline DeferredShading::createPipelinePlainOSG(osg::ref_ptr<osg::Group> scene, osg::ref_ptr<osgShadow::ShadowedScene> shadowScene, const osg::Vec3 lightPos)
    {
        Pipeline p;
        p._graph = new osg::Group;
        p._textureSize = 1024;

        // Pass 1(shadow)
        p._pass1Shadows = createFloatTextureRectangle(p._textureSize);
        osg::ref_ptr<osg::Camera> pass1 = createRTTCamera(osg::Camera::COLOR_BUFFER, p._pass1Shadows);
        pass1->addChild(shadowScene.get());

        // Pass2 (positions normals colors) 多目标渲染 RMT
        p._pass2Positions = createFloatTextureRectangle(p._textureSize);
        p._pass2Normals = createFloatTextureRectangle(p._textureSize);
        p._pass2Colors = createFloatTextureRectangle(p._textureSize);
        osg::ref_ptr<osg::Camera> pass2 = createRTTCamera(osg::Camera::COLOR_BUFFER0,p._pass2Positions);
        pass2->attach(osg::Camera::COLOR_BUFFER1, p._pass2Normals);
        pass2->attach(osg::Camera::COLOR_BUFFER2, p._pass2Colors);
        pass2->addChild(scene.get());

        std::string mediaPath = Utils::Helper::Ins().getMediaPath();
        std::string pass2_vert_path = mediaPath + "osg_deferred_shading/pass2.vert";
        std::string pass2_frag_path = mediaPath + "osg_deferred_shading/pass2.frag";

        // 设置着色器
        osg::StateSet* ss = setShaderProgram(pass2,pass2_vert_path,pass2_frag_path);
        std::string rockWall_path = mediaPath + "osg_deferred_shading/rockwall.png";
        std::string rockWall_NH_path = mediaPath + "osg_deferred_shading/rockwall_NH.png";

        // 设置图片资源
        ss->setTextureAttributeAndModes(0, createTexture(rockWall_path));
        ss->setTextureAttributeAndModes(1, createTexture(rockWall_NH_path)); // 凹凸贴图

        // 添加Uniform 变量,并初始化
        ss->addUniform(new osg::Uniform("diffMap", 0));
        ss->addUniform(new osg::Uniform("bumpMap", 1));

        // Pass3 (final)
        p._pass3Final = createFloatTextureRectangle(p._textureSize);
        osg::ref_ptr<osg::Camera> pass3 = createRTTCamera(osg::Camera::COLOR_BUFFER,p._pass3Final,true);
        
        // 设置着色器
        std::string pass3_vert_path = mediaPath + "osg_deferred_shading/pass3.vert";
        std::string pass3_frag_path = mediaPath + "osg_deferred_shading/pass3.frag";
        ss = setShaderProgram(pass3, pass3_vert_path, pass3_frag_path);
        ss->setTextureAttributeAndModes(0,p._pass2Positions);
        ss->setTextureAttributeAndModes(1,p._pass2Normals);
        ss->setTextureAttributeAndModes(2,p._pass2Colors);
        ss->setTextureAttributeAndModes(3,p._pass1Shadows);
        ss->addUniform(new osg::Uniform("posMap",0));       // 位置图
        ss->addUniform(new osg::Uniform("normalMap",1));    // 发现图
        ss->addUniform(new osg::Uniform("colorMap",2));
        ss->addUniform(new osg::Uniform("shadowMap",3));

        // Light position
        ss->addUniform(new osg::Uniform("lightPos",lightPos));

        // Graph
        p._graph->addChild(pass1);  // 绘制阴影图
        p._graph->addChild(pass2);  // 绘制Gbuffer
        p._graph->addChild(pass3);  // 绘制最终场景

        return p;
    }

    osg::Camera* DeferredShading::createRTTCamera(osg::Camera::BufferComponent buffer, osg::Texture* tex, bool isAbsolute /*= false*/)
    {
        osg::ref_ptr<osg::Camera> camera = new osg::Camera;
        camera->setClearColor(osg::Vec4());
        camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
        camera->setRenderOrder(osg::Camera::PRE_RENDER);
   
        if(tex)
        {
            tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
            tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
            camera->setViewport(0, 0, tex->getTextureWidth(), tex->getTextureHeight());
            camera->attach(buffer, tex);
        }

        if(isAbsolute) // 如果是绝对引用
        {
            camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
            camera->setProjectionMatrix(osg::Matrix::ortho2D(0.0, 1.0, 0.0, 1.0));
            camera->setViewMatrix(osg::Matrix::identity());
            camera->addChild(createScreenQuad(1.0f,1.0f)); 
        }
        return camera.release();
    }

    osg::Geode* DeferredShading::createScreenQuad(float width, float height, float scale /*= 1*/, osg::Vec3 corner /*= osg::Vec3()*/)
    {
        osg::Geometry* geometry = osg::createTexturedQuadGeometry(corner,osg::Vec3(width,0,0),osg::Vec3(0,height,0),0,0,scale,scale);
        osg::ref_ptr<osg::Geode> quad = new osg::Geode;
        quad->addDrawable(geometry);

        int values = osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED;
        quad->getOrCreateStateSet()->setMode(GL_LIGHTING,values);
        return quad.release();

    }

    osg::Texture2D * DeferredShading::createTexture(const std::string& fileName)
    {
        osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;
        osg::Image* image = osgDB::readImageFile(fileName);
        if(image)
            tex->setImage(image);

        tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
        tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
        tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
        tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
        return tex.release();
    }

    osg::ref_ptr<osg::Camera> DeferredShading::createTextureDisplayQuad(const osg::Vec3& pos, osg::StateAttribute* tex, float scale, float width /*= 0.3*/, float height /*= 0.2*/)
    {
        osg::ref_ptr<osg::Camera> camera = createHudCamera();
        camera->addChild(createScreenQuad(width,height,scale,pos));
        camera->getOrCreateStateSet()->setTextureAttributeAndModes(0,tex);
        return camera;
    }

    osg::ref_ptr<osg::StateSet> DeferredShading::setShaderProgram(osg::ref_ptr<osg::Camera> pass, std::string vert, std::string frag)
    {
        osg::ref_ptr<osg::Program> program = new osg::Program;
        osg::Shader* vertShader = new osg::Shader(osg::Shader::VERTEX);
        vertShader->loadShaderSourceFromFile(vert);
        osg::Shader* fragShader = new osg::Shader(osg::Shader::FRAGMENT);
        fragShader->loadShaderSourceFromFile(frag);

        program->addShader(vertShader);
        program->addShader(fragShader);

        osg::ref_ptr<osg::StateSet> ss = pass->getOrCreateStateSet();
        ss->setAttribute(program.get(),osg::StateAttribute::OVERRIDE| osg::StateAttribute::ON);

        return ss;
    }



