// This is public domain software and comes with
// absolutely no warranty. Use of public domain software
// may vary between counties, but in general you are free
// to use and distribute this software for any purpose.


// Example: OSG using an OpenGL 3.0 context.
// The comment block at the end of the source describes building OSG
// for use with OpenGL 3.x.

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/GraphicsContext>
#include <osg/Camera>
#include <osg/Viewport>
#include <osg/StateSet>
#include <osg/Program>
#include <osg/Shader>
#include <osgUtil/Optimizer>
#include <osg/Geometry>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/DispatchCompute>
#include <osg/BindImageTexture>
#pragma comment(lib, "opengl32.lib")

bool configGL3Camera(osgViewer::Viewer* viewer,int w = 800, int h = 600)
{
    const std::string version("4.3");
    osg::ref_ptr< osg::GraphicsContext::Traits > traits = new osg::GraphicsContext::Traits();
    traits->x = 100; traits->y = 100;
    traits->width = w; traits->height = h;
    traits->windowDecoration = true;
    traits->doubleBuffer = true;
    traits->glContextVersion = version;
    traits->glContextProfileMask = GL_CONTEXT_COMPATIBILITY_PROFILE_BIT;// 0x1;// 
    traits->readDISPLAY();
    traits->setUndefinedScreenDetailsToDefaultScreen();
    osg::ref_ptr< osg::GraphicsContext > gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    if (!gc.valid())
    {
        osg::notify(osg::FATAL) << "Unable to create OpenGL v" << version << " context." << std::endl;
        return false;
    }

    // Create a Camera that uses the above OpenGL context.
    osg::Camera* cam = viewer->getCamera();
    cam->setGraphicsContext(gc.get());
    gc->getState()->resetVertexAttributeAlias(false);
    // Must set perspective projection for fovy and aspect.
    cam->setProjectionMatrix(osg::Matrix::perspective(45., (double)w / (double)h, 1., 1000.));
    // Unlike OpenGL, OSG viewport does *not* default to window dimensions.
    cam->setViewport(new osg::Viewport(0, 0, w, h));
    return true;
}

osg::Camera* createRttCamera(int w, int h, std::map<int, osg::ref_ptr<osg::Texture2D>> attachedTextures)
{
    osg::ref_ptr<osg::Camera> pCamera = new osg::Camera;
    pCamera->setClearColor(osg::Vec4(0.1f, 0.1f, 0.3f, 1.0f));
    pCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pCamera->setRenderOrder(osg::Camera::PRE_RENDER);
    pCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    for (auto itr = attachedTextures.begin(); itr != attachedTextures.end(); ++itr)
    {
        if (itr->second)
            pCamera->attach(osg::Camera::BufferComponent(itr->first), itr->second);
    }
    pCamera->setReferenceFrame(osg::Transform::RELATIVE_RF);
    return pCamera.release();
}

void configureHudShaders(osg::StateSet* stateSet)
{
    const std::string vertexSource =
        "#version 430 compatibility \n"
        " \n"
        "uniform mat4 osg_ModelViewProjectionMatrix; \n"
        "in vec4 osg_Vertex; \n"
        "in vec3 osg_Normal; \n"
        "out vec4 color; \n"
        "out vec2 texCoord;\n"
        " \n"
        "void main() \n"
        "{ \n"
        "    color = vec4( 1.0); \n"
        "   texCoord = gl_MultiTexCoord0.xy;\n"
        "    gl_Position = osg_ModelViewProjectionMatrix * osg_Vertex; \n"
        "} \n";
    osg::Shader* vShader = new osg::Shader(osg::Shader::VERTEX, vertexSource);

    const std::string fragmentSource =
        "#version 430 compatibility \n"
        " \n"
        "uniform sampler2D tex; \n"
        "in vec4 color; \n"
        "in vec2 texCoord;\n"
        "out vec4 fragData; \n"
        " \n"
        "void main() \n"
        "{ \n"
        "    fragData = texture(tex,texCoord); \n"
        "} \n";
    osg::Shader* fShader = new osg::Shader(osg::Shader::FRAGMENT, fragmentSource);

    osg::Program* program = new osg::Program;
    program->addShader(vShader);
    program->addShader(fShader);
    stateSet->setAttribute(program);

    osg::Vec3f lightDir(0., 0.5, 1.);
    lightDir.normalize();
    stateSet->addUniform(new osg::Uniform("ecLightDir", lightDir));
}

