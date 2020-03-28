# osg 阴影技术研究：
1. [记录我的OSG动态阴影开发](https://my.oschina.net/u/235558/blog/3061713)
2. [实时渲染中的软阴影技术](https://zhuanlan.zhihu.com/p/26853641)
3. [实时阴影技术总结](https://xiaoiver.github.io/coding/2018/09/27/%E5%AE%9E%E6%97%B6%E9%98%B4%E5%BD%B1%E6%8A%80%E6%9C%AF%E6%80%BB%E7%BB%93.html)
4. [osgShadow讨论](http://bbs.osgchina.org/forum.php?mod=viewthread&tid=13977)
	
		
osgShadow不是个实验库，很多东西是可以直接用的，也有商业应用的价值。我个人也基于osgShadow实现了目前效果最好的阴影算法，包含了CSM和ESM

shadow map算法有两个不可能攻克的难题，一是projective aliasing，一是perspective aliasing。您这个就属于前者，因为正好有多个面和光照方向完全平行，所以shadow map上的一个light space像素被迫对应了camera space中大量的实际像素，此时花斑是无法避免的。而国际上主流的方法（asm）都难以做到离线

在实际工程中，正确的做法有两种，一是回避这种正好的平行面，二是预处理或者后处理模糊
还有shadow map是目前唯一可以做到大规模阴影实现的方法，shadow volume在大场景中就是渣渣。。。
CryEngine就是标准的CSM + ESM的方案

您目前的理解可能还比较浅，不妨阅读一些有关shadow map算法的文献，我推荐原版的《Realtime Shadows》一书。中文版虽然是我翻译的，但是能力和时间都有限，因此质量不是很好

csm（也就是您说的pssm，后者只是csm的一个变种）算法的意义是改善了阴影采样时的信号损失，但是它对于aliasing的问题是毫无贡献的。

后处理的filter比如人们最常用的pcf，还有改进的pcss；预处理的方法就是划时代的vsm，以及后继的esm等。

5. [探究软阴影实现方式1](https://blog.csdn.net/qq_23030843/article/details/104370860)
6. [klayge-esm](http://www.klayge.org/2013/10/07/%E5%88%87%E6%8D%A2%E5%88%B0esm/)
7. [shadow map的一些变种](http://www.aiuxian.com/article/p-2830825.html)

SM: Shadow Maps
ASM: Adaptive Shadow Maps
ASM/AFSM: Alias-free Shadow Maps
ASSM: High-Quality Adaptive Soft Shadow Mapping
BSM: Bitmap Shadow Maps
CSM: Cascaded Shadow Maps
CSM: Coherent Shadow Maps
DASM: Dynamic Adaptive Shadow Maps
ESM: Exponential Shadow Maps
LogSM: Logarithmic Shadow Maps
LogPSM: Logarithmic Perspective Shadow Maps
LogTSM: Logarithmic Trapezoidal Shadow Maps
LSPSM/LispSM: Light Space Perspective Shadow Maps (在OpenSceneGraph 2.7/2.8中有见到使用)
LVSM: Layered Variance Shadow Maps
MDSM: Multiple-Depth Shadow Maps
MTSM: Multilayer Transparent Shadow Maps
PLSM: Practical Logarithmic Shadow Maps
PSM: Perspective Shadow Maps
PSSM: Parallel-Split Shadow Maps (中国人发明，见《nvidia GPU Gems 3》第10章)
PSVSM Parallel-Split Variance Shadow Maps
QVSM: Queried Virtual Shadow Maps
RMSM: Resolution-Matched Shadow Maps
RSM: Robust Shadow Maps
SPPSM: Separating Plane Perspective Shadow Mapping
SSM: Standard Shadow Maps
SSM: Subdivision Shadow Maps
SSM: Soft Shadow Maps
SAVSM: summed-area variance shadow maps
SSSM: Simpler Soft Shadow Mapping
TSM: Trapezoidal Shadow Maps
VSM: Variance Shadow Maps
XPSM: Extended Perspective Shadow Maps

8. [osgShadow](http://www.openscenegraph.org/index.php/documentation/guides/programming-guides/108-shadows)
9. [Shadow Map 原理和改进](https://blog.csdn.net/ronintao/article/details/51649664)
10. [Deferred Rendering Shadow Mapping](http://www.codinglabs.net/tutorial_opengl_deferred_rendering_shadow_mapping.aspx)
11. [GraphicsLab Project之Parallel Split Shadow Map(PSSM)](https://blog.csdn.net/i_dovelemon/article/details/70246110)
12. [《GPU Gems 3》Chapter 10](https://www.cnblogs.com/pulas/archive/2012/02/02/2335593.html)
13. [osg document](https://codedocs.xyz/openscenegraph/OpenSceneGraph/a05094.html)


[Cascaded Shadow Maps(CSM)实时阴影的原理与实现](https://zhuanlan.zhihu.com/p/53689987)
[nv-csm](https://developer.download.nvidia.cn/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf)
[Directx11进阶教程之CascadeShadowMap(层级阴影)(上)](https://blog.csdn.net/qq_29523119/article/details/79266293)
 

