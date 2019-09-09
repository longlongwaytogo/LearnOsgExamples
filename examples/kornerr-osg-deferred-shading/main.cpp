
#include "main.h"

#include <Utils.h>
#include <osg/AnimationPath>
#include <osg/PolygonMode>
#include <osgDB/ReadFile>
//#define USE_EFFECT_COMPOSITOR

#define NO_DEFERRED_SHADING 1
#ifdef USE_EFFECT_COMPOSITOR
    #include <osgFX/EffectCompositor>
#endif
#include <osgShadow/SoftShadowMap>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/TangentSpaceGenerator>

osg::TextureRectangle *createFloatTextureRectangle(int textureSize)
{
    osg::ref_ptr<osg::TextureRectangle> tex2D = new osg::TextureRectangle;
    tex2D->setTextureSize(textureSize, textureSize);
    tex2D->setInternalFormat(GL_RGBA16F_ARB);
    tex2D->setSourceFormat(GL_RGBA);
    tex2D->setSourceType(GL_FLOAT);
    return tex2D.release();
}

osg::Camera *createHUDCamera(double left,
                             double right,
                             double bottom,
                             double top)
{
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    camera->setRenderOrder(osg::Camera::POST_RENDER);
    camera->setAllowEventFocus(false);
    camera->setProjectionMatrix(osg::Matrix::ortho2D(left, right, bottom, top));
    camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    return camera.release();
}

osg::ref_ptr<osg::LightSource> createLight(const osg::Vec3 &pos)
{
    osg::ref_ptr<osg::LightSource> light = new osg::LightSource;
    light->getLight()->setPosition(osg::Vec4(pos.x(), pos.y(), pos.z(), 1));
    light->getLight()->setAmbient(osg::Vec4(0.2, 0.2, 0.2, 1));
    light->getLight()->setDiffuse(osg::Vec4(0.8, 0.8, 0.8, 1));
    return light;
}
class CreateTangentSpace : public osg::NodeVisitor
{
public:
    CreateTangentSpace() : NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN), tsg(new osgUtil::TangentSpaceGenerator) {}
    virtual void apply(osg::Geode& geode)
    {
        for (unsigned int i = 0; i < geode.getNumDrawables(); ++i)
        {
            osg::Geometry *geo = dynamic_cast<osg::Geometry *>(geode.getDrawable(i));
            if (geo != NULL)
            {
                // assume the texture coordinate for normal maps is stored in unit #0
                tsg->generate(geo, 0);
                // pass2.vert expects the tangent array to be stored inside gl_MultiTexCoord1
                geo->setTexCoordArray(1, tsg->getTangentArray());
            }
        }
        traverse(geode);
    }
private:
    osg::ref_ptr<osgUtil::TangentSpaceGenerator> tsg;
};

Pipeline createPipelineEffectCompositor(
        osg::ref_ptr<osg::Group> scene,
        osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene,
        const osg::Vec3 lightPos)
{
    Pipeline p;
#ifdef USE_EFFECT_COMPOSITOR
    p.graph = new osg::Group;
    // Pass 1 (shadow).
    osgFX::EffectCompositor *pass1 = osgFX::readEffectFile("pass1.xml");
    p.pass1Shadows = pass1->getTexture("pass1Shadows");
    // Take texture size from the effect compositor.
    // Width must be equal to height.
    p.textureSize = p.pass1Shadows->getTextureWidth();
    pass1->addChild(shadowedScene.get());
    // Passes 2 (positions, normals, colors) and 3 (final combination).
    osgFX::EffectCompositor *passes2_3 = osgFX::readEffectFile("passes2_3.xml");
    passes2_3->addChild(scene.get());
    osgFX::EffectCompositor::PassData pass3;
    passes2_3->getPassData("pass3", pass3);
    // Pass 1st pass texture to the 3rd pass.
    osg::StateSet *ss = pass3.pass->getOrCreateStateSet();
    ss->setTextureAttributeAndModes(3, p.pass1Shadows);
    ss->addUniform(new osg::Uniform("shadowMap", 3));
    // Light position.
    ss->addUniform(new osg::Uniform("lightPos", lightPos));
    // Graph.
    p.graph->addChild(pass1);
    p.graph->addChild(passes2_3);
    // Textures.
    p.pass2Colors    = passes2_3->getTexture("pass2Colors");
    p.pass2Normals   = passes2_3->getTexture("pass2Normals");
    p.pass2Positions = passes2_3->getTexture("pass2Positions");
    p.pass3Final     = passes2_3->getTexture("pass3Final");
#endif // USE_EFFECT_COMPOSITOR
    return p;
}

