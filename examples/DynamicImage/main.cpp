#include <osg/Image>
#include <osg/Node>
#include <osg/Texture2D>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>
#include <osg/Vec3>
#include <osg/Geometry>
#include <osg/NodeCallback>
#include <osg/NodeVisitor>


void updateImageData(osg::Image* img,osg::Vec4 color,bool bAllocate = true)
{
    int w = 8;
    int h = 8;
    if(bAllocate)
        img->allocateImage(w,h,1,GL_RGBA,GL_FLOAT,1);
    unsigned char* data = img->data();

     for(unsigned int r=0;r<h;++r)
    {
        for(unsigned int c=0;c<w;++c)
        {
            *((osg::Vec4f*)img->data(c,r)) = color;
        }
    }

     if(!bAllocate)
         img->dirty();
	
}
class ImageUpdateCallback :public osg::NodeCallback
{
public:
    
    class ImageUpdateCallback(osg::Image* img,osg::Texture2D*tex)
    {
        _img = img;
        _tex = tex;
        m_num = 0;
    }
    virtual void operator()(osg::Node* node,osg::NodeVisitor* nv)
    {
        osg::StateSet* ss = node->getOrCreateStateSet();
        m_num++;
        if(ss)
        {
            osg::Vec4 clr(0,0,0,1.0);
            
            if(m_num ==100)
            {
                std::cout << m_num << std::endl;
                clr = osg::Vec4(1,0,1,1.0);
                 updateImageData(_img,clr,false);
            }
            else if(m_num == 200)
            {
                clr = osg::Vec4(1.0,1.0,0.0,1.0);
                 updateImageData(_img,clr,false);
                std::cout << m_num << std::endl;

            }
            else if(m_num ==300)
            {
                 clr = osg::Vec4(0,1.0,0.0,1.0);
                  updateImageData(_img,clr,false);
                std::cout << m_num << std::endl;

            }
            else if(m_num > 400)
            {
                m_num  = 0;
                std::cout << m_num << std::endl;

            }
          
        }
    }
private:
    osg::ref_ptr<osg::Image> _img;
    osg::ref_ptr<osg::Texture2D> _tex;
    unsigned int m_num;
};



void main()
{

    osg::ref_ptr<osg::Image> img = NULL;
    osg::ref_ptr<osg::Texture2D> tex = NULL;
    osgViewer::Viewer viewer;

    osg::ref_ptr<osg::Group> root = new osg::Group;
    viewer.setSceneData(root);

    osg::Node* pNode = osg::createTexturedQuadGeometry(osg::Vec3(0,0,0),osg::Vec3(10,0,0),osg::Vec3(0,0,10));
    root->addChild(pNode);
   

    osg::StateSet* ss = pNode->getOrCreateStateSet();
    if(ss)
    {
        img = new osg::Image();//osgDB::readImageFile("Images/skymap.jpg");
        
        img->setDataVariance(osg::Object::DYNAMIC);

        osg::Vec4 clr = osg::Vec4(1.0,0.0,0.0,1.0);
        updateImageData(img,clr);

        tex = new osg::Texture2D;
        tex->setImage(img);
        tex->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
        tex->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);
        tex->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
        tex->setSourceFormat(GL_RGBA);
        tex->setSourceType(GL_FLOAT);
        tex->setInternalFormat(GL_RGBA);
        

        ss->setTextureAttributeAndModes(0,tex,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
    }

    pNode->addUpdateCallback(new ImageUpdateCallback(img,tex));
    viewer.setUpViewInWindow(10,10,800,600);
    viewer.realize();
    viewer.run();
        
}