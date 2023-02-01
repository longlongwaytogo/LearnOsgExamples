
#include <osg/Node>
#include <osgViewer/Viewer>
#include <osg/Program>
#include <osgDB/ReadFile>
#include <osg/Shader>
#include <osgViewer/ViewerEventHandlers>


static char * vertexShader = {
    "varying vec4 color;\n"
    "void main(void ){\n"
    "color = gl_Vertex;\n"
    "gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;\n"
    "}\n"
};
static char * fragShader = {
    "varying vec4 color;\n"
    "void main(void){\n"
    "   gl_FragColor = clamp(color,0.0,1.0);\n"
    "}\n"
};
int main()
{
    osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
    osg::ref_ptr<osg::Node>node = osgDB::readNodeFile("glider.osg");


    osg::StateSet * ss = node->getOrCreateStateSet();
    osg::Program * program = new osg::Program;
    program->addShader(new osg::Shader(osg::Shader::FRAGMENT,fragShader));
    program->addShader(new osg::Shader(osg::Shader::VERTEX,vertexShader));
    ss->setAttributeAndModes(program,osg::StateAttribute::ON);

    viewer->addEventHandler(new osgViewer:: WindowSizeHandler);
    viewer->setSceneData(node.get());
    return viewer->run();
}
//--------------------- 
//    作者：封狼居胥_COU 
//    来源：CSDN 
//原文：https://blog.csdn.net/qq_16123279/article/details/82463266 
//版权声明：本文为博主原创文章，转载请附上博文链接！