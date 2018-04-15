#include <iostream>
#include <osgViewer/viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Node>
#include <osg/Group>
#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osg/GraphicsContext>
#include <osgGA/TrackballManipulator>
#include <osg/Texture2D>
#include <osg/Camera>
#include <osg/Geometry>
#include <osg/Geode>


class SysncOperation:public osg::GraphicsOperation
{

public:
    SysncOperation():osg::GraphicsOperation("SysncOperation",false)
    {

    }
    virtual void operator() (osg::GraphicsContext* context)
    {
        if(context)
        {
            context->makeCurrent();
            if(osgViewer::GraphicsWindow* window = dynamic_cast<osgViewer::GraphicsWindow*>(context))
            {
                window->setSyncToVBlank(false); // 关闭显卡垂直同步
            }
        }
    }
};

int nWidth = 2*800;
int nHeight = 2*800;
int main()
{
	osgViewer::Viewer viewer;
	osg::Node* pNode = osgDB::readNodeFile("cow.osg");

    osg::ref_ptr<osg::Group> root = new osg::Group;
    for(int i = 0; i< 2; ++i)
        for(int j = 0; j <1; ++j)
        {
            osg::ref_ptr<osg::MatrixTransform> transfrom = new osg::MatrixTransform();
            transfrom->addChild(pNode);
            transfrom->setMatrix(osg::Matrix::translate(osg::Vec3(10*i,10*j,0)));
            root->addChild(transfrom);

        }
	

	// 创建纹理
	// Main color texture 
	osg::ref_ptr<osg::Texture2D> p_renderTextureColor = new osg::Texture2D(); 
	p_renderTextureColor->setTextureSize(nWidth,nHeight); 
	p_renderTextureColor->setInternalFormat(GL_RGBA32F_ARB); 
	p_renderTextureColor->setSourceFormat(GL_RGBA); 
	p_renderTextureColor->setSourceType(GL_FLOAT); 
	p_renderTextureColor->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR); 
	p_renderTextureColor->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR); 
	 

	p_renderTextureColor->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE); 
	p_renderTextureColor->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE); 
	//p_renderTextureColor->generateMipmap()

	// Main depth texture 
	osg::ref_ptr<osg::Texture2D> p_renderTextureDepth = new osg::Texture2D(); 
	p_renderTextureDepth->setTextureSize(nWidth,nHeight); 
	p_renderTextureDepth->setSourceFormat(GL_DEPTH_COMPONENT); 
	p_renderTextureDepth->setSourceType(GL_FLOAT); 
	p_renderTextureDepth->setInternalFormat(GL_DEPTH_COMPONENT32F); 
	p_renderTextureDepth->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR); 
	p_renderTextureDepth->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR); 
	p_renderTextureDepth->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE); 
	p_renderTextureDepth->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE); 


	// 创建相机
	osg::ref_ptr<osg::Camera> pCamera = new osg::Camera();
	pCamera->setClearColor(osg::Vec4(0.1f,0.1f,0.3f,1.0f));
	pCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	pCamera->setViewport(0,0,nWidth,nHeight);
	pCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	pCamera->attach(osg::Camera::COLOR_BUFFER, p_renderTextureColor, 0, 0, false, 8, 8); 
	pCamera->setRenderOrder(osg::Camera::PRE_RENDER);
	//pCamera->attach(osg::Camera::DEPTH_BUFFER, p_renderTextureDepth, 0, 0, false, 4, 4); 

	
	osg::ref_ptr<osg::MatrixTransform> transfrom = new osg::MatrixTransform();
	transfrom->addChild(pNode);
	transfrom->setMatrix(osg::Matrix::scale(0.5,0.5,0.5) * osg::Matrix::translate(osg::Vec3(0,0,0)));
	pCamera->addChild(transfrom);
	
	const osg::BoundingSphere& bs = transfrom->getBound();
	if (!bs.valid())
	{
		return 0;
	}

	float znear = 1.0f*bs.radius();
	float zfar  = 3.0f*bs.radius();

	// 2:1 aspect ratio as per flag geometry below.
	float proj_top   = 0.25f*znear;
	float proj_right = 0.5f*znear;

	znear *= 0.9f;
	zfar *= 1.1f;

	// set up projection.
	pCamera->setProjectionMatrixAsFrustum(-proj_right,proj_right,-proj_top,proj_top,znear,zfar);

	// set view
	pCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	pCamera->setViewMatrixAsLookAt(bs.center()-osg::Vec3(0.0f,2.0f,0.0f)*bs.radius(),bs.center(),osg::Vec3(0.0f,0.0f,1.0f));

	root->addChild(pCamera);

	if(root)
	{
		viewer.setSceneData(root);
	}

	osg::Geometry* pGeom = osg::createTexturedQuadGeometry(osg::Vec3(),osg::Vec3(10,0,0),osg::Vec3(0,0,10));
	osg::Geode* pGeode = new osg::Geode;
	pGeode->addDrawable(pGeom);
	root->addChild(pGeode);
	pGeom->getOrCreateStateSet()->setTextureAttributeAndModes(0,p_renderTextureColor,osg::StateAttribute::ON);
	viewer.setRealizeOperation(new SysncOperation());
	viewer.addEventHandler(new osgViewer::StatsHandler());
	//viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	// viewer.setRunMaxFrameRate(40);

    viewer.realize();
	viewer.run();
	return 0;
}