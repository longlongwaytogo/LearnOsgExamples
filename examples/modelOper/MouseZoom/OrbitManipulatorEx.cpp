//#include "pch.h"
#include "OrbitManipulatorEx.h"
#include <osgViewer/Viewer>
 
//https://blog.csdn.net/rest_in_peace/article/details/127006947

#include <math.h>
#include <float.h>

using namespace osgGA;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

double calOrthNearFar(double w, double h)
{
	double dw = fabs(w);
	double dh = fabs(h);
	return max(min(dw, dh) * 100.0, 4000.0);
}


int floatComp(double a, double b, double epson = 1.0e-6)
{
	double c = a - b;
	if (c > epson)
		return 1;
	if (c < -epson)
		return -1;
	return 0;
};

 
float OrbitManipulatorEx::_fov = 45.0f;
float OrbitManipulatorEx::_zNear = 0.25f;
 
bool OrbitManipulatorEx::ZoomPerspective(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{

	//通过变换矩阵获取屏幕上的鼠标点转换到世界坐标下的三维点
	osg::Camera* camera = _pViewer->getCamera();
	osg::Matrix mvpw = camera->getViewMatrix() * camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();

	osg::Matrix inverseMVPW = osg::Matrix::inverse(mvpw);
	osg::Vec3 mouseWorld = osg::Vec3(ea.getX(), ea.getY(), 0) * inverseMVPW;

	//获取当前视点到中心的方向向量
	osg::Vec3 eye, center, up;
	camera->getViewMatrixAsLookAt(eye, center, up);
	_direction = center - eye;
	_direction.normalize();

	//获取视点到鼠标点的方向向量
	osg::Vec3 direction = mouseWorld - eye;
	direction.normalize();

	//记录缩放前的_distance
	double old_dis = _distance;

	//默认缩放操作
	handleMouseWheel(ea, us);

	//记录缩放后的_disance
	double new_dis = _distance;
	if (new_dis == old_dis)
	{
		return false;
	}

	//因为是单位向量，所以余弦值等于两向量的点积
	double res_dot = direction.x() * _direction.x() +
		direction.y() * _direction.y() +
		direction.z() * _direction.z();

	//两次_distance之差
	double d = old_dis - new_dis;
	//沿鼠标点方向移动的距离
	double diff = (old_dis - new_dis) / res_dot; // 相似三角形定理可以推导

#if 0 // 这段代码写的貌似有点不好理解
	//沿鼠标点方向移动后的点
	osg::Vec3 new_eye = eye + osg::Vec3(direction.x() * diff, direction.y() * diff, direction.z() * diff);
	//沿中心方向移动后的点
	osg::Vec3 s = eye + osg::Vec3(_direction.x() * d, _direction.y() * d, _direction.z() * d);
	//位移向量
	osg::Vec3 pan = new_eye - s;
#else
	//位移向量
	osg::Vec3 pan = osg::Vec3(direction.x() * diff, direction.y() * diff, direction.z() * diff)
					- osg::Vec3(_direction.x() * d, _direction.y() * d, _direction.z() * d);
#endif 
	//对_center做位移变换
	osg::Matrix rotateMat(_rotation);
	_center += pan * osg::Matrix::inverse(rotateMat);
	return true;
}

bool OrbitManipulatorEx::ZoomOrtho(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	osg::Vec3 ptWorldBefore, ptWorldAfter;
	osg::Camera* camera = _pViewer->getCamera();
	//计算缩放前的变换矩阵
	{
		osg::Matrix mvpw = camera->getViewMatrix() * camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();
		osg::Matrix inverseMVPW = osg::Matrix::inverse(mvpw);
		ptWorldBefore = osg::Vec3(ea.getX(), ea.getY(), 0) * inverseMVPW;
	}

	double dFactor = 1.0;
	osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();
	if (sm == osgGA::GUIEventAdapter::SCROLL_UP)
	{
		dFactor = 1.2;
	}
	else if (sm == osgGA::GUIEventAdapter::SCROLL_DOWN)
	{
		dFactor = 0.8;
	}

	double dL, dR, dT, dB, dZ, dF;
	camera->getProjectionMatrixAsOrtho(dL, dR, dB, dT, dZ, dF);
	double width, height;

	width = dR - dL;
	height = dT - dB;

	dR = width * dFactor;
	dT = height * dFactor;
	//这里就完成了缩放
	camera->setProjectionMatrixAsOrtho(-dR * 0.5, dR * 0.5, -dT * 0.5, dT * 0.5, dZ, dF);

	//计算缩放后的变换矩阵
	{
		osg::Matrix mvpw = camera->getViewMatrix() * camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();
		osg::Matrix inverseMVPW = osg::Matrix::inverse(mvpw);
		ptWorldAfter = osg::Vec3(ea.getX(), ea.getY(), 0) * inverseMVPW;
	}
	osg::Vec3 vecMove = ptWorldBefore - ptWorldAfter;

	//平移_center
	osg::Matrix rotateMat(_rotation);
	_center += vecMove * osg::Matrix::inverse(rotateMat);
	 
	return false;
}

bool OrbitManipulatorEx::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	osg::View* view = us.asView();
	_pViewer = dynamic_cast<osgViewer::Viewer*>(view);
	switch (ea.getEventType())
	{

	case GUIEventAdapter::FRAME:
		return handleFrame(ea, us);

	case GUIEventAdapter::RESIZE:
		return handleResize(ea, us);

	default:
		break;
	}

	if (ea.getHandled())
		return false;

	switch (ea.getEventType())
	{
	case GUIEventAdapter::MOVE:
		return handleMouseMove(ea, us);

	case GUIEventAdapter::DRAG:
		return handleMouseDrag(ea, us);

	case GUIEventAdapter::PUSH:
		return handleMousePush(ea, us);

	case GUIEventAdapter::RELEASE:
		return handleMouseRelease(ea, us);

	case GUIEventAdapter::KEYDOWN:
	{

		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_O)
		{
			switchToOrtho(_pViewer->getCamera());
		}
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_P)
		{
			switchToPerspective(_pViewer->getCamera());
		}
		return handleKeyDown(ea, us);
	}

	case GUIEventAdapter::KEYUP:
		return handleKeyUp(ea, us);

	case GUIEventAdapter::SCROLL:
		if (_flags & PROCESS_MOUSE_WHEEL)
			if (!isOrthoCamera(_pViewer->getCamera()->getProjectionMatrix()))
				return ZoomPerspective(ea, us);
			else
				ZoomOrtho(ea,us);
		else
			return false;

	default:
		return false;
	}
 
	return false;
}


