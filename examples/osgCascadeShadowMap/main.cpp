#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ViewDependentShadowMap>
#include <osgShadow/ViewDependentShadowTechnique>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgShadow/ShadowMap>
#include <osgShadow/StandardShadowMap>
#include <osgShadow/ShadowTexture>
#include <osgShadow/ShadowVolume>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>
#include <osg/Transform>

#include <osg/Light>
#include <osg/LightSource>

#include "CommonFunctions"

void main( int argc, char** argv )
{
    unsigned int rcvShadowMask = 0x1;
    unsigned int castShadowMask = 0x2;
    
    // Set the ground (only receives shadow)
    osg::ref_ptr<osg::MatrixTransform> groundNode = new osg::MatrixTransform;
        groundNode->setMatrix(osg::Matrix::translate(osg::Vec3(0.0,0.0,-10.0)));
        groundNode->addChild( osgDB::readNodeFile("lz.osg") );
    groundNode->setNodeMask( rcvShadowMask);
    


        osg::ref_ptr<osg::MatrixTransform> louNode2 = new osg::MatrixTransform;
        louNode2->setMatrix(osg::Matrix::rotate(osg:egreesToRadians(90.0),0.0,0.0,1.0)*osg::Matrix::translate(osg::Vec3(0.0,0.0,10.0)));
        louNode2->addChild( osgDB::readNodeFile("1lou.ive") );
        louNode2->setNodeMask( castShadowMask|rcvShadowMask );

        osg::ref_ptr<osg::MatrixTransform> louNode3 = new osg::MatrixTransform;
        louNode3->setMatrix(osg::Matrix::translate(osg::Vec3(-150.0,0.0,100.0)));
        louNode3->addChild( osgDB::readNodeFile("cessna.osg") );
        louNode3->setNodeMask( castShadowMask|rcvShadowMask );

    
        osg::ref_ptr<osgShadow:arallelSplitShadowMap> pssm = new osgShadow::ParallelSplitShadowMap(NULL,3);
        int mapres = 1024;
        pssm->setTextureResolution(mapres);

        osg::ref_ptr<osgShadow::StandardShadowMap> vdsm = new osgShadow::StandardShadowMap;


    // Set shadow node
    osg::ref_ptr<osgShadow::ShadowMap> sm = new osgShadow::ShadowMap;
    //vdsm->setShadowMapProjectionHint( osgShadow::ViewDependentShadowMap::ORTHOGRAPHIC_SHADOW_MAP );
    //vdsm->setBaseShadowTextureUnit( 1 );
    
        osg::ref_ptr<osgShadow::SoftShadowMap> ssm = new osgShadow::SoftShadowMap;
        ssm->setSoftnessWidth(0.0);
        // ssm->setAmbientBias(osg::Vec2(0.7,0.7));//0.5,0.5
        /*float bias = ssm->getBias();
        ssm->setBias(bias*2.0);
*/
        osg::ref_ptr<osgShadow::ShadowVolume> sv = new osgShadow::ShadowVolume;

    osg::ref_ptr<osgShadow::ShadowedScene> shadowRoot = new osgShadow::ShadowedScene;
        osg::ref_ptr<osgShadow::ShadowSettings> ss= new osgShadow::ShadowSettings;
        ss->setShaderHint(osgShadow::ShadowSettings::PROVIDE_VERTEX_AND_FRAGMENT_SHADER);
        shadowRoot->setShadowSettings(ss);
    shadowRoot->setShadowTechnique( sm.get() );
    shadowRoot->setReceivesShadowTraversalMask( rcvShadowMask );
    shadowRoot->setCastsShadowTraversalMask( castShadowMask );
    
        shadowRoot->addChild( groundNode.get() );
        //shadowRoot->addChild( louNode.get() );
        shadowRoot->addChild( louNode2.get() );
        shadowRoot->addChild( louNode3.get() );




        osg::ref_ptr<osg:ight> light = new osg::Light();
        light->setLightNum(0);

        light->setPosition(osg::Vec4(1.0,0.0,1.0,0.0));
        //light->setDirection(osg::Vec3(-1.0,0.0,-1.0));
        light->setAmbient(osg::Vec4(0.0,0.0,0.0,1.0));
        light->setDiffuse(osg::Vec4(1.0,1.0,1.0,1.0));


        osg::ref_ptr<osg::LightSource> lightsource = new osg::LightSource();
        lightsource->setLight(light.get());

        osg::ref_ptr<osg::Light> light1 = new osg::Light();
        light1->setLightNum(1);

        light1->setPosition(osg::Vec4(100.0,0.0,100.0,0.0));
        light1->setAmbient(osg::Vec4(1.0,1.0,1.0,1.0));
        light1->setDiffuse(osg::Vec4(1.0,1.0,1.0,1.0));


        osg::ref_ptr<osg::LightSource> lightsource1 = new osg::LightSource();
        lightsource1->setLight(light1.get());

        sm->setLight(light);
        pssm->setUserLight(light);
        pssm->setPolygonOffset(osg::Vec2(-1.0,-1.0));
        //ssm->setLight(light);
        shadowRoot->addChild( lightsource.get() );
        shadowRoot->addChild( lightsource1.get() );

        
    osg::ref_ptr<osg::Group>root  = new osg::Group;
        root->addChild(shadowRoot);
        root->getOrCreateStateSet()->setMode(GL_LIGHT1,osg::StateAttribute::ON);
        root->getOrCreateStateSet()->setMode(GL_LIGHT0,osg::StateAttribute::ON);
        //root->addChild(lightsource);

    osgViewer::Viewer viewer;
    viewer.setSceneData( root.get() );
        viewer.addEventHandler( new osgViewer::StatsHandler );
        viewer.addEventHandler( new osgViewer::WindowSizeHandler );
        viewer.setCameraManipulator(new osgGA::TrackballManipulator);
        while (!viewer.done())
        {
                viewer.frame();
        }
}
