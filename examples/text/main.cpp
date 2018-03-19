#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Material>


int main(int argc, char** argv)
{
	osgViewer::Viewer viewer;

	osg::Group* root = new osg::Group;

	osg::Node* cow = osgDB::readNodeFile("cow.osg");

	root->addChild(cow);

	viewer.setSceneData(root);

	// Set the material
	osg::ref_ptr<osg::Material> material = new osg::Material;
	osg::Vec4 redColor(1.0, 0.0, 0.0, 1.0);
	material->setAmbient(osg::Material::FRONT_AND_BACK, redColor);
	material->setDiffuse(osg::Material::FRONT_AND_BACK,redColor);
	material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
	cow->getOrCreateStateSet()->setAttributeAndModes(material.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	return viewer.run();
}

//#include <osgDB/ReadFile>
//#include <osgViewer/Viewer>
//#include <osg/Texture2D>
//#include <osg/TexEnv>
//#include <osg/TexGen>
//osg::Image* createImage( int width, int height,osg::Vec3 color )
//{
//	osg::ref_ptr<osg::Image> image = new osg::Image;
//	image->allocateImage( width, height, 1, GL_RGB, GL_UNSIGNED_BYTE );
//	unsigned char* data = image->data();
//	for ( int y=0; y<height; ++y )
//	{
//		for ( int x=0; x<width; ++x )
//		{
//			*(data++) = color.x();
//			*(data++) = color.y();
//			*(data++) = color.z();
//		}
//	}
//	return image.release();
//}
//int main(int argc, char *argv[])
//{
//	
//	osgViewer::Viewer viewer;
//	osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("cow.osg");
//	if(model.valid())
//	{
//		osg::ref_ptr<osg::Image> image= createImage(256,256,osg::Vec3(200,255.0,255.0));
//		if (image.get())
//		{
//			osg::ref_ptr<osg::Texture2D> texture=new osg::Texture2D();
//			texture->setImage(image.get());
//			//设置自动生成纹理坐标
//			osg::ref_ptr<osg::TexGen> texgen=new osg::TexGen();
//			texgen->setMode(osg::TexGen::NORMAL_MAP);
//			//设置纹理环境，模式为BLEND
//			osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
//			texenv->setMode(osg::TexEnv::Mode::BLEND);
//			texenv->setColor(osg::Vec4(0.9,0.9,0.8,0.0));
//			//启动单元一自动生成纹理坐标，并使用纹理
//			osg::ref_ptr<osg::StateSet> state=new osg::StateSet;
//			state->setTextureAttributeAndModes(1,texture.get(),osg::StateAttribute::ON);
//			state->setTextureAttributeAndModes(1,texgen.get(),osg::StateAttribute::ON);
//			// state->setTextureAttribute(1,texenv.get());
//			model->setStateSet(state.get());
//		}
//		viewer.setSceneData(model.get());
//	}
//	viewer.run();
//	
//}