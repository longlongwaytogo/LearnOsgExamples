#include <osgViewer/Viewer>   
#include <osgViewer/ViewerEventHandlers>   
#include <osgGA/StateSetManipulator>   
#include <osgDB/ReadFile>   
#include <osgUtil/LineSegmentIntersector>   
#include <osgUtil/IntersectionVisitor>   
#include <osg/MatrixTransform>   
#include <osg/Group>   
#include <osg/Node>   

// code from :http://blog.163.com/wh_pxg/blog/static/13538285520157522039215/

int main()   
{   
    osg::ref_ptr<osg::Node> cow = osgDB::readNodeFile("cow.osg");//第一个节点   
    osg::ref_ptr<osg::Node> axes = osgDB::readNodeFile("axes.osgt");//第二个节点   
    osg::ref_ptr<osg::MatrixTransform> mt_cow = new osg::MatrixTransform;   
    mt_cow->setMatrix(osg::Matrix::scale(osg::Vec3(0.5,0.5,0.5))*osg::Matrix::translate(osg::Vec3(-10,0,0)));   
    mt_cow->addChild(cow.get());   
    osg::ref_ptr<osg::MatrixTransform> mt_axes = new osg::MatrixTransform;   
    mt_axes->addChild(axes.get());   
    osg::ref_ptr<osg::Group> root = new osg::Group;   
    root->addChild(mt_cow.get());   
    root->addChild(mt_axes.get());   
    osgViewer::Viewer * viewer = new osgViewer::Viewer;   
    viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));   
    /************************************************************************/  
    /* 碰撞检测                                                             */  
    /************************************************************************/  
    osg::Vec3 p_start(-100,0,0);   
    osg::Vec3 p_end(100,0,0);   
    osgUtil::LineSegmentIntersector::Intersections _intersections;   
    osg::ref_ptr< osgUtil::LineSegmentIntersector > _lineSegmentIntersector = new osgUtil::LineSegmentIntersector(p_start,p_end);   
    osgUtil::IntersectionVisitor _iv(_lineSegmentIntersector.get());   
    cow->accept(_iv);   
    _intersections=_lineSegmentIntersector->getIntersections();   
    int _intersectionNumber=_intersections.size();           
    if (_intersectionNumber!=0)   
    {            
        osgUtil::LineSegmentIntersector::Intersections::iterator hitr = _intersections.begin();   
        osg::Vec3 vec3_p1 =hitr->getWorldIntersectPoint()*mt_cow->getMatrix();//这样得到的交点就是绝对的世界坐标了。   
        mt_axes->setMatrix(osg::Matrix::translate(vec3_p1));//把坐标系原点移动到交点，可以测试现在的坐标系原点是不是刚好和牛相交！   
        hitr++;   
        osg::Vec3 vec3_p2 =hitr->getWorldIntersectPoint()*mt_cow->getMatrix();   
    }   
    viewer->setSceneData(root.get());   
    return viewer->run();   
       
}  