void configureShaders(osg::StateSet* stateSet)
{
    const std::string vertexSource =
        "#version 430 compatibility \n"
        " \n"
        "uniform mat4 osg_ModelViewProjectionMatrix; \n"
        "uniform mat4 osg_ViewMatrixInverse;\n"
        "uniform mat4 osg_ModelViewMatrix;\n"
        "uniform mat3 osg_NormalMatrix; \n"
        "uniform vec3 ecLightDir; \n"
        "uniform vec4 clip0;\n"
        " \n"
        "in vec4 osg_Vertex; \n"
        "in vec3 osg_Normal; \n"
        "out vec4 color; \n"
        " \n"
        "void main() \n"
        "{ \n"
        "    vec3 ecNormal = normalize( osg_NormalMatrix * osg_Normal ); \n"
        "    float diffuse = max( dot( ecLightDir, ecNormal ), 0. ); \n"
        "    color = vec4( vec3( diffuse ), 1. ); \n"
        " \n"
        "    gl_Position = osg_ModelViewProjectionMatrix * osg_Vertex; \n"
        "    vec4 worldPos = osg_ViewMatrixInverse * osg_ModelViewMatrix * osg_Vertex;\n"
        "    gl_ClipDistance[0] = dot(clip0,worldPos);\n"
        "} \n";
    osg::Shader* vShader = new osg::Shader(osg::Shader::VERTEX, vertexSource);

    const std::string fragmentSource =
        "#version 430 compatibility \n"
        " \n"
        "in vec4 color; \n"
        "out vec4 fragData; \n"
        " \n"
        "void main() \n"
        "{ \n"
        "    fragData = color; \n"
        "} \n";
    osg::Shader* fShader = new osg::Shader(osg::Shader::FRAGMENT, fragmentSource);

    osg::Program* program = new osg::Program;
    program->addShader(vShader);
    program->addShader(fShader);
    stateSet->setAttribute(program);

    osg::Vec3f lightDir(0., 0.5, 1.);
    lightDir.normalize();
    stateSet->addUniform(new osg::Uniform("ecLightDir", lightDir));
}

void createRttTextures(int w, int h,std::map<int, osg::ref_ptr<osg::Texture2D>>& rttTextures)
{
    osg::Texture2D* pText = new osg::Texture2D;
    pText->setTextureSize(w, h);
    pText->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
    pText->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
    rttTextures[osg::Camera::COLOR_BUFFER] = pText;
    pText->setName("color_buffer_rtt");
    pText->setInternalFormat(GL_RGBA);
   /* pText->setSourceFormat(GL_RGBA);
    pText->setSourceType(GL_FLOAT);*/

    pText = new osg::Texture2D;
    pText->setTextureSize(w, h);
    pText->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
    pText->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
    rttTextures[osg::Camera::DEPTH_BUFFER] = pText;
    pText->setInternalFormat(GL_DEPTH_COMPONENT);
   // pText->setSourceFormat(GL_FLOAT);
  //  pText->setSourceType(GL_DEPTH_COMPONENT32);
    pText->setName("depth_buffer_rtt");

}


std::vector<osg::Vec4f> g_worldPosition;
osg::Camera* createHUD(osg::Texture2D* texture2d,int w,int h)
{
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setViewMatrix(osg::Matrix::identity());
    camera->setAllowEventFocus(false);
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    camera->setViewport(0, 0, w, h);
    camera->setProjectionMatrixAsOrtho2D(0, w, 0, h);
    camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    camera->setRenderOrder(osg::Camera::POST_RENDER);

    osg::Node* pNode = osg::createTexturedQuadGeometry(osg::Vec3(0, 0, 0), osg::Vec3(w, 0, 0), osg::Vec3(0, h , 0));
    camera->addChild(pNode);
    camera->setAllowEventFocus(false);
    osg::StateSet* pSS = pNode->getOrCreateStateSet();
    pSS->setTextureAttributeAndModes(0, texture2d);
    configureHudShaders(pSS);
    return camera.release();
}

