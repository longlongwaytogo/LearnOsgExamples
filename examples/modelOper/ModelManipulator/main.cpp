
#pragma comment(lib,"osgsimd.lib")

#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osgSim/Impostor>
#include <osgDB/ReadFile>
#include <osgGA/GUIEventHandler>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/RayIntersector>
#include <osg/MatrixTransform>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osgManipulator/Dragger>
#include <osgManipulator/Selection>
#include <osgManipulator/TranslateAxisDragger>


osg::Vec3d screenToWorld(osg::Camera* pCamera, osg::Vec3d screen)
{
    osg::Matrixd mvpw = pCamera->getViewMatrix() * pCamera->getProjectionMatrix()
        * pCamera->getViewport()->computeWindowMatrix();
    osg::Matrixd invMVPW = osg::Matrix::inverse(mvpw);

    osg::Vec3d pos = screen * invMVPW;
    return pos;

}

class ModelShape : public osg::Group
{
public:
    ModelShape(osg::Node* shape):m_shape(shape),
        m_dragger(new osgManipulator::TranslateAxisDragger()),
        m_selection(new osgManipulator::Selection())
    {
        setName("modelShape");

        float scale = shape->getBound().radius() * 1.6;
        m_dragger->setMatrix(osg::Matrix::scale(scale, scale, scale) * osg::Matrix::translate(
            shape->getBound().center()));

        m_dragger->setupDefaultGeometry();

        m_selection->addChild(shape);
        addChild(m_selection);
    }
    ~ModelShape()
    {

    }

    void EnableDragger()
    {
        addChild(m_dragger);
        m_dragger->addTransformUpdating(m_selection);
        m_dragger->setHandleEvents(true);
    }

    void DisableDragger()
    {
        removeChild(m_dragger);
        m_dragger->removeTransformUpdating(m_selection);
        m_dragger->setHandleEvents(false);
    }
private:
    osg::ref_ptr<osg::Node> m_shape;
    osg::ref_ptr<osgManipulator::Dragger> m_dragger;
    osg::ref_ptr<osgManipulator::Selection> m_selection;
};
class ModelHandler : public osgGA::GUIEventHandler
{
    float m_x;
    float m_y;
    bool m_enableDragger;
public:
    ModelHandler(): m_x(0.0f),m_y(0.0f),m_enableDragger(true)
    {
       
    }

    void pick(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
        osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
        osgUtil::LineSegmentIntersector::Intersections hits;

        if (view->computeIntersections(ea.getX(), ea.getY(), hits))
        {
            osgUtil::LineSegmentIntersector::Intersection intersection = *hits.begin();
            osg::NodePath& nodePath = intersection.nodePath;
            int nodeSize = nodePath.size();
            if (nodeSize > 0)
            {
                
                for (int i = nodeSize - 1; i >= 0; i--)
                {
                    ModelShape* shape = dynamic_cast<ModelShape*>(nodePath[i]);
                    if (shape)
                    {
                        m_enableDragger ? shape->EnableDragger() : shape->DisableDragger();
                    }
               }
              
            }
        }
    }

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
    { 
       
        osg::Camera* pCamera = aa.asView()->getCamera();
        if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH  )
        {
            // pick
           m_x = ea.getX();
           m_y = ea.getY();
           
           
         

        }
        else if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE)
        {
            if (ea.getX() == m_x && ea.getY() == m_y)
            {
                pick(ea, aa);
            }
        }
        else if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
        {
            if (ea.getKey() == 'd')
            {
                m_enableDragger = !m_enableDragger;
            }
        }
        return false;
    }

};

int main()
{
    osg::Group* root = new osg::Group;
    //…Í«Îviewer
    osgViewer::Viewer viewer ;
    //∂¡»°ƒ£–Õ
    osg::Node* node = osgDB::readNodeFile("cow.osg");
    float ratio = 10;
    for (int i = 0; i < 10;i++)
    {
        for (int j = 0; j < 10; j++)
        {
            osg::MatrixTransform* transform = new osg::MatrixTransform;
            transform->setName("cow");
            transform->setMatrix(osg::Matrix::translate(osg::Vec3d(i * ratio, j * ratio, 0)));
            transform->addChild(new ModelShape(node));
            root->addChild(transform);
        }
    }
   
 
   
  
    viewer.setSceneData( root );
    viewer.setUpViewInWindow(0, 0, 1024, 768);
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.addEventHandler(new osgViewer::WindowSizeHandler);
    viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));
    viewer.addEventHandler(new ModelHandler);
    viewer.realize();
    viewer.run();
    return 0;
}
