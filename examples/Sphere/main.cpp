#include <osg/shape>
#include <osg/shapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/io_utils>
#include <osg/Program>
#include <osg/Shader>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIActionAdapter>
#include <osg/PolygonMode>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

class  MyEventHandler:public osgGA::GUIEventHandler
{

    /** Deprecated, Handle events, return true if handled, false otherwise. */
    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa) 
    {
       if(ea.getEventType() == osgGA::GUIEventAdapter::KEYUP)
       {

           if(ea.getKey() == osgGA::GUIEventAdapter::KEY_W)
           {

               static bool bSet = false;
                osg::ref_ptr<osg::PolygonMode> pm = new osg::PolygonMode();
               if(!bSet)
               {
                  
                   pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
                   bSet = true;
               }
               else
               {
                   pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::FILL);
                   bSet = false;
               
               }
               if(aa.asView())
               {
                   osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(aa.asView());
                   if(viewer)
                   {
                       viewer->getSceneData()->getOrCreateStateSet()->setAttribute(pm,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
                   }
               }
           }
       }
       return false;
    }
};


osg::Geode* CreateHDRSkyDome()
{
    const unsigned int c_numRings( 20 );
    const unsigned int c_numSections( 20 );
    const unsigned int c_numSkyDomeVertices( ( c_numRings + 1 ) * ( c_numSections + 1 ) );
    const unsigned int c_numSkyDomeTriangles( 2 * c_numRings * c_numSections );
    const unsigned int c_numSkyDomeIndices( c_numSkyDomeTriangles * 3 );

    //std::vector<  unsigned int > skyDomeIndices;
    //std::vector< SVF_P3F_C4B_T2F > skyDomeVertices;

    osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> texCoordArray = new osg::Vec2Array;
    osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
    osg::ref_ptr<osg::UIntArray> indicesArray = new osg::UIntArray;
    osg::ref_ptr<osg::DrawElementsUInt> skyDomeIndiceArray = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,0);
    osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array;


    // setup buffers with source data
    //skyDomeVertices.reserve( c_numSkyDomeVertices );
    //skyDomeIndices.reserve( c_numSkyDomeIndices );

    // calculate vertices
    float sectionSlice( osg::DegreesToRadians( 360.0f / (float) c_numSections ) );
    float ringSlice( osg::DegreesToRadians( 180.0f / (float) c_numRings ) );
    std::fstream fs("osgst.txt",std::ios::out);
    for( unsigned int a = 0 ; a <= c_numRings; ++a )
    {
        float w( sinf( a * ringSlice ) );
        float z( cosf( a * ringSlice ) );

        for( unsigned int i = 0; i <= c_numSections; ++i )
        {
            //SVF_P3F_C4B_T2F v;

            float ii( i - a * 0.5f ); // Gives better tessellation, requires texture address mode to be "wrap" 
            // for u when rendering (see v.st[ 0 ] below). Otherwise set ii = i;		
            vertexArray->push_back(osg::Vec3(cosf( ii * sectionSlice ) * w, sinf( ii * sectionSlice ) * w, z));
            normalArray->push_back(osg::Vec3(cosf( ii * sectionSlice ) * w, sinf( ii * sectionSlice ) * w, z));
            //assert( fabs( v.xyz.GetLengthSquared() - 1.0 ) < 1e-2/*1e-4*/ );		// because of FP-16 precision
           // 此算法是从Dx例子中获取，纹理y轴需要1-y ，此处2.0的目的是将纹理进行压缩，只映射球体的上半部分，下半部分使用边缘纹理填充
            texCoordArray->push_back(osg::Vec2(i / (float) c_numSections, 2.0* (float) a / (float) c_numRings));
            if(fs.is_open())
                fs<< osg::Vec3(cosf( ii * sectionSlice ) * w, sinf( ii * sectionSlice ) * w, z) <<" = " << osg::Vec2(ii / (float) c_numSections, 1.0 - (float) a / (float) c_numRings) << std::endl;
        }
    }
    fs.close();

    // build faces
    for( unsigned int a = 0 ; a < c_numRings; ++a )
    {
        for( unsigned int i( 0 ); i < c_numSections; ++i )
        {
            skyDomeIndiceArray->push_back( (unsigned int) ( a * ( c_numSections + 1 ) + i + 1 ) );
            skyDomeIndiceArray->push_back( (unsigned int) ( a * ( c_numSections + 1 ) + i ) );
            skyDomeIndiceArray->push_back( (unsigned int) ( ( a + 1 ) * ( c_numSections + 1 ) + i + 1 ) );

            skyDomeIndiceArray->push_back( (unsigned int) ( ( a + 1 ) * ( c_numSections + 1 ) + i ) );
            skyDomeIndiceArray->push_back( (unsigned int) ( ( a + 1 ) * ( c_numSections + 1 ) + i + 1 ) );
            skyDomeIndiceArray->push_back( (unsigned int) ( a * ( c_numSections + 1 ) + i ) );
        }
    }

    // sanity checks
  /*  assert( vertexArray->size() == c_numSkyDomeVertices );
    assert( skyDomeIndiceArray->size() == c_numSkyDomeIndices );*/




    //m_needRenderParamUpdate = 1;

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    osg::ref_ptr<osg::Geometry> geo = new osg::Geometry;




    geo->setVertexArray(vertexArray);
    geo->setTexCoordArray(0,texCoordArray, osg::Array::Binding::BIND_PER_VERTEX);
    geo->setNormalArray(normalArray,osg::Array::Binding::BIND_PER_VERTEX);

    // osg test normal
    /*{
        osg::ref_ptr<osg::Geometry> normalGeo = new osg::Geometry;
        osg::ref_ptr<osg::Vec3Array> normalLines = new osg::Vec3Array;

        for(int i = 0; i < vertexArray->size(); ++i)
        {
            normalLines->push_back(osg::Vec3(0,0,0));
            normalLines->push_back(vertexArray->at(i)*1.5);
        }
        normalGeo->setVertexArray(normalLines);
        normalGeo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,normalLines->size()));
        geode->addDrawable(normalGeo.get());
    }*/


   // colorArray->push_back(osg::Vec4(0,1,0,1.0));
    //geo->setColorArray(colorArray,osg::Array::Binding::BIND_OVERALL);
    ////设置颜色
    //geo->setColorArray(color.get());
    //geo->setColorBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);//设置颜色绑定

    geo->addPrimitiveSet(skyDomeIndiceArray);
    {

        osg::ref_ptr<osg::Texture2D> qjTexture = new osg::Texture2D;
        qjTexture->setDataVariance(osg::Object::DYNAMIC);
        //  osg::ref_ptr<osg::Image> qjImage = osgDB::readImageFile("F:\\Project\\2018\\LearnOsgExamples\\examples\\Sphere\\room.jpg");
        osg::ref_ptr<osg::Image> qjImage = osgDB::readImageFile("Images/land_shallow_topo_2048.jpg");
        qjTexture->setImage(qjImage.get());
        qjTexture->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
        qjTexture->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);

        osg::ref_ptr<osg::TexEnv> sphereOneTexEnv = new osg::TexEnv;
        // sphereOneTexEnv->setMode(osg::TexEnv::DECAL); // decal表示不接受光照，直接吧纹理贴到表面
        osg::ref_ptr<osg::StateSet> ss = new osg::StateSet;
        ss->setTextureAttribute(0,sphereOneTexEnv.get());
        ss->setTextureAttributeAndModes(0,qjTexture.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
        {

            std::string vsSrc = 
                "void main() { \n"
                "gl_Position = ftransform();\n"
                "gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;\n"
                "}\n";
            std::string fsSrc = 
                "uniform sampler2D tex;"
                "void main(){\n"
               // "gl_FragColor = texture2D(tex,gl_TexCoord[0].xy);\n"
              // "gl_FragColor = vec4(1.0,0,0,1.0);\n"
              " gl_FragColor = vec4(gl_TexCoord[0].xy,0.0,1.0);\n"
                "}\n";

            osg::ref_ptr<osg::Program> program = new osg::Program;
            osg::ref_ptr<osg::Shader> vsShader = new osg::Shader(osg::Shader::VERTEX,vsSrc.c_str());
            osg::ref_ptr<osg::Shader> fsShader = new osg::Shader(osg::Shader::FRAGMENT,fsSrc.c_str());
            program->addShader(vsShader);
            program->addShader(fsShader);
            ss->setAttribute(program);

        }
        geode->setStateSet(ss);
    }

    geode->addDrawable(geo.get());
    {

        //geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere()));
    }

    return geode.release();

}

