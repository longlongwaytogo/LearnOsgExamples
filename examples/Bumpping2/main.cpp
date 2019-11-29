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
#include <osgFx/BumpMapping>

 


 
int main()
{
	osgViewer::Viewer viewer;
    osg::Group* pRoot = new osg::Group;
   
	osg::Node* cessna = osgDB::readNodeFile("cessna.osg");
    osg::ref_ptr<osgFX::BumpMapping> bm = new  osgFX::BumpMapping;
    bm->addChild(cessna);

  
    // 设置默认的漫反射和法线贴图环境，并设置相关的纹理坐标
    //bm->setLightNumber(6);
    bm->setDiffuseTextureUnit(0);
    bm->setNormalMapTextureUnit(1);

#if 0
    bm->setUpDemo();
   
#else
   
     osg::ref_ptr<osg::Texture2D> colorTex = new osg::Texture2D;
    
     
     colorTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
     colorTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
     colorTex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
     colorTex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
     colorTex->setMaxAnisotropy(8);
    //colorTex->setImage(osgDB::readImageFile("Images/whitemetal_diffuse.jpg"));
    colorTex->setImage(osgDB::readImageFile(  "E:\\project\\osg\\OSG.ms\\OsgEditor\\Data\\materials\\pics\\qiang\\qiang.png"));
    
   
    osg::ref_ptr<osg::Texture2D> normalTex = new osg::Texture2D;
    normalTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    normalTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    normalTex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    normalTex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    normalTex->setMaxAnisotropy(8);
    //normalTex->setImage(osgDB::readImageFile("Images/whitemetal_normal.jpg"));
    normalTex->setImage(osgDB::readImageFile("E:\\project\\osg\\OSG.ms\\OsgEditor\\Data\\materials\\pics\\qiang\\qiang_NORM.png"));
    bm->setOverrideDiffuseTexture(colorTex);
    bm->setOverrideNormalMapTexture(normalTex);
    bm->setUpDemo();
    // bm->prepareChildren();
#endif 
 
      

  

    


    pRoot->addChild(bm);
    viewer.setSceneData(pRoot);
    viewer.setUpViewInWindow(0,0,800,600);
	viewer.realize();
	viewer.run();
	return 0;
}