Pipeline createPipelinePlainOSG(
        osg::ref_ptr<osg::Group> scene,
        osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene,
        const osg::Vec3 lightPos)
{
    Pipeline p;
    p.graph = new osg::Group;
    p.textureSize = 1024;
    // Pass 1 (shadow).
    p.pass1Shadows = createFloatTextureRectangle(p.textureSize);
    osg::ref_ptr<osg::Camera> pass1 =
        createRTTCamera(osg::Camera::COLOR_BUFFER, p.pass1Shadows);
    pass1->addChild(shadowedScene.get());
    CreateTangentSpace cts;
    scene->accept(cts);
    // Pass 2 (positions, normals, colors).
    p.pass2Positions = createFloatTextureRectangle(p.textureSize);
    p.pass2Normals   = createFloatTextureRectangle(p.textureSize);
    p.pass2Colors    = createFloatTextureRectangle(p.textureSize);
    osg::ref_ptr<osg::Camera> pass2 =
        createRTTCamera(osg::Camera::COLOR_BUFFER0, p.pass2Positions);
    pass2->attach(osg::Camera::COLOR_BUFFER1, p.pass2Normals);
    pass2->attach(osg::Camera::COLOR_BUFFER2, p.pass2Colors);
    pass2->addChild(scene.get());
   
    std::string mediaPath = Utils::Helper::Ins().getMediaPath();
    std::string pass2_vert_path = mediaPath + "pass2.vert";
    std::string pass2_frag_path = mediaPath + "pass2.frag";

    osg::StateSet *ss = setShaderProgram(pass2,pass2_vert_path, pass2_frag_path);
    std::string rockwall_path = mediaPath + "rockwall.png";
    std::string rockwall_NH_path = mediaPath + "rockwall_NH.png";
    ss->setTextureAttributeAndModes(0, createTexture(rockwall_path));
    ss->setTextureAttributeAndModes(1, createTexture(rockwall_NH_path));
   
    ss->addUniform(new osg::Uniform("diffMap", 0));
    ss->addUniform(new osg::Uniform("bumpMap", 1));
    // Pass 3 (final).
    p.pass3Final = createFloatTextureRectangle(p.textureSize);
    osg::ref_ptr<osg::Camera> pass3 =
        createRTTCamera(osg::Camera::COLOR_BUFFER, p.pass3Final, true);
    std::string pass3_vert_path = mediaPath + "pass3.vert";
    std::string pass3_frag_path = mediaPath + "pass3.frag";
    ss = setShaderProgram(pass3, pass3_vert_path, pass3_frag_path);
    ss->setTextureAttributeAndModes(0, p.pass2Positions);
    ss->setTextureAttributeAndModes(1, p.pass2Normals);
    ss->setTextureAttributeAndModes(2, p.pass2Colors);
    ss->setTextureAttributeAndModes(3, p.pass1Shadows);
    ss->addUniform(new osg::Uniform("posMap",    0));
    ss->addUniform(new osg::Uniform("normalMap", 1));
    ss->addUniform(new osg::Uniform("colorMap",  2));
    ss->addUniform(new osg::Uniform("shadowMap", 3));
    // Light position.
    ss->addUniform(new osg::Uniform("lightPos", lightPos));
    // Graph.
    p.graph->addChild(pass1);
    p.graph->addChild(pass2);
    p.graph->addChild(pass3);
    return p;
}

osg::Camera *createRTTCamera(osg::Camera::BufferComponent buffer,
                             osg::Texture *tex,
                             bool isAbsolute)
{
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setClearColor(osg::Vec4());
    camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    camera->setRenderOrder(osg::Camera::PRE_RENDER);
    if (tex)
    {
        tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
        tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
        camera->setViewport(0, 0, tex->getTextureWidth(), tex->getTextureHeight());
        camera->attach(buffer, tex);
    }
    if (isAbsolute)
    {
        camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        camera->setProjectionMatrix(osg::Matrix::ortho2D(0.0, 1.0, 0.0, 1.0));
        camera->setViewMatrix(osg::Matrix::identity());
        camera->addChild(createScreenQuad(1.0f, 1.0f));
    }
    return camera.release();
}


static char  room_vertex[] = 
{
    
 
"void main() \n"
"{ \n"
    "gl_Position = ftransform(); \n"
    " gl_TexCoord[0] = gl_MultiTexCoord0; \n"
" } \n"
};