osg::ref_ptr<osg::Node> createScene(osg::ArgumentParser& arguments)
{
    // create scene
    osg::ref_ptr<osg::Node> pNode = osgDB::readRefNodeFiles(arguments);
    if (pNode == NULL)
    {
        osg::notify(osg::FATAL) << "Unable to load model from command line." << std::endl;
        pNode = osgDB::readNodeFile("cow.osg");
    }
    osg::StateSet* pSs = pNode->getOrCreateStateSet();
    configureShaders(pSs);
    return pNode;
}

#include <osg/Math>
class UpdateMVPCallback: public osg::NodeCallback
    {
          osg::ref_ptr<osg::StateSet> _rootStateSet;
public:
    UpdateMVPCallback(osg::StateSet* pRootStateSet) :_rootStateSet(pRootStateSet)
    {

    }

    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        traverse(node, nv);

        if (osgUtil::CullVisitor* cv = nv->asCullVisitor())
        {
            osg::Matrixd P = *(cv->getProjectionMatrix());
            double znear = cv->getCalculatedNearPlane();
            double zfar = cv->getCalculatedFarPlane();
            if(znear < FLT_MAX && zfar > 0)
                cv->clampProjectionMatrix(P, znear, zfar);

            osg::Matrix MV = *(cv->getModelViewMatrix());
            osg::Matrix mvp = MV * P;
            osg::Matrix mvpInverse = osg::Matrix::inverse(mvp);

            if (_rootStateSet)
            {
                _rootStateSet->getOrCreateUniform("scene_mvp", osg::Uniform::FLOAT_MAT4)->set(mvp);
                _rootStateSet->getOrCreateUniform("scene_mvpInverse", osg::Uniform::FLOAT_MAT4)->set(mvpInverse);
            }
        }
        
    }
};
class ComputeWorldPosDrawCallback : public osg::Drawable::DrawCallback
{
public:
    virtual void drawImplementation(osg::RenderInfo& renderInfo, const osg::Drawable* drawable) const 
    {
        drawable->drawImplementation(renderInfo);
        const osg::StateSet* pStateSet = drawable->getStateSet();
        if (!pStateSet)
            return;

        const osg::Texture* pTex = nullptr;
        const osg::StateAttribute* pObj = drawable->getStateSet()->getAttribute(osg::StateAttribute::BINDIMAGETEXTURE, 1);
        if (pObj)
        {
            const osg::BindImageTexture* pBit = dynamic_cast<const osg::BindImageTexture*>(pObj);
            if (!pBit)
                return;

            pTex = pBit->getTexture();
        }
        else
        {
            const osg::StateSet::AttributeList& list = pStateSet->getAttributeList();
            for (auto itr = list.begin(); itr != list.end(); ++itr)
            {
                auto  attribPair = itr->first;
                if (attribPair.first == osg::StateAttribute::BINDIMAGETEXTURE)
                {
                    const osg::BindImageTexture* pBit = dynamic_cast<const osg::BindImageTexture*>((itr->second).first.get());
                    if (pBit)
                    {
                        pTex = pBit->getTexture();
                    }
                }
            }
        }
   
        GLenum id = pTex->getTextureObject(renderInfo.getContextID())->id();
        int nLen = pTex->getTextureWidth() * pTex->getTextureHeight() * 4 * sizeof(float);
        unsigned char* pBuf = new unsigned char[nLen];
        glBindTexture(GL_TEXTURE_2D, id);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pBuf);
        GLenum code = glGetError();
        glBindTexture(GL_TEXTURE_2D, 0);
        osg::Vec4f* pPoints = (osg::Vec4f*)(pBuf);
        g_worldPosition[0] = pPoints[1];
        printf("%f,%f,%f,%f\n", 
            g_worldPosition[0].x(), g_worldPosition[0].y(), g_worldPosition[0].z(), g_worldPosition[0].w());
        delete [] pBuf;

    }
};

