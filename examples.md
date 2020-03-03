1.example_osganimate
一）演示了路径动画的使用（AnimationPath、AnimationPathCallback），路径动画回调可以作用在Camera、CameraView、MatrixTransform、PositionAttitudeTransform等四种类型的节点上。
二）演示了osgSim::OverlayNode的使用

2.example_osganimationeasemotion
一）演示了osgAnimation::EaseMotion的使用，EaseMotion可以用于表达位移、速度值随时间的变化情况，通过不同的模板参数（functor），实现了不同的变化曲线。
二）程序中用到了一个使用osgWidget构造的弹出式菜单。

3.example_osganimationhardware
一）演示了在osgAnimation::RigGeometry中使用自定义RigTransformImplementation的方法。
二）可以参考程序中获取骨骼动画模型包含的动画列表的方法。

4.example_osganimationmakepath
一）演示了osgAnimation::Sampler采样器的使用，更新物体颜色（vec4）、物体位置（vec3）。Sampler派生模板类用于在一组已知离散数据样本的基础上，进行插值计算，得到连续数据。通过模板参数支持多种插值方法。
二）在更新物体位置这方面，与路径动画AnimationPath类似，但它需要自己写回调来更新节点矩阵，好处是能够使用更灵活的方式来进行数据采样，而AnimationPath只能进行线性内插。

5.example_osganimationmorph
一）演示了利用osgAnimation::MorphGeometry实现几何体变形动画的方法。

6.example_osganimationnode
一）同osganimationmakepath演示的功能基本相同。

7.example_osganimationskinning
一）演示了使用osgAnimation::Skeleton、osgAnimation::Bone构建骨骼，使用osgAnimation::Animation、osgAnimation::Channel构建动画，使用osgAnimation::RigGeometry构建蒙皮，以及使用osgAnimation::BasicAnimationManager播放骨骼动画的流程。可以做为学习osgAnimation骨骼动画实现结构的参考。

8.example_osganimationsolid
一）演示了利用多个Channel对一个物体进行运动控制（最终落实到更新MT节点的矩阵）。与示例osganimationmakepath的区别在于，这里使用了osgAnimation::Channel、osgAnimation::Animation、osgAnimation::UpdateMatrixTransform，对同一个物体应用了多个运动控制的叠加。

9.example_osganimationtimeline
一）演示了osgAnimation中的时间线控制。osgAnimation::Timeline、osgAnimation::TimelineAnimationManager。timeline是做什么用的？

10.example_osganimationviewer
一）演示了osgAnimation骨骼动画的加载及播放控制，可以在多个动作间切换，但播放多个动作、动作暂停等方面似乎不好使。
二）程序中出现了一个使用osgWidget构建的播放控制GUI，还有一个可以滑入滑出的菜单，可以作为GUI构建的参考。

11.example_osgautocapture
一）一个自动截屏的示例程序，用到了自定义osgViewer::Renderer，osg::Camera::DrawCallback等。
二）里面有一段用经纬高和方向角定位相机的代码。

12.example_osgautotransform
一）演示了利用osg::AutoTransform实现自动的缩放、旋转，从而达到按指定的效果自动面向屏幕。

13.example_osgbillboard
一）演示了osg::Billboard的使用。与AutoTransform的区别在哪里？

14.example_osgblendequation
一）演示了osg::BlendEquation的不同配置对于渲染结果的影响。

15.example_osgcallback
一）演示了OSG中各种回调方法的使用。

16.example_osgcamera
一）演示了单窗口多相机（使用SlaveCamera）、多窗口多相机（一个Viewer，多个GraphicsContext，多个Camera作为Slave添加到viewer下）、多屏幕多窗口。

17.example_osgcatch
一）一个类似接蛋游戏的小游戏程序

18.example_osgclip
一）演示了osg::ClipNode（osg::ClipPlane）的使用。

19.example_osgcluster
一）多机同步的演示

20.example_osgcompositeviewer
一）演示了osgViewer::CompositeViewer的使用

21.example_osgcopy
一）演示了osg中clone操作的使用，以及osg::CopyOp的自定义用法。

22.example_osgcubemap
一）演示如何使用高光贴图增强模型显示效果。高光贴图使用的立方体贴图（osg::TextureCubeMap），高光CubeMap使用osgUtil::HighlightMapGenerator自动生成。使用osg::TexGen以反射贴图的方式自动生成纹理坐标。并用osg::TexEnvCombine将高光贴图与原始颜色混合（ADD）。
osgUtil::HighlightMapGenerator派生自osgUtil::CubeMapGenerator，还有其他一些用途的立方体贴图生成器。

23.example_osgdatabaserevisions
一）演示了分页数据库本地缓存及数据版本检查的使用。

