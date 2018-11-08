#include <iostream>
#include <osg/Node>
#include <osg/Camera>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osg/Referenced>

using namespace  osg;


class HudCallback :public osg::NodeCallback
{

public:
    HudCallback(osgViewer::Viewer* viewer):m_viewer(viewer)
    {
    }
       virtual void operator()(Node* node, NodeVisitor* nv)
       {
           /*
           对模型旋转的理解，本代码中涉及到相机旋转怎么转换倒模型的旋转，可以采用两种思路：
           1.获取当前主相机的视图矩阵，然后作用到hudcamera的视图矩阵中，模型就是正常的放对位置即可。
           2.通过重新合成模型的变换矩阵，模型最终要显示到屏幕，需要经历 v * modelViewMatrx * projection * windowMatrix;
             则，可以对hudcamera设置好初始位置(初始位置也有两种方法，a：设置为OpenGL的默认相机姿态，b：设置为osg中的相机姿态：+z向上），
             然后在更新回调中，将模型矩阵和当前相机的变换矩阵进行合成。

             对于两种合成分别说明：
             a：设置为OpenGL的默认相机姿态:
                hudCamera->setViewMatrixAsLookAt(osg::Vec3(0,0,1),osg::Vec3(0,0,0),osg::Vec3(0,1,0)); // opengl 默认相机位置
             
                则，在更新回调中，需要这样合成：
                osg::Camera* camera = m_viewer->getCamera();
                osg::Vec3 translate = pTM->getMatrix().getTrans();
                osg::Vec3 scale = pTM->getMatrix().getScale();
                osg::Matrix mv = camera->getViewMatrix();

                osg::Matrix inv_mv = camera->getInverseViewMatrix();  // 将视图矩阵转换为正常的变换矩阵
                osg::Quat inv_q = inv_mv.getRotate();  
                osg::Quat q = mv.getRotate();

                mv.setTrans(translate);
                pTM->setMatrix(osg::Matrix::scale(scale) *  mv);

            b: 设置为osg中的相机姿态：
                
                 hudCamera->setViewMatrixAsLookAt(osg::Vec3(0,-1,0),osg::Vec3(0,0,0),osg::Vec3(0,0,1)); // osg 默认相机位置
                 
                 则，在更新回调中，需要这样合成：
                 osg::Camera* camera = m_viewer->getCamera();
                 osg::Vec3 translate = pTM->getMatrix().getTrans();
                 osg::Vec3 scale = pTM->getMatrix().getScale();
                 osg::Matrix mv = camera->getViewMatrix();
                 osg::Quat q = mv.getRotate();

                 // 模型当前所处的旋转坐标系，和相机所处的坐标系存在绕x轴逆时顺时针转90度
                 osg::Quat dq(osg::DegreesToRadians(90.0f),osg::Vec3(1.0f,0.0f,0.0f));

                 mv.setTrans(translate);
                 // 这里dq是用于抵销由opengl坐标系-》osg坐标系变换矩阵，因为最终的矩阵操作为: v * scale * mv * (dq * osgMV) * projection * windowMatrix
                 // dq * osgMV == OpenGL_MV
                 pTM->setMatrix(osg::Matrix::scale(scale) *  mv * osg::Matrix::rotate(dq)); 


                 以上只是理论，b方法在编写代码时出现坐标轴来回闪烁，某一时刻是正确的，某一时刻处于错误位置，反复跳变，但我认为理论结果是正确的，应该是自己写的代码有问题。
           */
               osg::MatrixTransform* pTM = dynamic_cast<osg::MatrixTransform*>(node);
               if(pTM)
               {
                   osg::Camera* camera = m_viewer->getCamera();
                   osg::Vec3 translate = pTM->getMatrix().getTrans();
                   osg::Vec3 scale = pTM->getMatrix().getScale();
                   osg::Matrix mv = camera->getViewMatrix();
            
                   osg::Matrix inv_mv = camera->getInverseViewMatrix();  // 将视图矩阵转换为正常的变换矩阵
                   osg::Quat inv_q = inv_mv.getRotate();  
                   osg::Quat q = mv.getRotate();
               
                   // 模型当前所处的旋转坐标系，和相机所处的坐标系存在绕x轴逆时顺时针转90度
                   osg::Quat dq(osg::DegreesToRadians(90.0f),osg::Vec3(1.0f,0.0f,0.0f));
                  // pTM->setMatrix(osg::Matrix::scale(scale)* osg::Matrix::rotate( dq * q ) * osg::Matrix::translate(translate));
                    static osg::Matrix mm = osg::Matrix::rotate(dq);
                   mv.setTrans(translate);
                   pTM->setMatrix(osg::Matrix::scale(scale) *  mv /** mm*/);
               }
       }
private:
    osg::ref_ptr<osgViewer::Viewer> m_viewer;
};

