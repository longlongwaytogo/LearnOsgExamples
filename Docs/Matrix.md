# Matrix 矩阵

![](image/The_Matrix.jpg)



矩阵变换在三维中的应用
1. 坐标系认识：
  世界坐标系，物体坐标系，相机坐标系
2. 矩阵的组成：
		
		R | T
		-----  ------------- matrix
		O | E

3. 矩阵计算基本法则：
		A+B = B + A
		A*B != B*A
		A*E = A
		A*A-1 = E

4. 矩阵的变换顺序： 行主序列矩阵 列主序列矩阵
		osg： Vertex * Scale * Rotate * Translate  
		opengl: Translate * Rotate * Scale * vertex
5. 项目中用的的矩阵

  - 位置传送
  - 手柄

