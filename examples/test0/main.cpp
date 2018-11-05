#include <iostream>
#include <osgViewer/viewer>
#include <osg/Node>
#include <osgDB/ReadFile>



int main()
{
	osgViewer::Viewer viewer;
	osg::Node* pNode = osgDB::readNodeFile("cow.osg");
	if(pNode)
	{

		viewer.setSceneData(pNode);
	}
	viewer.realize();
	viewer.run();
	return 0;
}