24.example_osgdelaunay
一）演示了DelaunayTriangulator的使用，用于对不规则网格点进行三角化处理。

25.example_osgdepthpartition
一）实现了一个派生自osg::Group的DepthPartitionNode节点类型，它可以自动对子节点树按照深度值进行划分，用于解决大场景下深度缓冲的问题。

26.example_osgdepthpeeling
DepthPeeling

27.example_osgdistortion
一）演示了类似哈哈镜的效果

28.example_osgdrawinstanced
一）演示了利用DrawArraysInstanced将一个四边形面片渲染成若干小面片的方法。

29.example_osgfadetext
一）演示了osgText::FadeText的使用，并使用osg::ClusterCullingCallback实现文字的聚集剪裁（根据指定法线与视线对物体进行剪裁）。FadeText封装了自动淡入淡出和自动进行互相间的遮挡技术的功能。

30.example_osgfont
一）演示了字体的使用

31.example_osgforest
一）演示了用多种技术方法实现森林的效果。首先对整个森林的所有树木做分块处理（cell），然后按照分块结构，使用多种方式进行建模。对树的建模使用的方法包括：Billboard、Shader、十字面片等。

32.example_osgfpdepth
一）演示了使用深度缓冲相关的应用。floating point depth buffer? invert the depth buffer?

33.example_osgfxbrowser
一）演示了使用osgFX创建的几种特殊渲染效果，使用osgFX::Effect管理各种效果。包括的效果有：AnisotropicLighting、BumpMapping、Cartoon、Outline、Scribe、SpecularHighlights。

34.example_osggameoflife
一）演示了一种叫做"Ping-Pong"的渲染技术，它是指利用FBO和两张纹理对象，在着色程序中以一个纹理为输入把计算结果写入另一个纹理，然后反转输入输出纹理再进行相同的操作，如此迭代。
二）其中用到的"GameOfLife"是使用Ping-Pong处理图片并输出的一个示例。

35.example_osggeodemo
一）演示了.geo文件的加载，以及geo模型的动画更新。
二）GEO PRO是CarbonGraphics出品的一个建模工具，.geo文件是它的模型文件格式。

36.example_osggeometry
一）演示了osg::Geometry（几何体）的各种构造方法。

37.example_osggeometryshades
一）演示了如何利用GeometryShader程序动态改变几何体的绘制，示例中把一个点绘制成不断变化的十字线。

38.example_osggpx
一）演示了一个跟踪路径的数据结构，以及使用osgDB::XmlParser对它的读写。

39.example_osggraphicscost
一）演示了osg::GraphicsCostEstimator的使用，osg::GraphicsCostEstimator用来评估编译/绘制几何体、纹理、着色程序、节点所需要的时间。

40.example_osghangglide
一）演示了使用程序构建场景的过程。
二）实现了一个滑行漫游器。

41.example_osgimagesequence
一）演示了使用osg::ImageSequence作为纹理贴图，并通过osg::ImageStream接口进行贴图动画播放控制的方法。ImageSequence贴图的加载有多种控制方案。

42.example_osgimpostor
一）演示了osgSim::Impostor的使用。Impostor节点的作用似乎是，通过在一些情况下，把物体渲染结果保存为纹理，而后通过渲染纹理代替渲染物体，以达到提高渲染效率的目的？
二）实现了一个自定义的漫游器。

43.example_intersection
一）演示并对别了多种求交计算的方法，如osgSim::LineOfSight、osgSim::HeightAboveTerrain、osgSim::ElevationSlice、osgUtil::IntersectorGroup。

44.example_osgkdtree
一）演示了kdtreee的使用。OSG能够在读取模型文件时自动构建kdtree。

45.example_keyboard
一）演示了键盘输入消息的处理。

46.example_keyboardmouse
一）演示了鼠标、键盘消息的处理。

47.example_osglauncher
一）一个三位的程序导航菜单。为每个例子程序构造一个立方体对象，通过即时的求交计算判定当前选中的对象，从而关联一个例子程序并启动执行。

48.example_osglight
一）演示了光照的使用。

49.example_osglightpoint
一）演示了osgSim::LightPoint、osgSim::LightPointNode的使用。

50.example_osglogicop
一）演示了osg::LogicOp（glLogicOp）的使用。

51.example_osglogo
一）演示了logo的制作。

52.example_osgmanipulator
一）演示了osgManipulator中各种拖拽器（Dragger）的使用。

53.example_osgmemorytest
一）一些GL操作的测试。

54.example_osgmotionblur
一）演示了如何利用累积缓冲区（accumulation buffer）实现一个简单的运动模糊（motion blur）效果。

55.example_osgmovie
一）演示了视频播放的功能。需要使用视频读取插件，如osgdb_quicktime、osgdb_ffmpg

