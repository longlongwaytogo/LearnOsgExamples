#include <osgViewer/Viewer>
#include <osg/Geometry>
#include <osg/Material>
#include <osgDB/ReadFile>
#include <osg/Image>
#include <osg/Texture2D>
#include <osg/Texture1D>

void configureShaders(osg::StateSet* stateSet)
{
    const std::string vertexSource =
        "#version 330\n"
        "\n"
        "layout (location = 0) in vec4 Position;\n"
        "layout (location = 1) in vec4 color;\n"
        "layout (location = 2) in vec3 Normal;\n"
        "layout (location = 3) in vec2 Texcoord;\n"
        "uniform mat3 osg_NormalMatrix;\n"
        "out vec4 vcolor;\n"
        "out vec3 Norm;\n"
        "out vec2 texcoord;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = Position;\n"
        "    vcolor = color;\n"
        "    Norm = osg_NormalMatrix * Normal; \n"
        "    texcoord = Texcoord;  \n"
        "} \n";
    osg::Shader* vShader = new osg::Shader(osg::Shader::VERTEX, vertexSource);

    const std::string geomSource =
        "#version 330                                                                       \n"
        "layout(triangles) in;                                                                  \n"
        "layout(triangle_strip) out;                                                        \n"
        "layout(max_vertices = 9) out;                                                        \n"
        "uniform mat4 osg_ModelViewProjectionMatrix;                                     \n"
        /*<数组大小为3，，下标索引为0,1,2>*/
        "in vec4 vcolor[];\n"
        "in vec3 Norm[]; \n"
        "in  vec2 texcoord[];  \n "/*<数组大小为3，下标索引为0,1,2>*/
        "out vec2 tex; \n"
        "out vec4 fcolor;                                                                   \n"
        "out vec3 norm;   \n"
        "uniform sampler2D Tex;\n"
        "uniform sampler1D Tex1D;\n"
        "void main(void)                                                                    \n"
        "{                                                                                    \n"
        /*<绘制四边形>*/
        "    for (int i = 0; i < gl_in.length(); i++)                                        \n"
        "    {                                                                                \n"
        "        gl_Position = osg_ModelViewProjectionMatrix*gl_in[i].gl_Position;            \n"
        "       tex=texcoord[i];   \n"
        "       fcolor=vcolor[i];                                                       \n"/*<设置每个顶点颜色都不一样>*/
        "       norm=Norm[i]; \n"
        "        EmitVertex();                                                                \n"
        "    }                                                                                \n"
        "    EndPrimitive();                                                                    \n"
        "        gl_Position = osg_ModelViewProjectionMatrix*gl_in[0].gl_Position;            \n"
        "       tex=texcoord[0];   \n"
        "       fcolor=vcolor[0];                                                       \n"/*<设置每个顶点颜色都不一样>*/
        "       norm=Norm[0]; \n"
        "        EmitVertex();                                                                \n"
        "        gl_Position = osg_ModelViewProjectionMatrix*gl_in[2].gl_Position;            \n"
        "       tex=texcoord[2];   \n"
        "       fcolor=vcolor[2];                                                       \n"/*<设置每个顶点颜色都不一样>*/
        "       norm=Norm[2]; \n"
        "        EmitVertex();                                                                \n"
        "        gl_Position = osg_ModelViewProjectionMatrix*vec4(1.0,0.0,0.0,1.0);            \n"/*<TempVertex[0]不是原点>*/
        "       tex=texcoord[1];   \n"
        "       fcolor=vcolor[2];                                                       \n"/*<设置每个顶点颜色都不一样>*/
        "       norm=Norm[0]; \n"
        "        EmitVertex();                                                                \n"
        "    EndPrimitive();                                                                    \n"

        "} \n";

    osg::Shader* gShader = new osg::Shader(osg::Shader::GEOMETRY, geomSource);

    const std::string fragmentSource =
        "#version 330 \n"
        " \n"
        "layout (location = 0) out vec4 fragData; \n"
        " \n"
        "uniform sampler2D Texture;\n"
        "in vec4 fcolor;                           \n"
        "in vec3 norm;   \n"
        "in vec2 tex; \n"
        "void main() \n"
        "{ \n"
        "vec4 textureColor=texture(Texture,tex) * fcolor;\n"
        "  fragData = textureColor; \n"
        "} \n";
    osg::Shader* fShader = new osg::Shader(osg::Shader::FRAGMENT, fragmentSource);

    osg::Program* program = new osg::Program;

    program->addShader(vShader);
    program->addShader(gShader);
    program->addShader(fShader);

    stateSet->setAttribute(program);
}

