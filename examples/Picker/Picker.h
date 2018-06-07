#include <osgDB/ReadFile>

#include <osgViewer/Viewer>

#include <osg/Geode>

#include <osg/Depth>
#include <osgText/Text>
#include <osgGA/TrackballManipulator>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>

#include <iostream>
#include <sstream>

// 
//
//#pragma comment( lib, "osgd.lib") //.在Debug版本下的库名都加d,如"osgd.lib"
//#pragma comment( lib, "osgDBd.lib")
//#pragma comment( lib, "osgViewerd.lib")
//#pragma comment( lib, "osgTextd.lib")
//#pragma comment( lib, "osgGAd.lib")
//#pragma comment( lib, "osgUtild.lib")


osg::ref_ptr<osg::Group> g_grpMouse;
using namespace std;

//事件类

class CHUD_viewPoint: public osgGA::GUIEventHandler  
{
public:  

	/**构造函数*/
	CHUD_viewPoint(osgText::Text* updateText):
	  m_text(updateText) {}
	  ~CHUD_viewPoint(){}
	  virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
	  void UpdateText(osgViewer::Viewer* viewer,const osgGA::GUIEventAdapter&);
	  /**LABEL*/
	  void setLabel(const std::string& name)
	  {
		  if ( m_text.get())
		  {
			  m_text->setText(name); 
		  }
	  }

protected: 
	osg::Vec2 m_vPosWindowMouse;//鼠标单击处的窗口坐标
	osg::ref_ptr<osgText::Text>  m_text;//视点信息，会动态改变
};