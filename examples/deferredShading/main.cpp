
#include "deferredShading.h"
#include "Utils.h"
#include <osg/AnimationPath>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgShadow/SoftShadowMap>



// 创建场景
 osg::ref_ptr<osg::Group> createSceneRoom();
 void setAnimationPath(osg::ref_ptr<osg::MatrixTransform> node,
                        const osg::Vec3& center, float time, float radius);

 int main(int argc, char** argv)
 {
     osg::ref_ptr<osg::StateSet> ss;

     std::string mediaPath = Utils::Helper::Ins().getMediaPath();
     // Scene 
     osg::Vec3 lightPos(0,0,80);
     osg::ref_ptr<osg::Group> scene = createSceneRoom();
     osg::ref_ptr<osg::LightSource> light = DeferredShading::createLight(lightPos);
     scene->addChild(light.get());

     // Shadowd Scene
     osg::ref_ptr<osgShadow::SoftShadowMap> shadowMap = new osgShadow::SoftShadowMap;
     shadowMap->setJitteringScale(16);
     std::string pass1ShadowPath = mediaPath + "osg_deferred_shading/pass1Shadow.frag";
     shadowMap->addShader(osgDB::readShaderFile(pass1ShadowPath));
     shadowMap->setLight(light);
     osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene = new osgShadow::ShadowedScene;
     shadowedScene->setShadowTechnique(shadowMap.get());
     shadowedScene->addChild(scene.get());

#ifdef USE_EFFECT_COMPOSITOR
      Pipeline p =  DeferredShading::createPipelineEffectCompositor(scene, shadowedScene,lightPos);
#else
     Pipeline p = DeferredShading::createPipelinePlainOSG(scene, shadowedScene, lightPos);
#endif 

     // 显示Dispaly 
     // pass1
     osg::ref_ptr<osg::Camera> qTexN = DeferredShading::createTextureDisplayQuad(osg::Vec3(0,0.7,0),p._pass2Normals,p._textureSize);
     osg::ref_ptr<osg::Camera> qTexP = DeferredShading::createTextureDisplayQuad(osg::Vec3(0,0.35,0),p._pass2Positions,p._textureSize);
     osg::ref_ptr<osg::Camera> qTexC = DeferredShading::createTextureDisplayQuad(osg::Vec3(0,0,0),p._pass2Colors,p._textureSize);

     // pass2  shadow texture
     osg::ref_ptr<osg::Camera> qTexS = DeferredShading::createTextureDisplayQuad(osg::Vec3(0.7,0.7,0),p._pass1Shadows,p._textureSize);

     // pass3 final text
     osg::ref_ptr<osg::Camera> qTexFinal = DeferredShading::createTextureDisplayQuad(osg::Vec3(0,0,0),p._pass3Final,p._textureSize,1,1);
     
     // Must be processed before the first pass takes
     // the result into pass1Shadow texture
     p._graph->insertChild(0,shadowedScene.get());
     // Quads are displayed in order, so the biggest one (final) must be first,
     // otherwise other quads won't be visible.

     p._graph->addChild(qTexFinal.get());
     p._graph->addChild(qTexN.get());
     p._graph->addChild(qTexP.get());
     p._graph->addChild(qTexC.get());
     p._graph->addChild(qTexS.get());

     // Display everything 
     osgViewer::Viewer viewer;
     
     viewer.addEventHandler( new osgViewer::ScreenCaptureHandler(
                                new osgViewer::ScreenCaptureHandler::WriteToFile(
                                "screenshot",
                                "png",
                                osgViewer::ScreenCaptureHandler::WriteToFile::OVERWRITE)));
                                
    viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.setSceneData(p._graph.get());
    viewer.setRunMaxFrameRate(50);
    viewer.setUpViewInWindow(300,100,800,600);

     return viewer.run();

 }

 osg::ref_ptr<osg::Group> createSceneRoom()
 {
     // Romm 
     std::string mediaPath = Utils::Helper::Ins().getMediaPath();
     osg::ref_ptr<osg::MatrixTransform> room = new osg::MatrixTransform;
     std::string simpleRoomPath =  mediaPath + "osg_deferred_shading/simpleroom.osgt";
     osg::ref_ptr<osg::Node> roomModel = osgDB::readNodeFile(simpleRoomPath);
     room->addChild(roomModel);
     room->setMatrix(osg::Matrix::translate(0,0,1));
     
     // small room
     osg::ref_ptr<osg::MatrixTransform> smallRoom = new osg::MatrixTransform;
     smallRoom->addChild(roomModel);
     smallRoom->setMatrix(osg::Matrix::translate(70,0,-30) );

     // Torus
     osg::ref_ptr<osg::MatrixTransform> torus = new osg::MatrixTransform;
     std::string torusPath = mediaPath + "osg_deferred_shading/torus.osgt";
     osg::ref_ptr<osg::Node> torusModel = osgDB::readNodeFile(torusPath);
     torus->addChild(torusModel);
     setAnimationPath(torus, osg::Vec3(0,0,15),6,16);

     // Torus2.
     osg::ref_ptr<osg::MatrixTransform> torus2 = new osg::MatrixTransform;
     torus2->addChild(torusModel);
     setAnimationPath(torus2, osg::Vec3(-20, 0, 10), 20, 0);
   
     // Torus3.
     osg::ref_ptr<osg::MatrixTransform> torus3 = new osg::MatrixTransform;
     torus3->addChild(torusModel);
     setAnimationPath(torus3, osg::Vec3(0, 0, 40), 3, 25);

     // scene
     osg::ref_ptr<osg::Group> scene = new osg::Group;
     scene->addChild(room);
     scene->addChild(smallRoom);
     scene->addChild(torus);
     scene->addChild(torus2);
     scene->addChild(torus3);

     // Alter room's bump'ness
     osg::StateSet* ss = smallRoom->getOrCreateStateSet();
     std::string wallPath = mediaPath + "osg_deferred_shading/wall.png";
     ss->setTextureAttributeAndModes(0,DeferredShading::createTexture(wallPath));
     ss->addUniform(new osg::Uniform("diffMap",0));
     ss->addUniform(new osg::Uniform("useBumpMap",0));

     return scene;
  }

 void setAnimationPath(osg::ref_ptr<osg::MatrixTransform> node,
     const osg::Vec3& center, float time, float radius)
 {
     // Create Animation
     osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
     path->setLoopMode(osg::AnimationPath::LOOP);
     unsigned int numSamples = 32;
     float delta_yaw = 2.0f * osg::PI / (static_cast<float>(numSamples) - 1.0f);
     float delta_time  = time / static_cast<float>(numSamples);

     for(unsigned int i = 0; i < numSamples; ++i)
     {

         float yaw = delta_yaw * static_cast<float>(i);
         osg::Vec3 pos(center.x() + sinf(yaw)* radius,
                       center.y() + cosf(yaw)* radius,
                       center.z());
         osg::Quat rot(-yaw, osg::Z_AXIS);
         path->insert(delta_time * static_cast<float>(i),
                    osg::AnimationPath::ControlPoint(pos,rot));
     }

     node->setUpdateCallback(new osg::AnimationPathCallback(path));

 }