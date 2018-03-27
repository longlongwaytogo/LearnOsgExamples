#include <iostream>
#include <osgViewer/viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Node>
#include <osg/Group>
#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osg/GraphicsContext>
#include <osgGA/TrackballManipulator>

class SysncOperation:public osg::GraphicsOperation
{

public:
    SysncOperation():osg::GraphicsOperation("SysncOperation",false)
    {

    }
    virtual void operator() (osg::GraphicsContext* context)
    {
        if(context)
        {
            context->makeCurrent();
            if(osgViewer::GraphicsWindow* window = dynamic_cast<osgViewer::GraphicsWindow*>(context))
            {
                window->setSyncToVBlank(false); // 关闭显卡垂直同步
            }
        }
    }
};

int main()
{
	osgViewer::Viewer viewer;
	osg::Node* pNode = osgDB::readNodeFile("cow.osg");

    osg::ref_ptr<osg::Group> root = new osg::Group;
    for(int i = 0; i< 2; ++i)
        for(int j = 0; j <1; ++j)
        {
            osg::ref_ptr<osg::MatrixTransform> transfrom = new osg::MatrixTransform();
            transfrom->addChild(pNode);
            transfrom->setMatrix(osg::Matrix::translate(osg::Vec3(10*i,10*j,0)));
            root->addChild(transfrom);

        }
	if(root)
	{
		viewer.setSceneData(root);
	}

    viewer.setRealizeOperation(new SysncOperation());
    viewer.addEventHandler(new osgViewer::StatsHandler());
    //viewer.setCameraManipulator(new osgGA::TrackballManipulator());
    // viewer.setRunMaxFrameRate(40);

    viewer.realize();
	viewer.run();
	return 0;
}