
#pragma comment(lib,"osgsimd.lib")

#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osgSim/Impostor>
#include <osgDB/ReadFile>
int main()
{
    //����viewer
    osgViewer::Viewer viewer ;
    //��ȡģ��
    osg::Node * node = osgDB::readNodeFile ("cow.osg") ;
    //����һ��impostor���
    osgSim::Impostor * sim = new osgSim::Impostor ;
    //��0��50000֮����ʾģ�ͣ�֮����ʾ��ͼ
    sim->addChild (node,0,50) ;
    sim ->setImpostorThreshold(10) ;
    osg::Group* root = new osg::Group;
    root ->addChild (sim) ;
    viewer.setSceneData( root );
    viewer.realize();
    viewer.run();
    return 0;
}