void main()
{
    osg::ref_ptr<osg::Group> root = new osg::Group;
    /*<顶点坐标>*/
    osgViewer::Viewer viewer;
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
    vertices->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
    vertices->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
    /*<顶点颜色>*/
    osg::ref_ptr<osg::Vec4Array> V_C = new osg::Vec4Array;
    V_C->push_back(osg::Vec4(0.0, 1.0, 1.0, 1.0));
    V_C->push_back(osg::Vec4(0.0, 0.0, 1.0, 1.0));
    V_C->push_back(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    V_C->push_back(osg::Vec4(0.0, 1.0, 0.0, 1.0));
    /*<顶点法线>*/
    osg::ref_ptr<osg::Vec3Array> V_N = new osg::Vec3Array;
    V_N->push_back(osg::Vec3(0.0, 1.0, 0.0));
    /*<顶点纹理坐标>*/
    osg::ref_ptr<osg::Vec2Array> V_T = new osg::Vec2Array;
    V_T->push_back(osg::Vec2(0.0, 0.0));
    V_T->push_back(osg::Vec2(1.0, 0.0));
    V_T->push_back(osg::Vec2(1.0, 1.0));
    V_T->push_back(osg::Vec2(0.0, 1.0));

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
    /*<设置顶点>*/
    geom->setVertexArray(vertices);
    /*<顶点颜色>*/
    geom->setVertexAttribArray(1, V_C);
    geom->setVertexAttribBinding(1, osg::Geometry::BIND_PER_VERTEX);
    /*<顶点法线>*/
    geom->setVertexAttribArray(2, V_N);
    geom->setVertexAttribBinding(2, osg::Geometry::BIND_OVERALL);
    /*<顶点纹理坐标>*/
    geom->setTexCoordArray(0, V_T);

    /*<纹理载入>*/
    osg::ref_ptr<osg::Image> image1 = osgDB::readImageFile("Images/Brick-Norman-Brown.TGA");
    if (!image1)
    {
        std::cout << " cannot Open image !!" << std::endl;
    }

    osg::ref_ptr<osg::Texture2D> texture1 = new osg::Texture2D();
    texture1->setDataVariance(osg::Object::DYNAMIC);
    texture1->setImage(image1.get());
    geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture1.get(), osg::StateAttribute::ON);
    geom->getOrCreateStateSet()->addUniform(new osg::Uniform("Texture", 0));
    geom->getOrCreateStateSet()->addUniform(new osg::Uniform("Tex", 0));

    geom->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, 3));
    configureShaders(geom->getOrCreateStateSet());
    root->addChild(geom);

    /*<版本指定>*/
    const int width(800), height(450);
    const std::string version("3.1");
    osg::ref_ptr< osg::GraphicsContext::Traits > traits = new osg::GraphicsContext::Traits();
    traits->x = 50; traits->y = 30;
    traits->width = width; traits->height = height;
    traits->windowDecoration = true;
    traits->doubleBuffer = true;
    traits->glContextVersion = version;
    osg::ref_ptr< osg::GraphicsContext > gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    if (!gc.valid())
    {
        osg::notify(osg::FATAL) << "Unable to create OpenGL v" << version << " context." << std::endl;
        return;
    }
    // Create a Camera that uses the above OpenGL context.
    osg::Camera* cam = viewer.getCamera();
    cam->setGraphicsContext(gc.get());
    // Must set perspective projection for fovy and aspect.
    cam->setProjectionMatrix(osg::Matrix::perspective(30., (double)width / (double)height, 1., 100.));
    // Unlike OpenGL, OSG viewport does *not* default to window dimensions.
    cam->setViewport(new osg::Viewport(0, 0, width, height));
    // for non GL3/GL4 and non GLES2 platforms we need enable the osg_ uniforms that the shaders will use,
    // you don't need thse two lines on GL3/GL4 and GLES2 specific builds as these will be enable by default.
    gc->getState()->setUseModelViewAndProjectionUniforms(true);
    gc->getState()->setUseVertexAttributeAliasing(true);

    viewer.setSceneData(root.get());
    viewer.run();
    return;
}