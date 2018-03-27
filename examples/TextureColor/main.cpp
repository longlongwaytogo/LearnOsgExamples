#include <iostream>
#include <osgViewer/viewer>
#include <osg/Node>
#include <osg/Image>
#include <osg/vec4>
#include <osg/Array>
#include <osg/Texture2D>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osgGA/EventHandler>
#include <osgGA/GUIEventHandler>



class KeyboardHandler  :public osgGA::GUIEventHandler
{

public:
	KeyboardHandler(osg::Geometry* pGemoetry)
	{
		m_geometry = pGemoetry;
	}
	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
	{
		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::KEYUP:
			{
				unsigned long long  key = ea.getKey();
				if((osgGA::GUIEventAdapter::KEY_0 < key) && (key < osgGA::GUIEventAdapter::KEY_Z) )
					std::cout<< "keyUp Event,key:"<<std::to_string(key)<<std::endl;
				else
					std::cout<<"KeyUp Event,control key:" <<ea.getKey()<<std::endl;

				osgViewer::Viewer* viewer =  dynamic_cast<osgViewer::Viewer*>(&aa);  

				switch (ea.getKey())
				{
				case osgGA::GUIEventAdapter::KEY_R:
					 
					{
						osg::Array* pArray = m_geometry->getColorArray();
					
						//osg::Vec4* clr  = (pArray[0]);
					}
					break;

				case osgGA::GUIEventAdapter::KEY_T:
					{

						break;
					}
				}
				
				break;

			}
		}
		return true;
	}
	private:
		osg::ref_ptr<osg::Geometry> m_geometry;
};
 

	osg::Geometry* createGeometry()
	{
		std::string imageFile =  "C:\\Users\\Public\\Pictures\\Sample Pictures\\Tulips.jpg";

		osg::Geometry* pGemotry = osg::createTexturedQuadGeometry(osg::Vec3(50,0,0),osg::Vec3(100,0,0),osg::Vec3(0,0,100));
		if(!pGemotry) return nullptr;

		pGemotry->setDataVariance(osg::Object::DYNAMIC);
		osg::Vec4Array* colarray = new osg::Vec4Array;     //颜色RGB值数组
		colarray->push_back(osg::Vec4(0,0,1,1));
		pGemotry->setColorArray(colarray);
		if(osgDB::fileExists(imageFile))
		{

			osg::Image* pImage = osgDB::readImageFile(imageFile);
			if(pImage)
			{
				osg::Texture2D* pTexture = new osg::Texture2D;
				pTexture->setImage(pImage);
				osg::StateSet* pSs = pGemotry->getOrCreateStateSet();
				if(pSs)
				{
					pSs->setTextureAttributeAndModes(0,pTexture,osg::StateAttribute::ON);
				}
			}
		}
		 
	  return pGemotry;
	}


int main()
{
	osgViewer::Viewer viewer;
	osg::Group* pRoot = new osg::Group;
	osg::Geode* pGeode = new osg::Geode;
	osg::Geometry* pGeometry = createGeometry();
	pGeode->addChild(pGeometry);
	pRoot->addChild(pGeode);
	 
	viewer.setSceneData(pRoot);
	viewer.addEventHandler(new KeyboardHandler(pGeometry));
	viewer.realize();
	viewer.run();

	return 0;
}