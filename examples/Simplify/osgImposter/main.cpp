
#pragma comment(lib,"osgsimd.lib")

#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osgSim/Impostor>
#include <osgDB/ReadFile>
int main()
{
    //申请viewer
    osgViewer::Viewer viewer ;
    //读取模型
    osg::Node * node = osgDB::readNodeFile ("cow.osg") ;
    //申请一个impostor结点
    osgSim::Impostor * sim = new osgSim::Impostor ;
    //在0到50000之内显示模型，之外显示贴图
    sim->addChild (node,0,50) ;
    sim ->setImpostorThreshold(10) ;
    osg::Group* root = new osg::Group;
    root ->addChild (sim) ;
    viewer.setSceneData( root );
    viewer.realize();
    viewer.run();
    return 0;
}