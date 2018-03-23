#include <iostream>
#include <osgViewer/viewer>
#include <osg/Node>
#include <osg/Image>
#include <osg/vec4>
#include <osg/Array>
#include <osg/Texture2D>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osgGA/EventHandler>
#include <osgGA/GUIEventHandler>

#include <osg/Matrix>
#include <osg/Camera>
#include <osg/Node>


int main()
{

	osgViewer::Viewer viewer;
	osg::Node* pNode  = osgDB::readNodeFile("cow.osg");
	viewer.setSceneData(pNode);

	osg::Camera* pCamera = viewer.getCamera();

	osg::Matrix transMat = osg::Matrix::translate(osg::Vec3(0,0,-18));
	double angle = -90;
	
	while(!viewer.done())
	{
		osg::Matrix ratate = osg::Matrix::rotate(osg::DegreesToRadians(angle),osg::Vec3(1,0,0));
		//pCamera->setViewMatrix( transMat * ratate);

		//pCamera->setViewMatrix(osg::Matrix::inverse( osg::Matrix::translate(osg::Vec3(0,0,18)) * osg::Matrix::rotate(osg::DegreesToRadians(angle),osg::Vec3(1,0,0) ) ) );
		pCamera->setViewMatrix(osg::Matrix::inverse( osg::Matrix::rotate(osg::DegreesToRadians(angle),osg::Vec3(1,0,0) ) *  osg::Matrix::translate(osg::Vec3(0,0,18))  ) );
		
		angle += 0.5;
		viewer.frame();
	}
}