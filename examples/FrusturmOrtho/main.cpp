
//
//#include <osg/shader>
//#include <osg/Node>
//#include <osgDB/ReadFile>
//#include <osgDB/fileutils>
//#include <osg/Geometry>
//#include <osgViewer/viewer>
//
//static char * vertexShader= {
//        "#version 430 \n"
//        "layout (location=0) in vec3 VertexPosition;\n"
//        "layout (location=1) in vec4 VertexColor;\n"
//        "uniform mat4 MVP;"
//        "out vec4 Color;\n"
//        "void main()\n"
//        "{\n"
//            "   Color = VertexColor;\n"
//            "   gl_Position = MVP * vec4(VertexPosition,1.0);\n"
//    "   }\n"
//};
//
//static char * fragShader ={
//        "#version 430 \n"
//        "in vec4 Color;\n"
//        "layout (location=0) out vec4 FragColor;\n"
//        "void main() {\n"
//        "   FragColor = Color;//vec4(0.5,0.5,0.5,0.4);\n"
//        "}\n"
//};
//osg::Node *  CreateNode()
//{
//        osg::Geode * geode = new osg::Geode;
//         osg::Geometry* polyGeom = new osg::Geometry();
//        osg::Vec3Array* vertices = new osg::Vec3Array();
//        vertices->push_back(osg::Vec3(-5,0,0));
//        vertices->push_back(osg::Vec3(5,0,0));
//        vertices->push_back(osg::Vec3(0,0,5));
//        //polyGeom->setVertexArray(vertices);
//
//
//        osg::ref_ptr<osg::Vec4Array> colorsArray = new osg::Vec4Array;
//        colorsArray->push_back(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
//        colorsArray->push_back(osg::Vec4(0.0f,0.0f,1.0f,1.0f));
//        colorsArray->push_back(osg::Vec4(0.0f,1.0f,0.0f,1.0f));
//    //  polyGeom->setColorArray(colorsArray.get());
//        //polyGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
//
//        polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,3));
//
//        /*
//        The osg::Geometry class uses the setVertexAttribArray() and
//        setVertexAttribBinding() methods to bind vertex attributes to shaders. They should
//        be provided per vertex. GLSL's built-in vertex attributes include the gl_Position, gl_
//        Normal, and gl_MultiTexCoord* variables. However, you may still specify your own
//        vertex attributes, such as tangents or vertex weights.
//        Try declaring an attribute in the vertex shader and make use of the osg::Geometry's vertex
//        attribute arrays. Another important task that you need to perform is to bind the external
//        attribute array and the GLSL attribute, with the help of the addBindAttribLocation()
//        method of osg::Program. It has a name and an index parameter, the first of which
//        indicates the attribute name in the shader source code, and the second should correspond
//        to the input index value of setVertexAttribArray().
//        */
//        polyGeom->setVertexAttribArray(0,vertices);
//        polyGeom->setVertexAttribBinding(0, osg::Geometry::BIND_PER_VERTEX);
//        polyGeom->setVertexAttribArray(1,colorsArray.get());    
//        polyGeom->setVertexAttribBinding(1, osg::Geometry::BIND_PER_VERTEX);
//
//        geode->addDrawable(polyGeom);
//        return geode; 
//}
//
//class MVPCallback: public osg::Uniform::Callback
//{
//public:
//        MVPCallback(osg::Camera * camera):mCamera(camera){
//        }
//         virtual void operator()( osg::Uniform* uniform, osg::NodeVisitor* nv){
//                 osg::Matrix modelView = mCamera->getViewMatrix();
//                 osg::Matrix projectM = mCamera->getProjectionMatrix();
//                 uniform->set(modelView * projectM);
//         }
//
//private:
//        osg::Camera * mCamera;
//};
// 
//#include <osgViewer/api/Win32/GraphicsWindowWin32>
//#include <osg/GLExtensions>
//
//#ifndef WGL_ARB_create_context
//#define WGL_CONTEXT_DEBUG_BIT_ARB      0x00000001
//#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
//#define WGL_CONTEXT_MAJOR_VERSION_ARB  0x2091
//#define WGL_CONTEXT_MINOR_VERSION_ARB  0x2092
//#define WGL_CONTEXT_LAYER_PLANE_ARB    0x2093
//#define WGL_CONTEXT_FLAGS_ARB          0x2094
//#define WGL_CONTEXT_PROFILE_MASK_ARB   0x9126
//#define ERROR_INVALID_VERSION_ARB      0x2095
//#endif
//
//void setOpenGL3Env(osgViewer::GraphicsWindowWin32* gw)
//{
//
//    typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);
//
//    if( gw )
//    {
//        OSG_NOTIFY( osg::INFO ) << "GL3: Attempting to create OpenGL3 context." << std::endl;
//        OSG_NOTIFY( osg::INFO ) << "GL3: version: " << gw->getTraits()->glContextVersion << std::endl;
//        OSG_NOTIFY( osg::INFO ) << "GL3: context flags: " << gw->getTraits()->glContextFlags << std::endl;
//        OSG_NOTIFY( osg::INFO ) << "GL3: profile: " << gw->getTraits()->glContextProfileMask << std::endl;
//
//    /*    osgViewer::Win32WindowingSystem::OpenGLContext openGLContext;
//        HDC hdc;
//        int _screenOriginX,_screenOriginY;
//        if( !Win32WindowingSystem::getInterface()->getSampleOpenGLContext( openGLContext, hdc, _screenOriginX, _screenOriginY ) )
//        {
//         
//        }
//        else*/
//        {
//            typedef void * (*WGLGETPROCADDRESS)(const char *);
//            typedef void (*GLGENBUFFERS)(GLsizei, GLsizei *);
//            HMODULE hDll = GetModuleHandle("OpenGL32.dll");
//
//            WGLGETPROCADDRESS wglGetProcAddress = (WGLGETPROCADDRESS)GetProcAddress(hDll, "wglGetProcAddress");
//           
//
//            PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
//                ( PFNWGLCREATECONTEXTATTRIBSARBPROC ) wglGetProcAddress( "wglCreateContextAttribsARB" );
//            if(!wglCreateContextAttribsARB)
//            {
//
//                wglCreateContextAttribsARB =(PFNWGLCREATECONTEXTATTRIBSARBPROC) osg::getGLExtensionFuncPtr("wglCreateContextAttribsARB");
//            }
//            if( wglCreateContextAttribsARB==0 )
//            {
//              /*  reportErrorForScreen( "GL3: wglCreateContextAttribsARB not available.",
//                    _traits->screenNum, ::GetLastError() );*/
//            }
//            else
//            {
//                unsigned int idx( 0 );
//                int attribs[ 16 ];
//
//                unsigned int major = 1, minor = 0;
//                if( ! gw->getTraits()->getContextVersion(major, minor) || major<3 )
//                {
//                    OSG_NOTIFY( osg::WARN ) << "GL3: Non-GL3 version number: " <<  gw->getTraits()->glContextVersion << std::endl;
//                }
//
//                attribs[ idx++ ] = WGL_CONTEXT_MAJOR_VERSION_ARB;
//                attribs[ idx++ ] = major;
//                attribs[ idx++ ] = WGL_CONTEXT_MINOR_VERSION_ARB;
//                attribs[ idx++ ] = minor;
//                if(  gw->getTraits()->glContextFlags != 0 )
//                {
//                    attribs[ idx++ ] = WGL_CONTEXT_FLAGS_ARB;
//                    attribs[ idx++ ] =  gw->getTraits()->glContextFlags;
//                }
//                if(  gw->getTraits()->glContextProfileMask != 0 )
//                {
//                    attribs[ idx++ ] = WGL_CONTEXT_PROFILE_MASK_ARB;
//                    attribs[ idx++ ] =  gw->getTraits()->glContextProfileMask;
//                }
//                attribs[ idx++ ] = 0;
//
//                HGLRC context = wglCreateContextAttribsARB( gw->getHDC(), 0, attribs );
//                if( context == NULL )
//                {
//                    /* reportErrorForScreen( "GL3: wglCreateContextAttribsARB returned NULL.",
//                    _traits->screenNum, ::GetLastError() );*/
//                }
//                else
//                {
//                    OSG_NOTIFY( osg::INFO ) << "GL3: context created successfully." << std::endl;
//                }
//
//                gw->setWGLContext(context);
//            }
//        }
//    }
//     
//}
//int main(int argc, char *argv[]) {   
//
//        osgViewer::Viewer viewer;
//
//        /*<版本指定>*/
//        const int width(800), height(450);
//        const std::string version("3.1");
//        osg::ref_ptr< osg::GraphicsContext::Traits > traits = new osg::GraphicsContext::Traits();
//        traits->x = 50; traits->y = 30;
//        traits->width = width; traits->height = height;
//        traits->windowDecoration = true;
//        traits->doubleBuffer = true;
//        traits->glContextVersion = version;
//       // osg::ref_ptr< osg::GraphicsContext > gc = osg::GraphicsContext::createGraphicsContext(traits.get());
//       /* if(gc)
//        {
//          
//            osg::ref_ptr<osgViewer::GraphicsWindowWin32> gw = osg::dynamic_pointer_cast<osgViewer::GraphicsWindowWin32>(gc);
//            if(gw)
//            {
//                setOpenGL3Env(gw.get());
//            }
//
//        }
//        if (!gc.valid())
//        {
//            osg::notify(osg::FATAL) << "Unable to create OpenGL v" << version << " context." << std::endl;
//            return -1;
//        }*/
//        // Create a Camera that uses the above OpenGL context.
//       // osg::Camera* cam = viewer.getCamera();
//        //cam->setGraphicsContext(gc.get());
//
//        osg::Group * root = new osg::Group;
//        osg::ref_ptr<osg::Node>node = CreateNode();
//
//        osg::StateSet * ss = node->getOrCreateStateSet();
//        osg::Program * program = new osg::Program;
//        program->addBindFragDataLocation("VertexPosition",0);
//        program->addBindFragDataLocation("VertexColor",1);
//
//        osg::Shader * vS = new osg::Shader(osg::Shader::FRAGMENT,fragShader); 
//        osg::Shader * fS = new osg::Shader(osg::Shader::VERTEX,vertexShader); 
//        osg::Uniform* MVPUniform = new osg::Uniform( "MVP",osg::Matrix());
//        MVPUniform->setUpdateCallback(new MVPCallback(viewer.getCamera()));
//        ss->addUniform(MVPUniform);//对应的Program和Uniform要加到同一个Node下的StateSet中
//        program->addShader(vS);
//        program->addShader(fS);
//        ss->setAttributeAndModes(program,osg::StateAttribute::ON);
//
//        root->addChild(node); 
//        viewer.setSceneData(root);
//
//        viewer.realize();
//          osg::ref_ptr< osg::GraphicsContext > gc = viewer.getCamera()->getGraphicsContext();
//        if(gc)
//        {
//          
//            osg::ref_ptr<osgViewer::GraphicsWindowWin32> gw = osg::dynamic_pointer_cast<osgViewer::GraphicsWindowWin32>(gc);
//            if(gw)
//            {
//                setOpenGL3Env(gw.get());
//            }
//
//        }
//
//
//        viewer.run();
//
//        return 0;
//}
//--------------------- 
//作者：封狼居胥_COU 
//来源：CSDN 
//原文：https://blog.csdn.net/qq_16123279/article/details/82463266 
//版权声明：本文为博主原创文章，转载请附上博文链接！