static char room_frag[] = 
{
    "varying vec2 coord; \n"
    "uniform sampler2D diffMap; \n"
    "uniform sampler2D bumpMap; \n"

    "void main() { \n"
   " gl_FragColor = texture2D(diffMap,gl_TexCoord[0]); \n"
 // "gl_FragColor = vec4(1,0,0,1.0); \n"
    " } \n"
};
osg::ref_ptr<osg::Group> createSceneRoom()
{
    // Room.
    osg::ref_ptr<osg::MatrixTransform> room = new osg::MatrixTransform;
    std::string simpleroomPath = Utils::Helper::Ins().getMediaPath() + "osg_deferred_shading/simpleroom.osgt";
    osg::ref_ptr<osg::Node> roomModel = osgDB::readNodeFile(simpleroomPath);
    room->addChild(roomModel);
    room->setMatrix(osg::Matrix::translate(0, 0, 1));
    // Small room.
    osg::ref_ptr<osg::MatrixTransform> smallRoom = new osg::MatrixTransform;
    smallRoom->addChild(roomModel);
    smallRoom->setMatrix(osg::Matrix::translate(70, 0, -30));
    // Torus.
    osg::ref_ptr<osg::MatrixTransform> torus = new osg::MatrixTransform;
    std::string torusPath = Utils::Helper::Ins().getMediaPath() + "osg_deferred_shading/torus.osgt";
    osg::ref_ptr<osg::Node> torusModel = osgDB::readRefNodeFile(torusPath);
    torus->addChild(torusModel);
    setAnimationPath(torus, osg::Vec3(0, 0, 15), 6, 16);
    // Torus2.
    osg::ref_ptr<osg::MatrixTransform> torus2 = new osg::MatrixTransform;
    torus2->addChild(torusModel);
    setAnimationPath(torus2, osg::Vec3(-20, 0, 10), 20, 0);
    // Torus3.
    osg::ref_ptr<osg::MatrixTransform> torus3 = new osg::MatrixTransform;
    torus3->addChild(torusModel);
    setAnimationPath(torus3, osg::Vec3(0, 0, 40), 3, 25);
    // Scene.
    osg::ref_ptr<osg::Group> scene = new osg::Group;
    scene->addChild(room);
    scene->addChild(smallRoom);
    scene->addChild(torus);
    scene->addChild(torus2);
    scene->addChild(torus3);
    // Alter room's bump'ness.
    osg::StateSet *ss = smallRoom->getOrCreateStateSet();
    std::string wallPath = Utils::Helper::Ins().getMediaPath() +"osg_deferred_shading/wall.png";
    std::string wallBumpPath = Utils::Helper::Ins().getMediaPath() + "osg_deferred_shading/wall_bump.png";
    ss->setTextureAttributeAndModes(0, createTexture(wallPath));
    ss->setTextureAttributeAndModes(1, createTexture(wallBumpPath));
    ss->addUniform(new osg::Uniform("diffMap", 0));
    ss->addUniform(new osg::Uniform("bumpMap", 1));
   

    osg::StateSet* ss1 = room->getOrCreateStateSet();
    ss1->setTextureAttributeAndModes(0, createTexture(wallPath));
    ss1->setTextureAttributeAndModes(1, createTexture(wallBumpPath));
    ss1->addUniform(new osg::Uniform("diffMap", 0));
    ss1->addUniform(new osg::Uniform("bumpMap", 1));
    osg::ref_ptr<osg::Program> program = new osg::Program;
    program->addShader(new osg::Shader(osg::Shader::VERTEX, room_vertex));
    program->addShader(new osg::Shader(osg::Shader::FRAGMENT, room_frag));
    ss1->setAttributeAndModes(program.get());

    //ss->addUniform(new osg::Uniform("useBumpMap", 0));
    return scene;
}

osg::Geode *createScreenQuad(float width,
                             float height,
                             float scale,
                             osg::Vec3 corner)
{
    osg::Geometry* geom = osg::createTexturedQuadGeometry(
        corner,
        osg::Vec3(width, 0, 0),
        osg::Vec3(0, height, 0),
        0,
        0,
        scale,
        scale);
    osg::ref_ptr<osg::Geode> quad = new osg::Geode;
    quad->addDrawable(geom);
    int values = osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED;
    quad->getOrCreateStateSet()->setAttribute(
        new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,
                             osg::PolygonMode::FILL),
        values);
    quad->getOrCreateStateSet()->setMode(GL_LIGHTING, values);
    return quad.release();
}