osg::Node* createComputeNode(osg::Texture2D* inTex,osg::Texture2D* outTex)
{
    osg::DispatchCompute* pComputeNode = new osg::DispatchCompute(1, 1, 1);// 定义一个组
    pComputeNode->setDataVariance(osg::Object::DYNAMIC);
    osg::StateSet* pSS = pComputeNode->getOrCreateStateSet();
    pComputeNode->setDrawCallback(new ComputeWorldPosDrawCallback);
    pSS->setTextureAttributeAndModes(0, inTex);
    /*
       BindImageTexture(
                        GLuint imageunit = 0,
                        osg::Texture* target = 0,
                        Access access = READ_ONLY,
                        GLenum format = GL_RGBA8,
                        int level = 0,
                        bool layered = GL_FALSE,
                        int layer = 0) : osg::StateAttribute(),
            _target(target),
    */
    osg::BindImageTexture* bit = new osg::BindImageTexture(
                1,outTex,osg::BindImageTexture::WRITE_ONLY,GL_RGBA32F);
    pSS->setAttributeAndModes(bit);
    osg::ref_ptr<osg::Program> computeProg = new osg::Program;
    std::string shaderFile = ShaderPath + std::string("/computeShader.glsl");
    osg::ref_ptr<osg::Shader> shader = osgDB::readRefShaderFile(shaderFile);
    computeProg->addShader(new osg::Shader(osg::Shader::COMPUTE, shader->getShaderSource()));
    pSS->setAttributeAndModes(computeProg.get());
    pSS->addUniform(new osg::Uniform("depthTex", (int)0));
   
    return pComputeNode;
}



int main(int argc, char** argv)
{
    g_worldPosition.assign(4, osg::Vec4f(0, 0, 0, 1));
    osg::ArgumentParser arguments(&argc, argv);
    const int w = 800;
    const int h = 600;
    osgViewer::Viewer viewer;
    std::map<int, osg::ref_ptr<osg::Texture2D>> rttTextures;
    osg::ref_ptr<osg::Group> root = new osg::Group;
    osg::ref_ptr<osg::Camera> pRttCamera; 
    osg::ref_ptr<osg::Node> pNode = createScene(arguments);
   
    configGL3Camera(&viewer,w,h);
    createRttTextures(w, h, rttTextures);
    pRttCamera = createRttCamera(w, h, rttTextures);
    root->addChild(pRttCamera);
    pRttCamera->addChild(pNode);
    UpdateMVPCallback* nc = new UpdateMVPCallback(root->getOrCreateStateSet());
    pRttCamera->setCullCallback(nc);

    osg::setNotifyLevel(osg::WARN);
    osg::setNotifyHandler(new osg::WinDebugNotifyHandler);
     
    osg::Texture2D* pTexOut = new osg::Texture2D;
    pTexOut->setTextureSize(2, 2);
    pTexOut->setInternalFormat(GL_RGBA32F);
    pTexOut->setSourceFormat(GL_RGBA);
    pTexOut->setSourceType(GL_FLOAT);
    if (osg::Node* pComputeNode = createComputeNode(rttTextures[osg::Camera::DEPTH_BUFFER].get(), pTexOut))
    {
        root->addChild(pComputeNode);
    }
    osg::Texture2D* pHudTex = nullptr;
    pHudTex = rttTextures[osg::Camera::DEPTH_BUFFER].get();
    //pHudTex = pTexOut;
    root->addChild(createHUD(pHudTex, w, h));
    osg::Vec4 screenSize(w, h, 1.0 / w, 1.0 / h);
    root->getOrCreateStateSet()->addUniform(new osg::Uniform("screenSize", screenSize));
    osgUtil::Optimizer optimizer;
    optimizer.optimize(root.get(), osgUtil::Optimizer::ALL_OPTIMIZATIONS | osgUtil::Optimizer::TESSELLATE_GEOMETRY);

    viewer.setSceneData(root);
    return(viewer.run());
}