#include <iostream>
#include <osg/Node>
#include <osgViewer/Viewer>
#include <osg/Program>
#include <osgDB/ReadFile>
#include <osg/Shader>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/GUIEventHandler>
#include <osg/uniform>
#include <osg/StateSet>

osg::Matrix g_matrix;
bool g_ortho = false;

class RemoveMaterial : public osg::NodeVisitor
{
public:
    RemoveMaterial():osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    {}

    virtual void apply(osg::Node& node)
    {
        if (auto ss = node.getStateSet())
        {
            ss->removeAttribute(osg::StateAttribute::MATERIAL);
        }
        osg::NodeVisitor::apply(node);
    }
};
class myEventHandler :public osgGA::GUIEventHandler
{

public:
    virtual bool handle(osgGA::Event* event, osg::Object* object, osg::NodeVisitor* nv)
    {
        osgGA::GUIEventAdapter* pGE = event->asGUIEventAdapter();
        if (pGE)
        {
            if (pGE->getEventType() == osgGA::GUIEventAdapter::KEYUP)
            {
                if (pGE->getKey() == osgGA::GUIEventAdapter::KEY_F3)
                {

                    std::cout << " Perspective Projection:\n" << std::endl;
                    g_ortho = false;
                    return true;
                }
                else if (pGE->getKey() == osgGA::GUIEventAdapter::KEY_F4)
                {
                    std::cout << " Ortho Projection: \n" << std::endl;
                    g_ortho = true;
                    return true;
                }
            } 
        }
        return false;
    }
};

 
static char* vertexShader = {
    "uniform bool bOrtho;\n"
    "uniform mat4 Projection;\n"
    "varying vec4 color;\n"
    "varying vec2 TexCoord;\n"
    "void main(void ){\n"
    "color = gl_Vertex;\n"
    "  TexCoord = gl_MultiTexCoord0.xy;\n"
    "gl_Position =Projection * gl_ModelViewMatrix * gl_Vertex;\n"
    "}\n"
};
static char * fragShader = {
    "varying vec4 color;\n"
     "varying vec2 TexCoord;\n"

    "uniform sampler2D tex;\n"
    "void main(void){\n"
    "   gl_FragColor = texture2D(tex,TexCoord);\n"
    "}\n"
};