#include <vector>
#include <fstream>

void testVector();

void main()
{  
   // testVector();
    osgViewer::Viewer viewer;
    osg::Group* root = new osg::Group;
    
    osg::Geode* geode = new osg::Geode;
    root->addChild(geode);
    geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere()));

    {

        //给球体贴图
        osg::ref_ptr<osg::Texture2D> qjTexture = new osg::Texture2D;
        qjTexture->setDataVariance(osg::Object::DYNAMIC);
      //  osg::ref_ptr<osg::Image> qjImage = osgDB::readImageFile("F:\\Project\\2018\\LearnOsgExamples\\examples\\Sphere\\room.jpg");
        osg::ref_ptr<osg::Image> qjImage = osgDB::readImageFile("Images/land_shallow_topo_2048.jpg");
        qjTexture->setImage(qjImage.get());
        qjTexture->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
        qjTexture->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);

        osg::ref_ptr<osg::TexEnv> sphereOneTexEnv = new osg::TexEnv;
       // sphereOneTexEnv->setMode(osg::TexEnv::DECAL); // decal表示不接受光照，直接吧纹理贴到表面
        osg::ref_ptr<osg::StateSet> ss = new osg::StateSet;
        ss->setTextureAttribute(0,sphereOneTexEnv.get());
        ss->setTextureAttributeAndModes(0,qjTexture.get());
        geode->setStateSet(ss);

   
    }
    {

        osg::MatrixTransform* mt = new osg::MatrixTransform;
        mt->setMatrix(osg::Matrix::translate(osg::Vec3(5,0,0)));
        mt->addChild(CreateHDRSkyDome());
        root->addChild(mt);

    }
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.addEventHandler(new MyEventHandler);
    viewer.setSceneData(root);
    viewer.setUpViewInWindow(10,10,800,600);
    viewer.realize();
    viewer.run();
   
}
void testVector()// test vector read write
{
    {

        std::fstream f("d:\\try.dat", std::ios::out|std::ios::binary);//供写使用，文件不存在则创建，存在则清空原内容
        f << 1234 <<' '  << 3.14 << 'A' << "How are you"; //写入数据
        f.close();//关闭文件以使其重新变为可访问，函数一旦调用，原先的流对象就可以被用来打开其它的文件
        f.open("d:\\try.dat", std::ios::in|std::ios::binary);//打开文件，供读
        int i;
        double d;
        char c;
        char s[20];
        f >> i >> d >> c;               //读取数据
        f.getline(s, 20);
        std::cout << i << std::endl;             //显示各数据
        std::cout << d <<  std::endl;
        std::cout << c <<  std::endl;
        std::cout << s <<  std::endl;
        f.close();
    }

    {
        std::vector<int> vals;
        for(int i = 0; i < 100; ++i)
            vals.push_back(i);

        std::fstream fs("info.dat",std::ios::out|std::ios::binary);
        if(fs.is_open())
        {
             //fs << 1234 <<' '  << 3.14 << 'A' << "How are you"; //写入数据
            int len = vals.size();
            fs << len;
           /* char buf[401];
            memset(buf,0,401);
            memcpy(buf,&(vals[0]),sizeof(int)*vals.size());*/
            fs.write((const char*)&(vals[0]),vals.size()* sizeof(int));
           // fs.write((const char*)buf,vals.size()* sizeof(int));
            fs.close();
            
        }

        std::fstream ifs("info.dat",std::ios::in|std::ios::binary);
        if(ifs.is_open())
        {
  

            int length  = 100;
           ifs >> length;
            if(length>0)
            {
                std::vector<int> rvals(length,0);
               // char buffer[401];
               // memset(buffer,0,401);
                //ifs.read(buffer,400);
                 ifs.read((char*)&(rvals[0]),length * sizeof(int));
               /* for(int i = 0; i< 100; ++i)
                {
                    memcpy(&(rvals[i]),(void*)(buffer +4*i),sizeof(int));
                }*/
               // fs.read((char*)&(rvals[0]),length * sizeof(int));
                for(int i = 0; i < rvals.size(); ++i)
                    std::cout << rvals[i] <<std::endl;
            }
            ifs.close();
        }
        
    }
}