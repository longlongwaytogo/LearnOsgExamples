#include <osg/Group>
#include <osg/Plane>
#include <osgviewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>

std::string vertex = "void main()\n"
	"{\n"
	"gl_Position = ftransform();\n"
	"gl_TexCoord[0] = gl_MultiTexCoord0;\n"
	"}\n";
std::string stippleFragment =  "uniform int stipple[128];"
	"void main(){\n"
	"ivec2 coord = ivec2(gl_FragCoord.xy - 0.5);\n"
	"uint highBit = 1u << 7;\n"
	"if((stipple[(coord.x%32 + coord.y%32*32)/8 ]  & ( highBit>>(uint)(coord.x%8)))== 0)\n"
	" discard;\n"
	"gl_FragColor = vec4(1,0,0,1);\n"
	"}"	;
 
#include <osg/PolygonStipple>
GLubyte ps_mask[] = {
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00,
	0x03, 0x80, 0x01, 0xC0,
	0x06, 0xC0, 0x03, 0x60,
	0x04, 0x60, 0x06, 0x20,
	0x04, 0x30, 0x0C, 0x20,
	0x04, 0x18, 0x18, 0x20,
	0x04, 0x0C, 0x30, 0x20,
	0x04, 0x06, 0x60, 0x20,
	0x44, 0x03, 0xC0, 0x22,
	0x44, 0x01, 0x80, 0x22,
	0x44, 0x01, 0x80, 0x22,
	0x44, 0x01, 0x80, 0x22,
	0x44, 0x01, 0x80, 0x22,
	0x44, 0x01, 0x80, 0x22,
	0x44, 0x01, 0x80, 0x22,
	0x66, 0x01, 0x80, 0x66, 
	0x33, 0x01, 0x80, 0xCC,
	0x19, 0x81, 0x81, 0x98, 
	0x0C, 0xC1, 0x83, 0x30,
	0x07, 0xe1, 0x87, 0xe0,
	0x03, 0x3f, 0xfc, 0xc0,
	0x03, 0x31, 0x8c, 0xc0,
	0x03, 0x33, 0xcc, 0xc0,
	0x06, 0x64, 0x26, 0x60, 
	0x0c, 0xcc, 0x33, 0x30,
	0x18, 0xcc, 0x33, 0x18, 
	0x10, 0xc4, 0x23, 0x08,
	0x10, 0x63, 0xC6, 0x08, 
	0x10, 0x30, 0x0c, 0x08,
	0x10, 0x18, 0x18, 0x08, 
	0x10, 0x00, 0x00, 0x08};
 
#include <iostream>
#include <iomanip>

void main()
{
#if 1
	std::cout <<"0x:" << std::hex<<( 1u << 7) << std::endl;
	for(int y = 0; y< 64;++y)
	{
		for (int x = 0; x <64 ;x=x+1)
		{
			unsigned int highBit = 1u << 7;
			int index = ((x%32) + (y%32)*32)/8;
			//std::cout << index << " ";
			unsigned int val =  ps_mask[index];
		 
			//std::cout <<"0x"<<std::setfill('0') <<std::setw(2)<<std::hex << val << " ";
			//std::cout << 	x%8 << " ";
			//std::cout<< std::hex << (highBit >>(unsigned int)(x%8))<< " ";
			if(( val & ( highBit>>(unsigned int)(x%8)))== 0)
			{
			std::cout <<"a";
			}
			else
			std::cout <<" "; 
		}
		std::cout<<std::endl;
	}
	
	//return ;
#endif 

	osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("cow.osg");
#define USE_SHADER_STIPPLE 1
#if USE_SHADER_STIPPLE 
	osg::Program* pg = new osg::Program;
	pg->addShader(new osg::Shader(osg::Shader::VERTEX, vertex.c_str()));
	pg->addShader(new osg::Shader(osg::Shader::FRAGMENT, stippleFragment.c_str()));

	osg::StateSet* ss = model->getOrCreateStateSet();
	ss->setAttribute(pg);
	osg::Uniform* uf = ss->getOrCreateUniform("stipple",osg::Uniform::INT,128);
	osg::IntArray* stippleArray = new osg::IntArray(0);
	
	for(int i =0; i < 128; ++i)
		stippleArray->push_back(ps_mask[i]);
	uf->setArray(stippleArray);
#else
	osg::PolygonStipple* ps = new osg::PolygonStipple;
	ps->setMask(ps_mask);
	model->getOrCreateStateSet()->setAttributeAndModes(ps);

#endif 

	osg::ref_ptr<osg::Group> root = new osg::Group;

	root->addChild(model);
	{
		osg::MatrixTransform* mt = new osg::MatrixTransform;
		mt->setMatrix(osg::Matrix::translate(10,0,0));
		mt->addChild(osgDB::readNodeFile("cow.osg"));
		osg::PolygonStipple* ps = new osg::PolygonStipple;
		ps->setMask(ps_mask);
		mt->getOrCreateStateSet()->setAttributeAndModes(ps);
		root->addChild(mt);
	}

	osgViewer::Viewer viewer;
	viewer.setSceneData(root);
	viewer.run();
}