56.example_osgmultiplerendertargets
一）演示了如何使用RTT(Render To Texture)、MRT(Multiple Render Targets)。

57.example_osgmultitexture
一）演示了多重纹理的使用。

58.example_osgmultitexturecontrol
一）演示了osgFX::MultiTextureControl的使用。

59.example_osgmultiviewpaging
一）多视口分屏的演示。

60.example_osgoccluder
一）演示了osg::ConvexPlanarOccluder的使用。

61.example_osgocclusionquery
一）演示了osg::OcclusionQueryNode的使用。

62.example_osgoit
一）depth peeling的使用。

63.example_osgoutline
一）演示了使用osgFX::Outline绘制物体外轮廓线的方法。

64.example_osgpackeddepthstencil
一）osg::Camera::PACKED_DEPTH_STENCIL_BUFFER的使用。

65.example_osgpagedlod
一）演示了利用osg::PagedLOD实现层次细节及流式加载。

66.example_osgparametric
一）演示了使用顶点着色程序（Vertex Shader Program）实现简单的变形动画。

67.example_osgparticle
一）几个粒子效果的演示，并使用了自定义osgParticle::Operator的方法控制粒子运动。

68.example_osgparticleeffects
一）演示了osgParticle中几个预定义Effect的效果，包括爆炸、爆炸碎片、烟、火等。

69.example_osgparticleshader
一）osgParticle实现的一个喷泉效果

70.example_osgpdf
使用osgWidget::PdfReader读取并显示pdf文档的例子，需要有相应插件支持。

71.example_osgphotoalbum
一个3D相册演示程序？实现了一种把多个图片文件合并到一个相册文件中的格式。

72.example_osgpick
通过求交计算进行拾取判断的一个例子。

73.example_osgplanets
天体运行的一个示例程序，其中使用billboard为太阳制作了大气层效果。

74.example_osgpoints
演示了使用点精灵（osg::PointSprite）绘制点，以及设置点大小。

75.example_osgpointsprite
使用点精灵绘（osg::PointSprite）制点云的示例。

76.example_osgposter
演示了渲染大分辨率图片（high-resolution image）的方法。

77.example_osgprecipitation
演示了osgParticle::PrecipitationEffect实现的雨、雪效果。

78.example_osgprerender
演示了渲染到纹理（RTT）的使用。在PRE_RENDER时把场景渲染到纹理上，再使用这个纹理渲染主场景。

79.example_osgprerendercubemap
演示了使用立方体贴图（osg::TextureCubeMap）实现环境反射的方法。

80.example_osgrefelect
演示了一个镜面反射效果的实现。

81.example_osgrobot
演示了一个由矩阵节点树构造成的多关节机器人。

82.example_osgscalarbar
演示了使用osgSim::ScalarBar实现的颜色标尺效果。

83.example_osgscreencapture
演示了利用osg::Camera::DrawCallback派生类实现截屏的方法。

84.example_osgscribe
在模型表面附加网格线的效果，利用osg::PolygonMode和osg::PolygonOffset实现，与osgSim::Scribe效果和原理一致。

85.example_osgsequence
演示了序列化播放多个模型的效果，利用osg::Sequence节点实现。

86.example_osgshadercomposition
演示了着色程序组合（ShaderComposition）的使用，用到osg::ShaderAttribute

87.example_shadergen
演示了利用osgUtil::ShaderGenVisitor生成替换固定功能管线的GLSL代码。

88.example_osgshaders
演示了Data\shaders目录下几个shader代码文件的使用。

89.example_osgshaderterrain
一个在着色程序中通过纹理采样计算地形高程来生成地形的示例。

90.example_osgshadow
包含多种实时阴影算法实现的阴影效果演示程序。

91.example_osgshape
利用osg::ShapeDrawable构建常用几何体的演示。

92.example_sharedarray
演示了以自定义数据结构构造几何体的方法，派生osg::Array类。

93.example_osgsidebyside
osgSim::MultiSwitch、osgSim::DOFTransform节点的控制。单窗口多视口的使用。

94.example_osgsimplifier
演示了使用osgUtil::Simplifier对模型进行简化的方法。

95.example_osgsimulation
演示了节点跟踪漫游器（osgGA::NodeTrackerManipulator）的使用。

96.example_osgslice
应该是很老的一个示例，还在直接使用osgUtil::SceneView，没有链接osgViewer库，所以窗口GC调用失败，不能运行。

97.example_osgspacewarp
使用一个小技巧实现点在相机运动情况下观察变成线的效果（类似虚影的效果）。小技巧是，对于每个点，也绘制一条线，线的一个端点始终在点的当前位置上，另一个端点使用点在上一帧的位置。

