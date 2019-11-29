# osg下使用GLSL实现polygon stipple 效果

效果图：
![image/1.stipple.png] 

![image/2.stipple.png]

stipple的实现原理类似于glDrawPixels,将位图(0或1表示的掩码)绘制到颜色缓冲区中。它独立于片段着色器。
实现polygon stipple的意义在于，版本的Opengl已经不支持stipple功能，需要自己通过shader实现。


具体实现主要步骤：
1. 构建mask位图信息
2. 创建shader文本
3. 向shader传入mask的uniform数组
4. 正确计算颜色和掩码

核心代码：
	std::string stippleFragment =  "uniform int stipple[128];"
	"void main(){\n"
	"ivec2 coord = ivec2(gl_FragCoord.xy - 0.5);\n"
	"uint highBit = 1u << 7;\n"
	"if((stipple[(coord.x%32 + coord.y%32*32)/8 ]  & ( highBit>>(uint)(coord.x%8)))== 0)\n"
	" discard;\n"
	"gl_FragColor = vec4(1,0,0,1);\n"
	"}"	;
	
 解析： 通过屏幕坐标计算出每个点的掩码值，进行与操作，为1则使用指定颜色或纹理，否则丢弃像素。
一点思考：
1. consle输出的位图是倒着的，显示器显示中是正确的，是opengl倒着绘制图像吗？