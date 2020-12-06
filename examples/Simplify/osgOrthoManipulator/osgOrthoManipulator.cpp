#include "osgOrthoManipulator.h"

#include <osg\Math>
#include <osg/ComputeBoundsVisitor>


using namespace osgGA;

OrthoManipulator::OrthoManipulator(osgViewer::Viewer* viewer, osg::Node* root)
    : _viewer(viewer), _scaleRatio(/*1.0*/0.05)
    , _zoom_radio(0.9), _root(root), _eye(0, 0, 100)
{
    setName("ortho");
}


OrthoManipulator::~OrthoManipulator(void)
{
}


void OrthoManipulator::setByMatrix( const osg::Matrixd& matrix )
{

}

void OrthoManipulator::setByInverseMatrix( const osg::Matrixd& matrix )
{

}

osg::Matrixd OrthoManipulator::getMatrix() const
{
    return	osg::Matrix::translate(_eye);
}

osg::Matrixd OrthoManipulator::getInverseMatrix() const
{
    return osg::Matrixd::inverse(osg::Matrix::translate(_eye));
}

bool OrthoManipulator::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
    if (ea.getHandled()) return false;

    if(ea.getEventType() == osgGA::GUIEventAdapter::FRAME)
    {
        updateOrthoProject();
    }
    if(ea.getEventType() == osgGA::GUIEventAdapter::SCROLL)
    {
        zoomOn(ea);
        return true;
    }
    if(ea.getEventType() == osgGA::GUIEventAdapter::PUSH)
    {
        flushMouseEventStack();
        addMouseEvent(ea);
        return true;
    }
    if(ea.getEventType() == osgGA::GUIEventAdapter::DRAG)
    {
        addMouseEvent(ea);

        if( _ga_t0.get() == NULL || _ga_t1.get() == NULL )
            return false;

        double deltaX = _ga_t1->getX() - _ga_t0->getX();
        double deltaY = _ga_t1->getY() - _ga_t0->getY();

        double transX = deltaX * _scaleRatio;
        double transY = deltaY * _scaleRatio;

        _eye += osg::Vec3d(transX, transY, 0);
        return true;
    }
    return false;
}

void OrthoManipulator::updateOrthoProject()
{
    int viewWidth = _viewer->getCamera()->getViewport()->width();
    int viewHeight = _viewer->getCamera()->getViewport()->height();

    double left = -double(viewWidth)/2.0 * _scaleRatio ;
    double right = double(viewWidth)* _scaleRatio + left;
    double bottom = double(-viewHeight)/2.0 *_scaleRatio;
    double top = double(viewHeight) * _scaleRatio + bottom;

    _viewer->getCamera()->setProjectionMatrixAsOrtho(left, right,
        bottom, top, -10000.0f, 10000.0f);
}

void OrthoManipulator::home( double )
{
    osg::BoundingBox bounding = computeCloudCenter();

    double xSize = bounding.xMax() - bounding.xMin();
    double ySize = bounding.yMax() - bounding.yMin();

    int viewWidth = _viewer->getCamera()->getViewport()->width();
    int viewHeight = _viewer->getCamera()->getViewport()->height();

    double maxSize = osg::maximum(xSize, ySize);
    double minWidth = osg::minimum(viewWidth,viewHeight);
    double ratio = maxSize/double(minWidth);

    _scaleRatio = ratio;
    _eye = bounding.center() + osg::Vec3d(0, 0, 100);
}

void OrthoManipulator::zoomOn( const osgGA::GUIEventAdapter& ea )
{
    osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();
    double zoom_radio = sm == osgGA::GUIEventAdapter::SCROLL_UP ? _zoom_radio : 1.0 / _zoom_radio;
    double ratio_ = _scaleRatio * zoom_radio;

    int viewWidth = _viewer->getCamera()->getViewport()->width();
    int viewHeight = _viewer->getCamera()->getViewport()->height();

    double x = ea.getX();
    double y = ea.getY();

    double transX =  (ea.getX() - viewWidth / 2 ) * _scaleRatio * (1 - zoom_radio);
    double transY =  (ea.getY() - viewHeight / 2 ) * _scaleRatio * (1 - zoom_radio);

    _eye += osg::Vec3d(transX, transY, 0);

    _scaleRatio = ratio_;
}

void OrthoManipulator::addMouseEvent( const osgGA::GUIEventAdapter& ea )
{
    _ga_t1 = _ga_t0;
    _ga_t0 = &ea;
}

void OrthoManipulator::flushMouseEventStack()
{
    _ga_t1 = 0L;
    _ga_t0 = 0L;
}

void OrthoManipulator::setHomePosition( const osg::Vec3d& center )
{
    _scaleRatio = 0.2;
    _eye = center + osg::Vec3d(0, 0, 50);
}

void OrthoManipulator::setEye(osg::Vec3d& eye)
{
    _eye = eye;
}

osg::BoundingBox osgGA::OrthoManipulator::computeCloudCenter()
{
  //  FindPCLDrawableVistor fpclv;
    
    osg::ComputeBoundsVisitor cbv;
    _root->accept(cbv);
    return  cbv.getBoundingBox();

}