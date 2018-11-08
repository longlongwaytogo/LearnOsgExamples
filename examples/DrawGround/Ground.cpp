#include <osgViewer/viewer>

#include <osg/PrimitiveSet>
#include <osg/StateSet>
#include <osg/LineWidth>
#include <osg/PolygonMode>
#include <osg/StateAttribute>
#include <osg/PrimitiveRestartIndex>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/array>
#include <osg/Hint>
#include <osg/BlendColor>
#include <osg/BlendEquation>
#include <osg/BlendFunc>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgGA/TrackballManipulator>
#include <osgGA/GUIEventHandler>

#include <iostream>

// 使用图元重启绘制网格
// dimessionX: X方向上细分多少个点
// dimessionY: Y方向上细分多少个点
// width:  X方总向长度
// height: Y方向总长度
// bCoordinateAllMesh 将整个图片映射到mesh上(true)，还是映射到每个单元中(false)
// bFill：是否使用多边形填充（true),line模式（false)
osg::Node* CreateGround(int dimensionX = 5, int dimensionY= 5, float width = 80, float height = 80,bool bCoordinateAllMesh = true,bool bFill = true)
{
    // 网格绘制，按照列序进行绘制，每一列为一组Quad_Strip,然后插入重启绘制标识符。
	float dx = width / (dimensionX - 1); // x方向，每段线的长度
	float dy = height / (dimensionY - 1); // y方向，每段线的长度
	float startX = - width / 2;
	float startY = - height / 2;
    float endX =  - startX;
    float endY = - startY;

	osg::ref_ptr<osg::Geode> node = new osg::Geode;
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> coords = new osg::Vec2Array;
  
    // 索引数组个数
    // 2*dimessionX * dimessionY 标识所有顶点都重复使用
    // - dimensionY * 2  减掉 网格开始和结束的两条边界
    // (dimessionX -2) 重启索引个数
    int indicesSize = 2 * dimensionX * dimensionY - dimensionY * 2 + (dimensionX - 2);
	osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::QUAD_STRIP, indicesSize);
	unsigned int restartFlag = 0xFFFF;
	// 使用图元重启绘制网格
	/*	
	  X1 Y1  ----  X2,Y1
			|	|
			|	|
	 X1,Y2	----- X2,Y2
	*/

    // calculate vertices 
    for(int i = 0; i < dimensionX; ++i)
    {
        for(int j = 0; j < dimensionY; ++j)
        {
            float x1 = startX + dx * i;
            float y1 = startY + dy * j;
            osg::Vec3 pos1(x1, y1, 0);
            vertices->push_back(pos1);
            
           
            if(bCoordinateAllMesh)
            {
                osg::Vec2 coord(float(i)/(dimensionX - 1),float(j)/(dimensionY - 1));
                coords->push_back(coord);
            }
            else
            {
                osg::Vec2 coord(i,j);
                coords->push_back(coord);
            }
        }
    }
    vertices->push_back(osg::Vec3(endX,endY,0));// 多存储一个顶点，用作因使用图元重启，而导致计算包围盒访问越界。
    // calculate indices
    // fist quad strip 
    for (int j=0;j<dimensionY;j++)
    {
        (*indices)[j*2] = j;
        (*indices)[j*2+1] = dimensionY+j;
        
        // 设置纹理坐标
        if(!bCoordinateAllMesh)
        {
             
           
        }
      
        std::cout <<  j*2 <<"," <<  j*2+1 << "-->"<< j <<"," << dimensionY +j <<std::endl;
    }
    std::cout << std::endl;

	unsigned int restartIndex = 0;
	for (int i = 1; i < dimensionX - 1 ; ++i)  
	{
        restartIndex += 2 * dimensionY;
     // 重启索引所设置的标识符, 此处不能这么设置，必须设置为一个特殊的点，比如，最后一个顶点,因为osg在计算包围盒时，
     // 会通过索引去访问顶点,若为0xFFFF,则出现访问越界，所以使用最后一个顶点
    // (*indices)[restartIndex] = restartFlag;
       (*indices)[restartIndex] = dimensionX * dimensionY;
        restartIndex++; // 存储下一个有效的索引
		for (int j = 0; j < dimensionY; ++j) // 每次绘制一列的quad_strip
		{
		 
            (*indices)[restartIndex + j*2] = dimensionY*i + j; // 计算当前顶点索引
            (*indices)[restartIndex + j*2+1] = dimensionY*(i+1) + j;
          
			std::cout << restartIndex + j*2 <<"," << restartIndex + j*2+1 << "-->"<< dimensionY*i + j <<"," << dimensionY*(i+1) + j<<std::endl;
		}
        std::cout << std::endl;
	}
 
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	geometry->addPrimitiveSet(indices);
    //geometry->addPrimitiveSet(new osg::DrawArrays(GL_QUAD_STRIP,0,dimensionX * dimensionY));
	geometry->setVertexArray(vertices);
	geometry->setTexCoordArray(0, coords);

	normals->push_back(osg::Vec3d(0, 0, 1));
	geometry->setNormalArray(normals, osg::Array::BIND_PER_PRIMITIVE_SET);

	osg::StateSet* stateset = new osg::StateSet();
	osg::PolygonMode* pm = new osg::PolygonMode(osg::PolygonMode::Face::FRONT_AND_BACK, osg::PolygonMode::Mode::LINE);
	stateset->setAttributeAndModes(pm, osg::StateAttribute::ON);
	stateset->setAttributeAndModes(new osg::LineWidth(1.5f), osg::StateAttribute::ON);

    if(bFill)
    {
	
	    stateset->setMode(GL_POLYGON_SMOOTH, osg::StateAttribute::ON);
        stateset->setAttributeAndModes(new osg::Hint(GL_POLYGON_SMOOTH_HINT, GL_NICEST), osg::StateAttribute::ON);
        // 开启融合并启用抗锯齿，但会出现网格线，所以先屏幕融合
        // stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
        // stateset->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA),osg::StateAttribute::ON);
    }
    else
    {
        stateset->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
        stateset->setAttributeAndModes(new osg::Hint(GL_LINE_SMOOTH_HINT, GL_NICEST), osg::StateAttribute::ON);
        stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
        stateset->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA),osg::StateAttribute::ON);
    }

    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    stateset->setAttributeAndModes(new osg::PrimitiveRestartIndex(dimensionX * dimensionY), osg::StateAttribute::ON);
    stateset->setMode(GL_PRIMITIVE_RESTART, osg::StateAttribute::ON);

	//std::string fileName = "Images/primitives.gif";
    std::string fileName = "E:\\project\\osg\\OSG.ms\\OsgEditor\\Data\\textures\\ground3.jpg";
	osg::Image* image = osgDB::readImageFile(fileName);
	osg::Texture2D* tex = new osg::Texture2D;
    	/* mipmap 使用说明
	其中GL_NEAREST_MIPMAP_NEAAREST具有很好的性能，也能够解决闪烁的问题，但在视觉效果上会比较差。
	其中GL_LINEAR_MIPMAP_NEAREST常用于游戏加速，使用了质量较高的线性过滤，和快速的选择的方式(最邻近方式）。
	使用最邻近的方式作为mipmap选择器的效果依然不能令人满意。从某一个角度去看，常常可以看到物体表面从一个mip层
	到另一个mip层的转变。GL_LINEAR_MIPMAP_LINEAR和GL_NEAREST_MIPMAP_LINEAR过滤器在mip层之间执行一些额外的线性插值，
	以消除不同层之间的变换痕迹，但也需要一些额外的性能开销。GL_LINEAR_MIPMAP_LINEAR具有最高的精度。
	https://blog.csdn.net/yiting52/article/details/52401133
	*/
	// osg设置带有MipMap的滤波选择器模式，对于没有mipmap的纹理，进行自动生成mipmap
    tex->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR_MIPMAP_NEAREST);
    tex->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR_MIPMAP_NEAREST);
    tex->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);
    tex->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);
   

	tex->setImage(image);
	stateset->setTextureAttributeAndModes(0, tex,osg::StateAttribute::ON);
	geometry->setStateSet(stateset);
	geometry->setUseVertexBufferObjects(true);
 
	node->addDrawable(geometry);

	return node.release();
}