OrbitManipulatorEx::OrbitManipulatorEx()
{
	_pViewer = nullptr;
	_pCamera = nullptr;
}
 

bool OrbitManipulatorEx::isOrthoCamera(const osg::Matrix& m)
{
	return m(0, 3) == 0 && m(1, 3) == 0.0 && m(2, 3) == 0.0 && m(3, 3) == 1;
}

bool OrbitManipulatorEx::switchToOrtho(osg::Camera* pCamera)
{
	osg::Matrix p = pCamera->getProjectionMatrix();
	osg::Viewport* vp = pCamera->getViewport();
	bool isOrtho = isOrthoCamera(p);
	osg::Vec3d _eye, _center, _up;
	const_cast<OrbitManipulatorEx*>(this)->getTransformation(_eye, _center, _up);
	if (!isOrtho)
	{
		
		setTransformation(_eye, _center, _up);
		double left, right, top, bottom, zNear, zFar;
		p.getFrustum(left, right, bottom, top, zNear, zFar);
		double fov, aspectRatio;
		p.getPerspective(fov, aspectRatio, zNear, zFar);
		//double aspect_ratio = vp->width() / double(vp->height());
		double distance =  (_eye - _center).length();
		// 视景体一半高度
		double halfHeight = tan(osg::DegreesToRadians(fov) / 2.0) * distance;
		// 视景体一半宽度
		double halfWidth = halfHeight * aspectRatio;
		double dfar = calOrthNearFar(halfWidth, halfHeight);
		osg::Matrix orthoMatrix = osg::Matrix::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -dfar, dfar);
		pCamera->setProjectionMatrix(orthoMatrix);
		return true;
	}
	return false;
}

bool OrbitManipulatorEx::switchToPerspective(osg::Camera* pCamera)
{
	osg::Matrix p = pCamera->getProjectionMatrix();
	osg::Viewport* vp = pCamera->getViewport();
	bool isOrtho = isOrthoCamera(p);
	osg::Vec3d _eye, _center, _up;
	const_cast<OrbitManipulatorEx*>(this)->getTransformation(_eye, _center, _up);
	if (isOrtho)
	{
		setTransformation(_eye, _center, _up);
		double left, right, top, bottom, zNear, zFar;
		p.getOrtho(left, right, bottom, top, zNear, zFar);
		double aspectRatio = vp->width() / double(vp->height());
		osg::Matrix perspectiveMat = osg::Matrix::perspective(45.0, aspectRatio, 0.25, zFar);
		pCamera->setProjectionMatrix(perspectiveMat);

		return true;
	}
	return false;
}