// HUDAxis 参考了：“图形码农” 的代码

//--------------------- 
//    作者：图形码农 
//    来源：CSDN 
//原文：https://blog.csdn.net/wang15061955806/article/details/49617055 
//版权声明：本文为博主原创文章，转载请附上博文链接！
//
class HUDAxis:public Camera
{
public:
    HUDAxis();
    HUDAxis(HUDAxis const& copy, CopyOp copyOp = CopyOp::SHALLOW_COPY);
    META_Node(osg, HUDAxis);
    inline void setMainCamera(Camera* camera){ _mainCamera = camera;}
    virtual void traverse(NodeVisitor& nv);
protected:
    virtual ~HUDAxis();
    observer_ptr<Camera> _mainCamera;
};
/////////////////////////////////////cpp///////////////////////////////////////////////////
HUDAxis::HUDAxis()
{
    //可以在这直接读取axes.osgt;
    // this->addChild(osgDB::readNodeFile("axes.osgt"));
}
HUDAxis::HUDAxis(HUDAxis const& copy, CopyOp copyOp /* = CopyOp::SHALLOW_COPY */):Camera(copy, copyOp),
    _mainCamera(copy._mainCamera)
{
}
void HUDAxis::traverse(NodeVisitor& nv)
{
    double fovy, aspectRatio, vNear, vFar;
    _mainCamera->getProjectionMatrixAsPerspective(fovy, aspectRatio, vNear, vFar);
    //this->setProjectionMatrixAsOrtho(-10.0*aspectRatio, 10.0*aspectRatio, -10.0, 10.0, 2.0, -2.0); //设置投影矩阵，使缩放不起效果
    this->setProjectionMatrixAsOrtho2D(-10.0*aspectRatio, 10.0*aspectRatio, -10.0, 10.0);
    Vec3 trans(8.5*aspectRatio, -8.5, -8.0);
    if(_mainCamera.valid() && nv.getVisitorType() == NodeVisitor::CULL_VISITOR)
    {
        Matrix matrix = _mainCamera->getViewMatrix();//改变视图矩阵，让移动位置固定
        matrix.setTrans(trans);
        this->setViewMatrix(matrix);
    }//if
    osg::Camera::traverse(nv);
}
HUDAxis::~HUDAxis()
{
}


osg::Camera* createHudCamera(osgViewer::Viewer* viewer,int width = 1024, int height = 1024)
{
    osg::ref_ptr<osg::Camera> hudCamera = new osg::Camera;
    hudCamera->setProjectionMatrixAsOrtho(0,width,0,height,1,100);
    hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
    hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);

    osg::Node* axes = osgDB::readNodeFile("axes.osgt");
    osg::MatrixTransform* pTM = new osg::MatrixTransform;
    pTM->addChild(axes);
    axes->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    pTM->setMatrix(osg::Matrix::scale(osg::Vec3(width/12,width/12,width/12))*osg::Matrix::translate(osg::Vec3(width/20,width/20,1)));
    pTM->setUpdateCallback(new HudCallback(viewer));
    hudCamera->addChild(pTM);
    //hudCamera->setViewMatrixAsLookAt(osg::Vec3(0,-1,0),osg::Vec3(0,0,0),osg::Vec3(0,0,1)); // osg 默认相机位置
    hudCamera->setViewMatrixAsLookAt(osg::Vec3(0,0,1),osg::Vec3(0,0,0),osg::Vec3(0,1,0)); // opengl 默认相机位置

    return hudCamera.release();
}
void main()
{
    osgViewer::Viewer viewer;
    viewer.setUpViewInWindow(0,0,800,600);

    osg::ref_ptr<osg::Group> root = new osg::Group;
    osg::ref_ptr<HUDAxis> hudAxes = new HUDAxis;
    osg::ref_ptr<osg::Node> axes = osgDB::readNodeFile("axes.osgt");
    osg::ref_ptr<osg::Node> cow = osgDB::readNodeFile("cow.osg");
   
    // 使用hudAxes类绘制的坐标系
    hudAxes->addChild(axes);
    hudAxes->setMainCamera(viewer.getCamera());
    hudAxes->setRenderOrder(osg::Camera::POST_RENDER);
    hudAxes->setClearMask(GL_DEPTH_BUFFER_BIT);
    hudAxes->setAllowEventFocus(false);
    hudAxes->setReferenceFrame(Transform::ABSOLUTE_RF);
    root->addChild(hudAxes);
 
    // 使用回调方式创建的坐标系
    root->addChild(createHudCamera(&viewer));
    root->addChild(cow);

    viewer.setSceneData(root);
    viewer.realize();
    viewer.run();
}