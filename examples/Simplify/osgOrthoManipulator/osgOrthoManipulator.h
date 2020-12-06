
#ifndef OSGGA_ORTHO_MANIPULATOR
#define OSGGA_ORTHO_MANIPULATOR 1


#include <osgGA\StandardManipulator>
#include <osg\BoundingBox>
#include <osgViewer\Viewer>

namespace osgGA{

    class  OrthoManipulator : public osgGA::CameraManipulator
    {
    public:
        OrthoManipulator(osgViewer::Viewer* viewer, osg::Node* root);
        virtual ~OrthoManipulator(void);
        virtual void setHomePosition(const osg::Vec3d& center);
        virtual void setByMatrix( const osg::Matrixd& matrix );
        virtual void setByInverseMatrix( const osg::Matrixd& matrix );
        virtual osg::Matrixd getMatrix() const;
        virtual osg::Matrixd getInverseMatrix() const;
        virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
        const double& getScaleRation();
        const osg::Vec3d& getEye();
        void setEye(osg::Vec3d& eye);
        void home(double);
    protected:
        void updateOrthoProject();
        void zoomOn(const osgGA::GUIEventAdapter& ea);
        void addMouseEvent(const osgGA::GUIEventAdapter& ea);
        void flushMouseEventStack();
        osg::BoundingBox computeCloudCenter();
    private:
        osg::Vec3d _eye;
        osgViewer::Viewer* _viewer;
        osg::Node* _root;
        double _scaleRatio;
        double _zoom_radio;
        osg::ref_ptr< const osgGA::GUIEventAdapter > _ga_t1;
        osg::ref_ptr< const osgGA::GUIEventAdapter > _ga_t0;
    };

    inline const double& OrthoManipulator::getScaleRation() { return _scaleRatio; }
    inline const osg::Vec3d& OrthoManipulator::getEye() { return _eye; }

}


#endif 