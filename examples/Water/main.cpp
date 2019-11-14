#include <iostream>
#include <osgViewer/viewer>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osg/Shape>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Geode>

// link: http://glslsandbox.com/e#57540.0
int main()
{
	osgViewer::Viewer viewer;
    osg::Group* pRoot = new osg::Group;
    osg::Geometry* plane = osg::createTexturedQuadGeometry(osg::Vec3(0,0,0),osg::Vec3(1 ,0,0),osg::Vec3(0,0,1 ));
      osg::ref_ptr<osg::Geode> quad = new osg::Geode;
    quad->addDrawable( plane );

   // pRoot->addChild(osgDB::readNodeFile("cow.osg"));

	if(pRoot)
	{
        pRoot->addChild(quad);
		viewer.setSceneData(pRoot);
	}

    {
        osg::Shader* fsShader = osgDB::readShaderFile(osg::Shader::FRAGMENT,"I:\\co\\LearnOsgExamples\\examples\\Water\\shader.glsl");

     
        
    const std::string vertexSource =
        "void main()\n"
        "{\n"
        "    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;\n"
           " gl_TexCoord[0] = gl_MultiTexCoord0; \n"
       
        "} \n";
    osg::Shader* vShader = new osg::Shader(osg::Shader::VERTEX, vertexSource);

    osg::Program* program = new osg::Program;

    program->addShader(vShader);
    
    program->addShader(fsShader);

    pRoot->getOrCreateStateSet()->setAttribute(program);
    }
    viewer.setUpViewInWindow(0,0,800,600);
	viewer.realize();
	viewer.run();
	return 0;
}