class CullCallback :public osg::Callback
{
    osgViewer::Viewer* _pViewer;
public:
    CullCallback(osgViewer::Viewer* pViewer):_pViewer(pViewer)
    {
    }
    virtual bool run(osg::Object* object, osg::Object* data)
    {
      
        if (osg::Node* pNode = object->asNode())
        {
            if (osg::StateSet* pSs = pNode->getOrCreateStateSet())
            {
                std::string u = "Projection";
                if (osg::Uniform* pUniform = pSs->getOrCreateUniform(u, osg::Uniform::FLOAT_MAT4))
                {
                    if (_pViewer)
                    {
                        if(osg::Camera* pCamera = _pViewer->getCamera())
                        {
                            osg::Matrixd projection;
                            osg::Matrixd ortho;
                            osg::Matrixd frustum = pCamera->getProjectionMatrix();
                            double l, r, t, b, n, f;
                           
                            frustum.getFrustum(l, r, b, t, n, f);
                           // n = 0.1;
                          //  f = 10000;
                            osg::Matrixd perstoOrtho = osg::Matrixd( 
                                n, 0, 0, 0,
                                0, n, 0, 0,
                                0,0, n +f, 1,
                                0, 0, -n*f, 0
                            );
                            osg::Matrixd M_ = osg::Matrixd(
                                -1, 0, 0, 0,
                                0, -1, 0, 0,
                                0, 0, 1, 0,
                                0, 0, 0, -1);

                      
                            if (!g_ortho)
                            {
                                projection = frustum;
                            }
                            else
                            {
                                ortho = osg::Matrix::inverse(perstoOrtho) *frustum * M_;

                                double a = 10;
                               ortho.makeOrtho(l * a, r * a, b * a, t * a, n, f);
                               // ortho.makeOrtho(-4, 4, -4, 4, -1000, 1000);
                                projection = ortho;
                              
                            }
                           
                          // g_matrix;
                           // ortho = ortho.ortho(-8, 8, -1, 1, -1000, 1000);
                            ;
                            pUniform->set(g_matrix);
                           
                        }
                       
                    }
                       
                  
                }
            }
        }
        return traverse(object, data);
    }
};
int main()
{
    osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
    osg::ref_ptr<osg::Node>node = osgDB::readNodeFile("D:\\data\\osgData\\Model\\root.osgb");
    //osg::ref_ptr<osg::Node>node = osgDB::readNodeFile("lz.osg");
    node->setCullCallback(new CullCallback(viewer));
    RemoveMaterial rm;;
   // viewer->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
    node->accept(rm);
   // viewer->getCamera()->setProjectionMatrixAsPerspective(45, 4.0 / 3.0, 0.1, 2000);

    osg::StateSet * ss = node->getOrCreateStateSet();

    osg::BoundingSphere bs =  node->getBound();
   

    osg::Program * program = new osg::Program;
    program->addShader(new osg::Shader(osg::Shader::FRAGMENT,fragShader));
    program->addShader(new osg::Shader(osg::Shader::VERTEX,vertexShader));
    ss->setAttributeAndModes(program,osg::StateAttribute::ON);

    viewer->addEventHandler(new osgViewer:: WindowSizeHandler);
    viewer->addEventHandler(new myEventHandler);
    viewer->setSceneData(node.get());
    viewer->setUpViewInWindow(100, 100, 800, 600);
    viewer->realize();
  // bs.set(osg::Vec3(0, 0, 0), 100);
  //  g_matrix = osg::Matrix::ortho(-5 * (bs.radius() + bs.radius() / 2), (bs.radius() + bs.radius() / 2) * 5, -bs.radius(), bs.radius(), -1000, 1000);
    g_matrix = osg::Matrix::ortho(-5 * (bs.radius() + bs.radius() / 2), (bs.radius() + bs.radius() / 2) * 5, -bs.radius(), bs.radius(), -1000, 1000);
    viewer->getCamera()->setProjectionMatrix(g_matrix);
    return viewer->run();
}
//--------------------- 
//    作者：封狼居胥_COU 
//    来源：CSDN 
//原文：https://blog.csdn.net/qq_16123279/article/details/82463266 
//版权声明：本文为博主原创文章，转载请附上博文链接！