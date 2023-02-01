
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
#include <osgDB/ReaderWriter>
#include <osgUtil/ShaderGen>
#include "OrbitManipulatorEx.h"
osg::Vec3d screenToWorld(osg::Camera* pCamera, osg::Vec3d screen)
{
    osg::Matrixd mvpw = pCamera->getViewMatrix() * pCamera->getProjectionMatrix()
        * pCamera->getViewport()->computeWindowMatrix();
    osg::Matrixd invMVPW = osg::Matrix::inverse(mvpw);

    osg::Vec3d pos = screen * invMVPW;
    return pos;

}

class ModelHandler : public osgGA::GUIEventHandler
{
public:

    osg::ref_ptr<osg::MatrixTransform> m_pickedObj;
    osg::Vec3d m_pickedPoint;
    osg::Vec3d m_picedPt2ObjCenter;

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
    { 
        bool leftDown = ea.getButton() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON;
       
        osg::Camera* pCamera = aa.asView()->getCamera();
        if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && leftDown)
        {
            // pick
            double x = ea.getX();
            double y = ea.getY();
            osg::Vec3d s = screenToWorld(pCamera,osg::Vec3d(x, y, 0.0));
            osg::Vec3d e = screenToWorld(pCamera,osg::Vec3d(x, y, 1.0));
            
            osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(s, e);
             osgUtil::IntersectionVisitor picker(intersector);
            pCamera->accept(picker);

            if (intersector->getIntersections().size() > 0)
            {
                for (auto itr = intersector->getIntersections().begin(); itr != intersector->getIntersections().end(); ++itr)
                {
                    auto nodePath = itr->nodePath;
                    for (int i = 0; i < nodePath.size(); ++i)
                    {
                        if (auto node = dynamic_cast<osg::MatrixTransform*>(nodePath[i]))
                        {
                            m_pickedObj = node;
                            m_pickedPoint = itr->getWorldIntersectPoint();
                            m_picedPt2ObjCenter = m_pickedPoint - node->getBound().center();
                            break;
                        }
                    }
                }
            }
            else
            {
                m_pickedObj = nullptr;
            }
         

        }
        else if(ea.getEventType() == osgGA::GUIEventAdapter::DRAG )
        {
            if (!m_pickedObj)
                return false;
            // move model
            // 获取相机位置
            osg::Vec3d eye = pCamera->getInverseViewMatrix().getTrans();

            // 移动的目标位置
            osg::Vec3d moveToworldPos = screenToWorld(pCamera, osg::Vec3d(double(ea.getX()), double(ea.getY()), 0.0));

#if 0 // 错的代码
           // osg::Vec3d v1 = eye - m_pickedPoint;
           // osg::Vec3d v2 = moveToworldPos - eye;
            osg::Vec3d offset = moveToworldPos - m_pickedPoint;

            osg::Vec3d newPos = m_pickedObj->getMatrix().getTrans() + offset;
            m_pickedObj->setMatrix(osg::Matrix::translate(newPos));

#else
           
            //计算当前摄像机与pick到的模型之间的距离是多少
            osg::Vec3 offset = m_pickedPoint - eye;
            int dist = offset.length();
           
            //计算当前鼠标三维点与摄像机的方向
            osg::Vec3 rayDir = moveToworldPos - eye;
            rayDir.normalize();
            //最后计算物体拖拽时最终的世界位置
            // 移动物体，只是移动物体在眼睛方向上保持远来物体相同的距离
            osg::Vec3 curPos = eye + rayDir * dist;
            m_pickedObj->setMatrix(osg::Matrix::translate(curPos));
#endif 
            return true;
        }
        return false;
    }

};

void configureShaders(osg::StateSet* stateSet)
{
    const std::string vertexSource =
        "#version 130 \n"
        " \n"
        "uniform mat4 osg_ModelViewProjectionMatrix; \n"
        "uniform mat3 osg_NormalMatrix; \n"
        "uniform vec3 ecLightDir; \n"
        " \n"
        "in vec4 osg_Vertex; \n"
        "in vec3 osg_Normal; \n"
        "out vec4 color; \n"
        " \n"
        "void main() \n"
        "{ \n"
        "    vec3 ecNormal = normalize( osg_NormalMatrix * osg_Normal ); \n"
        "    float diffuse = max( dot( ecLightDir, ecNormal ), 0. ); \n"
        "    color = vec4( vec3( diffuse ), 1. ); \n"
        " \n"
        "    gl_Position = osg_ModelViewProjectionMatrix * osg_Vertex; \n"
        "} \n";
    osg::Shader* vShader = new osg::Shader(osg::Shader::VERTEX, vertexSource);

    const std::string fragmentSource =
        "#version 130 \n"
        " \n"
        "in vec4 color; \n"
        "out vec4 fragData; \n"
        " \n"
        "void main() \n"
        "{ \n"
        "    fragData = color; \n"
        "} \n";
    osg::Shader* fShader = new osg::Shader(osg::Shader::FRAGMENT, fragmentSource);

    osg::Program* program = new osg::Program;
    program->addShader(vShader);
    program->addShader(fShader);
    stateSet->setAttribute(program);

    osg::Vec3f lightDir(0., 0.5, 1.);
    lightDir.normalize();
    stateSet->addUniform(new osg::Uniform("ecLightDir", lightDir));
}

int main()
{
    //申请viewer
    osgViewer::Viewer viewer ;
    //读取模型
    osg::MatrixTransform* transform = new osg::MatrixTransform;
    transform->setName("cow");
    osg::ref_ptr<osg::Node>  node = osgDB::readRefNodeFile("cessna.osg",nullptr) ;
    transform->addChild(node);
    //申请一个impostor结点
   // osgSim::Impostor * sim = new osgSim::Impostor ;
    //在0到50000之内显示模型，之外显示贴图
   // sim->addChild (node,0,50) ;
   // sim ->setImpostorThreshold(10) ;
    osg::Group* root = new osg::Group;
    root ->addChild (transform) ;
#if 0
   // configureShaders(root->getOrCreateStateSet());
#else
    osgUtil::ShaderGenVisitor shadergen;
    shadergen.assignUberProgram(root->getOrCreateStateSet());
    root->getOrCreateStateSet()->setDefine("GL_LIGHTING");
   // root->accept(shadergen);
#endif 
    viewer.setSceneData( root );
    viewer.setUpViewInWindow(0, 0, 1024, 768);
   // viewer.addEventHandler(new ModelHandler);
    viewer.realize();
    viewer.setCameraManipulator(new OrbitManipulatorEx());
    viewer.run();
    return 0;
}