class keyBoardHandler :public osgGA::GUIEventHandler
{
public:
    keyBoardHandler(osg::Group* groundRoot):m_ground(groundRoot),m_dimessionX(2),m_dimessionY(2),m_bCoordinateAllMesh(false)
    {
    }

    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*) 
    {

        if(ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
        {
            if(ea.getKey() == osgGA::GUIEventAdapter::KEY_W)
            {
                osg::Geode* geode = m_ground->getChild(0)->asGeode();
                if(geode)
                {
                    osg::Geometry* geom = geode->getDrawable(0)->asGeometry();
                    if(geom)
                    {
                        osg::StateAttribute* attrib = geom->getOrCreateStateSet()->getAttribute(osg::StateAttribute::POLYGONMODE);
                        osg::PolygonMode* pm = dynamic_cast<osg::PolygonMode*>(attrib);
                        if(pm)
                        {
                            osg::PolygonMode::Mode mode =  pm->getMode(osg::PolygonMode::Face::FRONT_AND_BACK);
                            if(mode == osg::PolygonMode::Mode::LINE)
                            {
                                pm->setMode(osg::PolygonMode::Face::FRONT_AND_BACK, osg::PolygonMode::Mode::FILL);
                            }
                            else
                            {
                                pm->setMode(osg::PolygonMode::Face::FRONT_AND_BACK, osg::PolygonMode::Mode::LINE);
                            }
                        }
                    }
                }
                return true;
            }

            else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
            {
                m_dimessionY++;
                ChangeLines(m_dimessionX,m_dimessionY,m_bCoordinateAllMesh);
           
                return true;
            }
            else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
            {
                m_dimessionY--;
                ChangeLines(m_dimessionX,m_dimessionY,m_bCoordinateAllMesh);
          
                return true;

            }
            else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)
            {
                m_dimessionX++;
                ChangeLines(m_dimessionX,m_dimessionY,m_bCoordinateAllMesh);
           
                return true;
            }
            else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
            {
                m_dimessionX--;
                ChangeLines(m_dimessionX,m_dimessionY,m_bCoordinateAllMesh);
            }
            
            else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_A)
            {
                m_bCoordinateAllMesh = !m_bCoordinateAllMesh;
                ChangeLines(m_dimessionX,m_dimessionY,m_bCoordinateAllMesh);
            }
        }
        return false;
    }

    void ChangeLines(int x, int y,bool bCoordinateAllMesh = false)
    {
        if(x <2) x = 2;
        if(y < 2) y  = 2;
        m_ground->removeChild(0,1);
        m_ground->addChild(CreateGround(x,y,80,80,bCoordinateAllMesh));
    }
private:
    osg::ref_ptr<osg::Group> m_ground;
    int m_dimessionX;
    int m_dimessionY;
    bool m_bCoordinateAllMesh;
};

void main()
{
    osgViewer::Viewer viewer;
    osg::Group* root = new osg::Group;
    osg::Node* pNode = osgDB::readNodeFile("cow.osg");
   
    root->addChild(pNode);
    osg::Group* ground = new osg::Group;
    osg::Node* groundMesh = CreateGround(13,3);
    ground->addChild(groundMesh);
    root->addChild(ground);

    viewer.setSceneData(root);
    viewer.setUpViewInWindow(10,10,800,600);
    //viewer.addEventHandler(new osgGA::TrackballManipulator);
    viewer.addEventHandler(new keyBoardHandler(ground));
    viewer.run();
}

// referenced:
// https://blog.csdn.net/yulinxx/article/details/77896541
// https://blog.csdn.net/qq_36665171/article/details/81459338