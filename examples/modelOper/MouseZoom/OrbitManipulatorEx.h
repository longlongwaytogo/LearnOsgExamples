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
	virtual bool handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
	
	
	bool onPan(const double eventTimeDelta, const double dx, const double dy);
	bool onZoom(const double eventTimeDelta, const double dx, const double dy);
	bool onRotate(const double eventTimeDelta, const double dx, const double dy);
	bool onRoaming(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);


	
	bool onZoomOrtho(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us, double dy = 0.0, bool offsetCenter = true); // 偏移中心缩放
	bool onZoomPerspective(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us, double dy = 0.0, bool offsetCenter = true);
private:
	std::atomic_bool _bFlyTo;
	osg::Vec3d _flyToPos;
	osg::Vec3d _startPos;
	double _angle;
	double _lastLength;
	osg::Timer_t _lastTime;
	bool _bLeftDown; // roam 按下时的左键
	int _lastFrame;
	
	osg::Vec2i _leftMosueDownPos;
	osg::Vec3d _lastPickedPos;

	osg::Vec2d _lastPanMousePos;

	
public:
	OrbitManipulatorEx();
	static float _fov;
	static float _zNear;
	int _width;
	int _height;
	osg::Camera* _pCamera;
	osgViewer::Viewer* _pViewer;
	//bool _bBtnDown;
	//osg::Vec3d _lastPanMousePos;
	//全图
	void zoomE();
	void zoomE(osg::Vec3d center, double radius);

	// 窗口缩放
	void zoomW(osg::Vec2i start, osg::Vec2i end);

	// 相机定位
	void flyTo(osg::Vec3d pos);

	bool isOrthoCamera(const osg::Matrix& m);
	bool switchToOrtho(osg::Camera* pCamera);
	bool switchToPerspective(osg::Camera* pCamera);
 

	osg::Vec3d _direction;
 

};

