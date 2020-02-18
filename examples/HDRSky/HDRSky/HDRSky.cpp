#include "HDRSky.h"
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osg/Program>
#include <osg/Shader>
#include <osgDB/ReadFile>
#include <osg/Image>

HDRSky::HDRSky()
//: m_pRenderParams(0)
/*, m_skyDomeTextureLastTimeStamp(-1)
, m_frameReset(0)
, m_pStars(0)
, m_pSkyDomeTextureMie(0)
, m_pSkyDomeTextureRayleigh(0)*/
{
/*	mfSetType(eDATA_HDRSky);
	mfUpdateFlags(FCEF_TRANSFORM);*/
	
    Init();
   
}

HDRSky::~HDRSky()
{

}

 osg::Geode* HDRSky::CreateHDRSkyDome()
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
            texCoordArray->push_back(osg::Vec2(ii / (float) c_numSections, 2.0f * (float) a / (float) c_numRings));
			
		}
	}

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
    assert( vertexArray->size() == c_numSkyDomeVertices );
    assert( skyDomeIndiceArray->size() == c_numSkyDomeIndices );

 


	//m_needRenderParamUpdate = 1;

        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	    osg::ref_ptr<osg::Geometry> geo = new osg::Geometry;

      
      

        geo->setVertexArray(vertexArray);
        geo->setTexCoordArray(0,texCoordArray, osg::Array::Binding::BIND_PER_VERTEX);
        geo->setNormalArray(normalArray,osg::Array::Binding::BIND_PER_VERTEX);
       
          // osg test normal
        {
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
         }
       

        colorArray->push_back(osg::Vec4(0,1,0,1.0));
        geo->setColorArray(colorArray,osg::Array::Binding::BIND_OVERALL);
	    ////设置颜色
	    //geo->setColorArray(color.get());
	    //geo->setColorBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);//设置颜色绑定

	    geo->addPrimitiveSet(skyDomeIndiceArray);
        
        //geode->addDrawable(geo.get());
        {
           
            geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere()));
        }

        {
                    
             osg::ref_ptr<osg::StateSet> ss = new osg::StateSet;
             std::string path = "F:\\Project\\2018\\LearnOsgExamples\\examples\\HDRSky\\HDRSky\\";
             std::string texMie = path + "SkyDomeMie.HDR";
             std::string texRayleigh = path + "SkyDomeRayleigh.HDR";
             std::string texMoom = path + "Moon.dds";

             osg::ref_ptr<osg::Texture2D> tex0 = new osg::Texture2D;
             osg::ref_ptr<osg::Texture2D> tex1 = new osg::Texture2D;
             osg::ref_ptr<osg::Texture2D> tex2 = new osg::Texture2D;
             osg::Image* img0 = osgDB::readImageFile(texMie);
             osg::Image* img1 = osgDB::readImageFile(texRayleigh);
             osg::Image* img2 = osgDB::readImageFile(texMoom);

             tex0->setImage(img0);
             tex1->setImage(img1);
             tex2->setImage(img2);
         
            ss->setTextureAttributeAndModes(0,tex0.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
            ss->setTextureAttributeAndModes(1,tex1.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
            ss->setTextureAttributeAndModes(2,tex2.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
            {
                osg::ref_ptr<osg::Program> program = new osg::Program;
                std::string vsFile = path +"HDRSky.vert";
                std::string fsFile = path + "HDRSky.frag";
                osg::ref_ptr<osg::Shader> vsShader = osgDB::readRefShaderFile(osg::Shader::VERTEX,vsFile);
                osg::ref_ptr<osg::Shader> fsShader = osgDB::readRefShaderFile(osg::Shader::FRAGMENT,fsFile);
           
                program->addShader(vsShader);
                program->addShader(fsShader);
                ss->setAttribute(program);

            }
            geode->setStateSet(ss);
        }
        return geode.release();

    }

 osg::ref_ptr<osg::Node> createQuad()
 
{
     osg::ref_ptr<osg::Geode> geode = new osg::Geode;
     osg::ref_ptr<osg::Geometry> geom=new osg::Geometry;
    
     //首先定义四个点
     osg::ref_ptr<osg::Vec3Array> v=new osg::Vec3Array;
     geom->setVertexArray(v.get());
     v->push_back(osg::Vec3(-100.f,0.f,-100.f));
     v->push_back(osg::Vec3(100.f,0.f,-100.f));
     v->push_back(osg::Vec3(100.f,0.f,100.f));
     v->push_back(osg::Vec3(-100.f,0.f,100.f));

     //定义颜色数组
     osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array;
     geom->setColorArray(c.get());
     geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
     c->push_back(osg::Vec4(1.f,0.f,0.f,1.f));
     c->push_back(osg::Vec4(0.f,1.f,0.f,1.f));
     c->push_back(osg::Vec4(0.f,0.f,1.f,1.f));
     c->push_back(osg::Vec4(1.f,1.f,1.f,1.f));
     //设置定点关联方式
     geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
     geode->addDrawable(geom);
     return geode;

 }

#include <osg/MatrixTransform>
 void HDRSky::Init()
 {
     /*osg::Geode* geode = CreateHDRSkyDome();
     addChild(geode);*/
    // addChild(createQuad());
     osg::MatrixTransform* mt = new osg::MatrixTransform;
     mt->setMatrix(osg::Matrix::scale(100,100,100));
     mt->addChild(CreateHDRSkyDome());
     addChild(mt);

 }