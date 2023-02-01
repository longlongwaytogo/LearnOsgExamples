//#include "pch.h"
#include "OrbitManipulatorEx.h"
#include <osgViewer/Viewer>
 
//https://blog.csdn.net/rest_in_peace/article/details/127006947

#include <math.h>
#include <float.h>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#define USE_PIXEL_COMPUTE_DISTANCE 0
#define MOVEUSETIME 2000
#define PRECISION_LOW 1.0e-4
#define PRECISION_HIG 1.0e7

using namespace osgGA;

double calOrthNearFar(double w, double h)
{
	double dw = fabs(w);
	double dh = fabs(h);
	return max(min(dw, dh) * 100.0, 4000.0);
}
osg::Vec3d screen2World(osg::Camera* pCamera, osg::Vec3d pos)
{
	osg::Matrixd mvpw = pCamera->getViewMatrix() * pCamera->getProjectionMatrix() * pCamera->getViewport()->computeWindowMatrix();
	osg::Matrixd inv = mvpw.inverse(mvpw);
	return pos * inv;
}


osg::Vec3d world2Screen(osg::Camera* pCamera, osg::Vec3d pos)
{
	osg::Matrixd mvpw = pCamera->getViewMatrix() * pCamera->getProjectionMatrix() * pCamera->getViewport()->computeWindowMatrix();
	return pos * mvpw;
}

osg::Vec3d screen2View(osg::Camera* pCamera, osg::Vec3d pos)
{
	osg::Matrix screenMat = pCamera->getViewport()->computeWindowMatrix();
	osg::Vec4d d2 = osg::Vec4d(pos.x(), pos.y(), pos.z(), 1.0) * osg::Matrix::inverse(screenMat);
	osg::Matrix mvpw =  pCamera->getProjectionMatrix() * pCamera->getViewport()->computeWindowMatrix();
	osg::Matrix inv = mvpw.inverse(mvpw);
	osg::Vec4d dd = osg::Vec4d(pos.x(), pos.y(), pos.z(), 1.0) * inv;
	dd = dd / dd.w();
	return pos * inv;
}

osg::Vec3d View2Screen(osg::Camera* pCamera, osg::Vec3d pos)
{
	osg::Matrix mvpw =pCamera->getProjectionMatrix() * pCamera->getViewport()->computeWindowMatrix();
	return pos * mvpw;
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

osgGA::GUIEventAdapter* _ea = nullptr;
osgGA::GUIActionAdapter* _us = nullptr;
 

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

bool OrbitManipulatorEx::handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();
	 
	// handle centering
	if (_flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT)
	{

		if (((sm == osgGA::GUIEventAdapter::SCROLL_DOWN && _wheelZoomFactor > 0.)) ||
			((sm == osgGA::GUIEventAdapter::SCROLL_UP && _wheelZoomFactor < 0.)))
		{

			if (getAnimationTime() <= 0.)
			{
				// center by mouse intersection (no animation)
				setCenterByMousePointerIntersection(ea, us);
			}
			else
			{
				// start new animation only if there is no animation in progress
				if (!isAnimating())
					startAnimationByMousePointerIntersection(ea, us);

			}

		}
	}

	//osg::Camera* pCamera = us.asView()->getCamera();
	//if (isOrthoCamera(pCamera->getProjectionMatrix()))
	//{
	//	return onZoomOrtho(ea, us);
	//}
	//else
	//{
	//	//return onZoomPerspective(*_ea, *_us);
	//	if(sm ==  osgGA::GUIEventAdapter::SCROLL_UP)
	//		return onZoomPerspective(*_ea, *_us, -_wheelZoomFactor, true);
	//	else if (sm == osgGA::GUIEventAdapter::SCROLL_DOWN)
	//		return onZoomPerspective(*_ea, *_us, _wheelZoomFactor, true);
	//}
	switch (sm)
	{
		// mouse scroll up event
	case osgGA::GUIEventAdapter::SCROLL_UP:
	{
		// perform zoom
		zoomModel(-_wheelZoomFactor, true);
		us.requestRedraw();
		us.requestContinuousUpdate(isAnimating() || _thrown);
		return true;
	}

	// mouse scroll down event
	case osgGA::GUIEventAdapter::SCROLL_DOWN:
	{
		// perform zoom
		zoomModel(_wheelZoomFactor, true);
		us.requestRedraw();
		us.requestContinuousUpdate(isAnimating() || _thrown);
		return true;
	}

	// unhandled mouse scrolling motion
	default:
		return false;
	}
}

bool OrbitManipulatorEx::onPan(const double eventTimeDelta, const double dx, const double dy)
{
	// pan model
	float scale = 1.0;// -0.3f * _distance * getThrowScale(eventTimeDelta);
	
	panModel(dx * scale, dy * scale);
	return true;

}
bool OrbitManipulatorEx::onZoom(const double eventTimeDelta, const double dx, const double dy)
{
	// zoom model
	zoomModel(dy * getThrowScale(eventTimeDelta), true);
	return true;
}
bool OrbitManipulatorEx::onRotate(const double eventTimeDelta, const double dx, const double dy)
{
	// rotate camera
	if (getVerticalAxisFixed())
		rotateWithFixedVertical(dx, dy);
	else
		rotateTrackball(_ga_t0->getXnormalized(), _ga_t0->getYnormalized(),
			_ga_t1->getXnormalized(), _ga_t1->getYnormalized(),
			getThrowScale(eventTimeDelta));

	return true;
}
 
inline bool OrbitManipulatorEx::onZoomOrtho(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us, double dy /*= 0.0f*/, bool offsetCenter/* = true*/)
{
	return true;
}