osg::Texture2D *createTexture(const std::string &fileName)
{
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage(osgDB::readImageFile(fileName));
    texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
    texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    texture->setMaxAnisotropy(16.0f);
    return texture.release();
}

osg::ref_ptr<osg::Camera> createTextureDisplayQuad(
    const osg::Vec3 &pos,
    osg::StateAttribute *tex,
    float scale,
    float width,
    float height)
{
    osg::ref_ptr<osg::Camera> hc = createHUDCamera();
    hc->addChild(createScreenQuad(width, height, scale, pos));
    hc->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
    return hc;
}

void setAnimationPath(osg::ref_ptr<osg::MatrixTransform> node,
                      const osg::Vec3 &center,
                      float time,
                      float radius)
{
    // Create animation.
    osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
    path->setLoopMode(osg::AnimationPath::LOOP);
    unsigned int numSamples = 32;
    float delta_yaw = 2.0f * osg::PI / (static_cast<float>(numSamples) - 1.0f);
    float delta_time = time / static_cast<float>(numSamples);
    for (unsigned int i = 0; i < numSamples; ++i)
    {
        float yaw = delta_yaw * static_cast<float>(i);
        osg::Vec3 pos(center.x() + sinf(yaw)*radius,
                      center.y() + cosf(yaw)*radius,
                      center.z());
        osg::Quat rot(-yaw, osg::Z_AXIS);
        path->insert(delta_time * static_cast<float>(i),
                     osg::AnimationPath::ControlPoint(pos, rot));
    }
    // Assign it.
    node->setUpdateCallback(new osg::AnimationPathCallback(path));
}

