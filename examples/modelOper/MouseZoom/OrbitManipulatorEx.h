#pragma once
#include "osgGA\TrackballManipulator"
#include <atomic>

namespace osgViewer
{
	class Viewer;
}

class OrbitManipulatorEx :
    public osgGA::OrbitManipulator
{
	bool ZoomPerspective(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
	bool ZoomOrtho(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

public:
	bool isOrthoCamera(const osg::Matrix& m);
	bool switchToOrtho(osg::Camera* pCamera);
	bool switchToPerspective(osg::Camera* pCamera);
	OrbitManipulatorEx();

	static float _fov;
	static float _zNear;
private:
	int _width;
	int _height;
	osg::Camera* _pCamera;
	osgViewer::Viewer* _pViewer;
	osg::Vec3d _direction;
};