bool OrbitManipulatorEx::onZoomPerspective(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us, double dy, bool offsetCenter)
{
	{
		double l, r, b, t, n, f;
		_pCamera->getProjectionMatrixAsFrustum(l, r, b, t, n, f);
		printf("frustum:%f,%f,%f,%f\n", l, r, b, t);
	}
	bool  pushForwardIfNeeded = true;
	float scale = 1.0f + dy*1;
	// 计算鼠标在屏幕占比例
	double x = (ea.getX() - ea.getXmin()) / ea.getXmax();
	double y = (ea.getY() - ea.getYmin()) / ea.getYmax();
	// 从0，1---变换到-1--+1范围
	x -= 0.5;
	y -= 0.5;
	x *= 2;
	y *= 2;

	osg::Camera* pCamera = us.asView()->getCamera();
	osg::Vec3d eye, center, up;
	getTransformation(eye, center, up);

	auto ZoomPerspective = [this, &eye, &center, &up](osg::Camera* pCamera, double dx, double dy)-> void {
		if (pCamera)
		{
			osg::Matrix m = pCamera->getProjectionMatrix();
			{
				double fovy = 0;
				double aspectRatio = 0;
				double zNear = 0;
				double zFar = 0;
				m.getPerspective(fovy, aspectRatio, zNear, zFar);
				// 离地表的高度作为缩放控制因子
				double dDistance = fabs(eye.z());
				dDistance = osg::clampBetween<double>(dDistance, 1.0, 20000);
				// 获取视线方向与up方向的夹角
				osg::Vec3d vFoward = center - eye;
				osg::Vec3d vFowardOrigin = vFoward;
				vFoward.normalize();
			
				double speed = 1.0;// 0.5;
				dDistance = _distance;
				double dHeight = tan(osg::DegreesToRadians(fovy * 0.5)) * dDistance * speed;
				dHeight = fabs(dHeight);
				double dWidth = aspectRatio * dHeight;

				osg::Vec3d vRight = vFoward ^ up;
				vRight.normalize();
				osg::Vec3d vUp = vRight ^ vFoward;
				vUp.normalize();

				eye -= vRight * dWidth * dx;
				eye -= vUp * dHeight * dy;

				center = eye + vFowardOrigin;

				setTransformation(eye, center, up);
			}
		}
	};

	if (offsetCenter)
	{
		ZoomPerspective(pCamera, -x, -y);
	}
	// minimum distance
	float minDist = _minimumDistance;
	if (getRelativeFlag(_minimumDistanceFlagIndex))
		minDist *= _modelSize;
#if 1
	if (_distance * scale > minDist)
	{
		// regular zoom
		_distance *= scale;
	}
	else
	{
		pushForwardIfNeeded = true;
		if (pushForwardIfNeeded)
		{
			// push the camera forward
			float yscale = -_distance;
			osg::Matrixd rotation_matrix(_rotation);
			osg::Vec3d dv = (osg::Vec3d(0.0f, 0.0f, -1.0f) * rotation_matrix) * (dy * yscale);
			_center += dv;
		}
		else
		{
			// set distance on its minimum value
			_distance = minDist;
		}
	}

#else
	
	{
		float yscale = -10;
		osg::Matrixd rotation_matrix(_rotation);
		osg::Vec3d dv = (osg::Vec3d(0.0f, 0.0f, -1.0f) * rotation_matrix) * (dy * yscale);
		_center += dv;
		
	}
	//getTransformation(eye, center, up);
	//osg::Vec3d forward = center - eye;
	//float d = forward.length();
	//forward *= dy;
	//eye = eye + forward;
	//center = center + forward;
	//setTransformation(eye, center, up);
#endif 
	if (offsetCenter)
	{
		getTransformation(eye, center, up);
		ZoomPerspective(pCamera, x, y);
	}
	printf("distance:%f,eye:%f,%f,%f\n", _distance,eye.x(),eye.y(),eye.z());
	return true;
}



OrbitManipulatorEx::OrbitManipulatorEx(): 
	_bFlyTo(false), 
	_bLeftDown(false),
	_lastFrame(-1)
{
	_allowThrow = false;
	_lastPickedPos.set(0, 0, 0);
	_lastPanMousePos.set(0, 0);
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
		//OcGePoint3d eye = OcGePoint3d(_eye.x(), _eye.y(), _eye.z());
		//OcGePoint3d center = OcGePoint3d(_center.x(), _center.y(), _center.z());
		//OcGeLine3d line(eye, center);
		//OcGeRay3d ray(eye, center);
		//OcGePlane pln = OcGePlane::kXYPlane;
		//OcGePoint3d intersectPt;
		//if (line.intersectWith(pln, intersectPt))
		//	//if(ray.intersectWith(pln,intersectPt))
		//{
		//	if (intersectPt != eye)// 交点如果不是起点
		//	{
		//		center = intersectPt;
		//		_center = osg::Vec3d(center.x, center.y, center.z);
		//	}
		//}
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
	/*	OcGePoint3d eye(_eye.x(), _eye.y(), _eye.z());
		OcGePoint3d center(_center.x(), _center.y(), _center.z());
		OcGeVector3d foward = center - eye;
		foward.normalize();
		OcGePoint3d intersectPt;
		OcGePlane pln = OcGePlane::kXYPlane;
		OcGeLine3d line(eye, center);
		OcGeRay3d ray(eye, center);
		if (line.intersectWith(pln, intersectPt))*/
			//if (ray.intersectWith(pln, intersectPt))
		//{
		//	if (intersectPt != eye)// 交点如果不是起点
		//	{
		//		center = intersectPt;
		//		_center = osg::Vec3d(center.x, center.y, center.z);
		//	}
		//}
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