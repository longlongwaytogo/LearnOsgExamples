
#pragma comment(lib,"osgsimd.lib")

#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osgSim/Impostor>
#include <osgDB/ReadFile>
#include "osgOrthoManipulator.h"
int main()
{
    //����viewer
    osgViewer::Viewer viewer ;
    //��ȡģ��
    osg::Node * node = osgDB::readNodeFile ("cow.osg") ;
    //����һ��impostor���
   // osgSim::Impostor * sim = new osgSim::Impostor ;
    //��0��50000֮����ʾģ�ͣ�֮����ʾ��ͼ
   // sim->addChild (node,0,50) ;
   // sim ->setImpostorThreshold(10) ;
    osg::Group* root = new osg::Group;
    root ->addChild (node) ;
    viewer.setSceneData( root );

    viewer.setCameraManipulator(new osgGA::OrthoManipulator(&viewer,root));
    viewer.realize();
    viewer.run();
    return 0;
}

//reference
// https://blog.csdn.net/qingcaichongchong/article/details/79258084
// https://blog.csdn.net/cloudqiu/article/details/99287378?utm_medium=distribute.pc_relevant.none-task-blog-baidulandingword-2&spm=1001.2101.3001.4242