osg::ref_ptr<osg::StateSet> setShaderProgram(osg::ref_ptr<osg::Camera> pass,
                                             std::string vert,
                                             std::string frag)
{
    osg::ref_ptr<osg::Program> program = new osg::Program;
    program->addShader(osgDB::readShaderFile(vert));
    program->addShader(osgDB::readShaderFile(frag));
    osg::ref_ptr<osg::StateSet> ss = pass->getOrCreateStateSet();
    ss->setAttributeAndModes(
        program.get(),
        osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    return ss;
}

static const char fragmentSoftShaderSource_noBaseTexture[] =
    "#define SAMPLECOUNT 64 \n"
    "#define SAMPLECOUNT_FLOAT 64.0 \n"
    "#define SAMPLECOUNT_D2 32 \n"
    "#define SAMPLECOUNT_D2_FLOAT 32.0 \n"
    "#define INV_SAMPLECOUNT (1.0 / SAMPLECOUNT_FLOAT) \n"

    "uniform sampler2DShadow osgShadow_shadowTexture; \n"
    "uniform sampler3D osgShadow_jitterTexture; \n"

    "uniform vec2 osgShadow_ambientBias; \n"
    "uniform float osgShadow_softnessWidth; \n"
    "uniform float osgShadow_jitteringScale; \n"

    "void main(void) \n"
    "{ \n"
    "  vec4 sceneShadowProj  = gl_TexCoord[1]; \n"
    "  float softFactor = osgShadow_softnessWidth * sceneShadowProj.w; \n"
    "  vec4 smCoord  = sceneShadowProj; \n"
    "  vec3 jitterCoord = vec3( gl_FragCoord.xy / osgShadow_jitteringScale, 0.0 ); \n"
    "  float shadow = 0.0; \n"
    // First "cheap" sample test
    "  const float pass_div = 1.0 / (2.0 * 4.0); \n"
    "  for ( int i = 0; i < 4; ++i ) \n"
    "  { \n"
    // Get jitter values in [0,1]; adjust to have values in [-1,1]
    "    vec4 offset = 2.0 * texture3D( osgShadow_jitterTexture, jitterCoord ) -1.0; \n"
    "    jitterCoord.z += 1.0 / SAMPLECOUNT_D2_FLOAT; \n"

    "    smCoord.xy = sceneShadowProj.xy  + (offset.xy) * softFactor; \n"
    "    shadow +=  shadow2DProj( osgShadow_shadowTexture, smCoord ).x * pass_div; \n"

    "    smCoord.xy = sceneShadowProj.xy  + (offset.zw) * softFactor; \n"
    "    shadow +=  shadow2DProj( osgShadow_shadowTexture, smCoord ).x *pass_div; \n"
    "  } \n"
    // skip all the expensive shadow sampling if not needed
    "  if ( shadow * (shadow -1.0) != 0.0 ) \n"
    "  { \n"
    "    shadow *= pass_div; \n"
    "    for (int i=0; i<SAMPLECOUNT_D2 - 4; ++i){ \n"
    "      vec4 offset = 2.0 * texture3D( osgShadow_jitterTexture, jitterCoord ) - 1.0; \n"
    "      jitterCoord.z += 1.0 / SAMPLECOUNT_D2_FLOAT; \n"

    "      smCoord.xy = sceneShadowProj.xy  + offset.xy * softFactor; \n"
    "      shadow +=  shadow2DProj( osgShadow_shadowTexture, smCoord ).x * INV_SAMPLECOUNT; \n"

    "      smCoord.xy = sceneShadowProj.xy  + offset.zw * softFactor; \n"
    "      shadow +=  shadow2DProj( osgShadow_shadowTexture, smCoord ).x * INV_SAMPLECOUNT; \n"
    "    } \n"
    "  } \n"
    // apply shadow, modulo the ambient bias
    "  gl_FragColor = gl_Color * (osgShadow_ambientBias.x + shadow * osgShadow_ambientBias.y); \n"
    "} \n";

//////////////////////////////////////////////////////////////////
// fragment shader
//
static const char fragmentSoftShaderSource_withBaseTexture[] =
    "#define SAMPLECOUNT 64 \n"
    "#define SAMPLECOUNT_FLOAT 64.0 \n"
    "#define SAMPLECOUNT_D2 32 \n"
    "#define SAMPLECOUNT_D2_FLOAT 32.0 \n"
    "#define INV_SAMPLECOUNT (1.0 / SAMPLECOUNT_FLOAT) \n"

    "uniform sampler2D osgShadow_baseTexture; \n"
    "uniform sampler2DShadow osgShadow_shadowTexture; \n"
    "uniform sampler3D osgShadow_jitterTexture; \n"

    "uniform vec2 osgShadow_ambientBias; \n"
    "uniform float osgShadow_softnessWidth; \n"
    "uniform float osgShadow_jitteringScale; \n"

    "void main(void) \n"
    "{ \n"
    "  vec4 sceneShadowProj  = gl_TexCoord[1]; \n"
    "  float softFactor = osgShadow_softnessWidth * sceneShadowProj.w; \n"
    "  vec4 smCoord  = sceneShadowProj; \n"
    "  vec3 jitterCoord = vec3( gl_FragCoord.xy / osgShadow_jitteringScale, 0.0 ); \n"
    "  float shadow = 0.0; \n"
    // First "cheap" sample test
    "  const float pass_div = 1.0 / (2.0 * 4.0); \n"
    "  for ( int i = 0; i < 4; ++i ) \n"
    "  { \n"
    // Get jitter values in [0,1]; adjust to have values in [-1,1]
    "    vec4 offset = 2.0 * texture3D( osgShadow_jitterTexture, jitterCoord ) -1.0; \n"
    "    jitterCoord.z += 1.0 / SAMPLECOUNT_D2_FLOAT; \n"

    "    smCoord.xy = sceneShadowProj.xy  + (offset.xy) * softFactor; \n"
    "    shadow +=  shadow2DProj( osgShadow_shadowTexture, smCoord ).x * pass_div; \n"

    "    smCoord.xy = sceneShadowProj.xy  + (offset.zw) * softFactor; \n"
    "    shadow +=  shadow2DProj( osgShadow_shadowTexture, smCoord ).x *pass_div; \n"
    "  } \n"
    // skip all the expensive shadow sampling if not needed
    "  if ( shadow * (shadow -1.0) != 0.0 ) \n"
    "  { \n"
    "    shadow *= pass_div; \n"
    "    for (int i=0; i<SAMPLECOUNT_D2 -4; ++i){ \n"
    "      vec4 offset = 2.0 * texture3D( osgShadow_jitterTexture, jitterCoord ) - 1.0; \n"
    "      jitterCoord.z += 1.0 / SAMPLECOUNT_D2_FLOAT; \n"

    "      smCoord.xy = sceneShadowProj.xy  + offset.xy * softFactor; \n"
    "      shadow +=  shadow2DProj( osgShadow_shadowTexture, smCoord ).x * INV_SAMPLECOUNT; \n"

    "      smCoord.xy = sceneShadowProj.xy  + offset.zw * softFactor; \n"
    "      shadow +=  shadow2DProj( osgShadow_shadowTexture, smCoord ).x * INV_SAMPLECOUNT; \n"
    "    } \n"
    "  } \n"
#if !(NO_DEFERRED_SHADING)
    // apply color and object base texture
      "  vec4 color = gl_Color * texture2D( osgShadow_baseTexture, gl_TexCoord[0].xy ); \n"
    // apply shadow, modulo the ambient bias
       "  gl_FragColor = color * (osgShadow_ambientBias.x + shadow * osgShadow_ambientBias.y); \n"
#else
    /* "  vec4 color = gl_Color * texture2D( osgShadow_baseTexture, gl_TexCoord[0].xy ); \n"
    " gl_FragColor = color; \n"*/
    " gl_FragColor = gl_Color; \n"
#endif 
    "} \n";



int main ()
{
    // Useful declaration.
    osg::ref_ptr<osg::StateSet> ss;
    // Scene.
    osg::Vec3 lightPos(0, 0, 80);
    osg::ref_ptr<osg::Group> scene = createSceneRoom();
    osg::ref_ptr<osg::LightSource> light = createLight(lightPos);
    scene->addChild(light.get());
    // Shadowed scene.
    osg::ref_ptr<osgShadow::SoftShadowMap> shadowMap = new osgShadow::SoftShadowMap;
    shadowMap->setJitteringScale(16);
    std::string pass1ShadowPath = Utils::Helper::Ins().getMediaPath() + "osg_deferred_shading/pass1Shadow.frag";
    //shadowMap->addShader(osgDB::readShaderFile(pass1ShadowPath));
   // shadowMap->addShader(new osg::Shader(osg::Shader::FRAGMENT,fragmentSoftShaderSource_withBaseTexture));
    shadowMap->setLight(light);
    osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene = new osgShadow::ShadowedScene;
    shadowedScene->setShadowTechnique(shadowMap.get());
    shadowedScene->addChild(scene.get());
#ifdef USE_EFFECT_COMPOSITOR
    Pipeline p = createPipelineEffectCompositor(scene, shadowedScene, lightPos);
#else
    Pipeline p = createPipelinePlainOSG(scene, shadowedScene, lightPos);
#endif
    // Quads to display 1 pass textures.
    osg::ref_ptr<osg::Camera> qTexN =
        createTextureDisplayQuad(osg::Vec3(0, 0.7, 0),
                                 p.pass2Normals,
                                 p.textureSize);
    osg::ref_ptr<osg::Camera> qTexP =
        createTextureDisplayQuad(osg::Vec3(0, 0.35, 0),
                                 p.pass2Positions,
                                 p.textureSize);
    osg::ref_ptr<osg::Camera> qTexC =
        createTextureDisplayQuad(osg::Vec3(0, 0, 0),
                                 p.pass2Colors,
                                 p.textureSize);
    // Qaud to display 2 pass shadow texture.
    osg::ref_ptr<osg::Camera> qTexS =
        createTextureDisplayQuad(osg::Vec3(0.7, 0.7, 0),
                                 p.pass1Shadows,
                                 p.textureSize);
    // Quad to display 3 pass final (screen) texture.
    osg::ref_ptr<osg::Camera> qTexFinal =
        createTextureDisplayQuad(osg::Vec3(0, 0, 0),
                                 p.pass3Final,
                                 p.textureSize,
                                 1,
                                 1);
    // Must be processed before the first pass takes
    // the result into pass1Shadows texture.
    p.graph->insertChild(0, shadowedScene.get());
    // Quads are displayed in order, so the biggest one (final) must be first,
    // otherwise other quads won't be visible.
    p.graph->addChild(qTexFinal.get());
    p.graph->addChild(qTexN.get());
    p.graph->addChild(qTexP.get());
    p.graph->addChild(qTexC.get());
    p.graph->addChild(qTexS.get());
    // Display everything.
    osgViewer::Viewer viewer;
    // Make screenshots with 'c'.
    viewer.addEventHandler(
        new osgViewer::ScreenCaptureHandler(
            new osgViewer::ScreenCaptureHandler::WriteToFile(
                "screenshot",
                "png",
                osgViewer::ScreenCaptureHandler::WriteToFile::OVERWRITE)));
    viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
#if !NO_DEFERRED_SHADING
    viewer.setSceneData(p.graph.get());
#else
    viewer.setSceneData(shadowedScene);
#endif 
    viewer.setRunMaxFrameRate(40);
    viewer.setUpViewInWindow(300, 100, 800, 600);
    return viewer.run();
}

