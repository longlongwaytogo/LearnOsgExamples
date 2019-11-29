#include <iostream>
#include <osgViewer/viewer>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osg/Shape>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osgUtil/TangentSpaceGenerator>

static const char* vertSrc = {
    "attribute vec3 tangent;\n"
    "attribute vec3 binormal;\n"
    "varying vec3 lightDir;\n"
    "void main()\n"
    "{\n"
    "vec3 normal = normalize(gl_NormalMatrix * gl_Normal);\n"
    "mat3 TBN = mat3(tangent, binormal, normal);\n"
    "vec4 vertexInEye = gl_ModelViewMatrix * gl_Vertex;\n"
    "lightDir = vec3(gl_LightSource[0].position.xyz - vertexInEye.xyz);\n"
    "lightDir = normalize(TBN * normalize(lightDir));\n"
    "gl_Position = ftransform();\n"
    "gl_TexCoord[0] = gl_MultiTexCoord0;\n"

    "}\n"
};

static const char* fragSrc = {
    "uniform sampler2D colorTex; \n"
    "uniform sampler2D normalTex; \n"
    "varying vec3 lightDir;\n"
    "void main(void)\n"
    "{\n"
    "      vec4 base = texture2D(colorTex,gl_TexCoord[0].xy);\n"
    "      vec4 bump = texture2D(normalTex,gl_TexCoord[0].xy);\n"
    "       bump = normalize(bump*2.0-1.0);\n"

    "   float lambert = max(dot(bump,lightDir),0.0);\n"
    "   if( lambert > 0.0)\n"
    "   {\n"
    "       gl_FragColor = base * gl_LightSource[0].diffuse * lambert;\n"
    "       gl_FragColor += gl_LightSource[0].specular * pow(lambert,2.0);\n"
    "   }\n"
    "   gl_FragColor += gl_LightSource[0].ambient;\n"
    "}\n"
};

class ComputeTangentVisitor :public osg::NodeVisitor
{

public:
    void apply(osg::Node& node) { traverse(node);}

    void apply(osg::Geode& node)
    {

        for(unsigned int i = 0; i < node.getNumDrawables(); ++i)
        {

            osg::Geometry* geom = dynamic_cast<osg::Geometry*>(node.getDrawable(i));
            if(geom) generateTangentArray(geom);
        }
    }

    void generateTangentArray(osg::Geometry* geom)
    {
        osg::ref_ptr<osgUtil::TangentSpaceGenerator> tsg = new osgUtil::TangentSpaceGenerator;
        tsg->generate(geom);

        geom->setVertexAttribArray(6,tsg->getTangentArray());
        geom->setVertexAttribBinding(6,osg::Geometry::BIND_PER_VERTEX);

        geom->setVertexAttribArray( 7, tsg->getBinormalArray() );
        geom->setVertexAttribBinding( 7, osg::Geometry::BIND_PER_VERTEX );

    }
     
};


 
int main()
{
	osgViewer::Viewer viewer;
    osg::Group* pRoot = new osg::Group;
    osg::Node* skydome = osgDB::readNodeFile("skydome.osgt");
    ComputeTangentVisitor ctv;
    ctv.setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
     skydome->accept(ctv);

     osg::ref_ptr<osg::Program> program = new osg::Program;
     program->addShader(new osg::Shader(osg::Shader::VERTEX, vertSrc));
     program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragSrc));
     program->addBindAttribLocation("tangent",6);
     program->addBindAttribLocation("binormal",7);

     osg::ref_ptr<osg::Texture2D> colorTex = new osg::Texture2D;
     colorTex->setImage(osgDB::readImageFile("Images/whitemetal_diffuse.jpg"));

     osg::ref_ptr<osg::Texture2D> normalTex = new osg::Texture2D;
     normalTex->setImage(osgDB::readImageFile("Images/whitemetal_normal.jpg"));

     osg::StateSet* ss = skydome->getOrCreateStateSet();
     ss->addUniform(new osg::Uniform("colorTex",0));
     ss->addUniform(new osg::Uniform("normalTex",1));
     ss->setAttributeAndModes(program.get());

     osg::StateAttribute::GLModeValue val = osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE;
     ss->setTextureAttributeAndModes(0,colorTex,val);
     ss->setTextureAttributeAndModes(1,normalTex,val);


    
    
     pRoot->addChild(skydome);
	 
	viewer.setSceneData(pRoot);
	 
   
    viewer.setUpViewInWindow(0,0,800,600);
	viewer.realize();
	viewer.run();
	return 0;
}