98.example_osgspheresegment
使用osgSim::SphereSegment绘制弧面的例子。

99.example_osgspotlight
演示了聚光灯的使用。osg::LightSource

100.example_osgstereoimage
演示了生成立体图片的方法。

101.example_osgstereomatch
左右眼图像立体匹配的示例，使用Data\images目录下的狗的图片运行。

102.example_osgteapot
使用osg::Drawable派生类封装经典的glut_teapot的绘制。

103.example_osgterrain
动态控制地形参数的示例，使用osgTerrain::Terrain。

104.example_tessellate
演示了镶嵌（tessellation）的使用，osgUtil::Tessellator。

105.example_osgtext
演示了文字的各种使用方式，osgText::Text。

106.example_osgtext3D
演示了立体文字的使用，osgText::Text3D。

107.example_osgtexture1D
演示了一维纹理的使用osg::Texture1D，纹理坐标生成节点osg::TexGenNode。

108.example_osgtexture2D
演示了二维纹理的使用osg::Texture2D，并通过更新回调演示了纹理在不同纹理过滤方式（filter）、纹理环绕模式（wrap）、各项异型系数（anisotropic）下的不同效果。

109.example_osgtexture3D
演示了三维纹理的使用osg::Texture3D。示例中用多张二维图片组装出一个三维纹理对象，在顶点数据中指定s、t两个纹理坐标，而使用osg::TexGen单独生成纹理坐标r（TexGen可以用来独立的生成某一维的纹理坐标），通过在更新回调中更新TexGen的参数动态更新纹理坐标r，由于纹理过滤方式使用的线性过滤，从而得到的效果是在多张纹理间平滑切换。

110.example_osgtexturecompression
演示了多种压缩格式（InternelFormatMode）的对比效果。同时有多视口osgViewer::CompositeViewer的使用。

111.example_osgtexturerectangle
演示了矩形纹理osg::TextureRectangle的使用，矩形纹理的纹理坐标范围必须为图片像素范围，并且环绕模式不支持重复，不支持边框和mipmap。

112.example_osgthirdpersonview
使用osgViewer::CompositeViewer构建多窗口视图，并在第二个窗口中绘制第一个窗口相同的场景的同时绘制出第一个窗口相机的视锥体。

113.example_osgthreadedterrain
一）使用增量编译osgUtil::IncrementalCompileOperation，避免加载大模型时造成的帧率下降。
二）osgTerrain::Layer、osgTerrain::TerrainTile的使用。

114.example_osguniformbuffer
演示了osg::UniformBufferObject的使用。

115.example_osgunittests
单元测试代码。

116.example_osguserstats
演示了如何在OSG的统计信息显示界面中显示用户自定义的统计信息。

117.example_osgvertexattributes
演示了在着色程序中使用顶点属性（VertexAttribute）。

118.example_osgvertexprogram
使用立方体贴图（CubeMap）和顶点着色程序（VertexShader）实现环境反射的例子，同时提供了一个天空盒的实现。

119.example_osgviewerGLUT
OSG与GLUT窗口系统的结合。

120.example_osgviewerMFC
OSG与MFC的结合。

121.example_osgvirtualprogram
着色程序的使用。

122.example_osgvolume
三维纹理和体渲染的示例。

123.example_osgwidgetaddremove
osgWidget动态添加和删除Widget的示例，有鼠标事件的自定义处理。

124.example_osgwidgetbox
osgWidget::Box的使用，以及拖拽处理。

125.example_osgwidgetcanvas
osgWidget::Canvas的使用。

126.example_osgwidgetframe
osgWidget::Frame的使用，能够拖动左右下及角上的边框改变Widget大小，拖动上侧的边框移动Widget。

127.example_osgwidgetinput
接收文字输入的osgWidget::Input的使用。

128.example_osgwidgetlabel
文字标签osgWidget::Label的使用。

129.example_osgwidgetmenu
使用osgWidget构造菜单的示例。

130.example_osgwidgetmessagebox
使用osgWidget::Frame、osgWidget::Box、osgWidget::Label等构建的MessageBox的示例。

131.example_osgwidgetnotebook
使用osgWidget构造的带Tab的记事本。

132.example_osgwidgetperformance
osgWidget效率测试。

133.example_osgwidgetscrolled
通过设置鼠标滚轮消息回调处理函数处理滚轮消息，实现osgWidget窗口的滚屏。

134.example_osgwidgetshader
对Widget使用着色程序渲染。

135.example_osgwidgetstyled
osgWidget::Style的使用。

136.example_osgwidgettable
表格osgWidget::Table的使用。

137.example_osgwidgetwindow
LUA脚本的使用。

138.example_osgwindows
多窗口拼接的示例。

139.example_osganalysis
场景分析？