### 光线追踪

基于概率采样的方式实现了蒙特卡罗光线追踪，蒙特卡罗光线追踪的本质就是通过概率理论，把半球积分绘制方程进行近似简化，使之可以通过少量相对重要的采样来模拟积分。

##### 编译环境

VS2013+QT5.8+Eigen

##### 功能实现

输入：通过选择记录场景信息的txt文件或者勾选默认的场景1、2加载场景

输出：绘制完成的场景图片
+ 基于BVH光线求交
+ 单向蒙特卡罗光线追踪
+ 双向蒙特卡罗光线追踪

##### 详细分析

1>光线求交

Object类实现了Mesh三角面片和Sphere球体物体对象

- Mesh类物体对象以三角面片构成，对于mesh物体求交首先通过BVH判定光线与物体哪个叶子节点包围盒有交，然后再判断光线与包围盒中每个三角面片是否有交。对于三角形求交，设三角形的三个顶点为$v_0,v_1,v_2$ ,则三角形内部的点坐标可表示为

  $p=u(v_1-v_0)+v(v_2-v_0)$ 

  $u>=0,v>=0,u+v<1$ 

  射线上的点的坐标为

  $p=o+t*dir$

  因此射线与三角面片的交点满足

  $o+t*dir=u(v_1-v_0)+v(v_2-v_0)$

  根据x、y、z三个维度列出三个等式，然后根据克莱姆法则求解。

- Sphere球体物体求交可以根据距离求新$c$距离为$r$列出等式$(p-c)^T*(p-c)=r^2$求出交点。

2>单向蒙特卡罗光线追踪

根据物体表面光线传播的不同，实现了不同的材质：Light、Diffuse、Mirror、Glass、Metal
+ Light
  光源是发射能量的地方，因此从光源发出的光线的能量默认为是一个固定的值，并且以光源表面法向量构建半球以均匀分布向外发射光线，因此各个方向的采样概率为$P=\frac{1}{2\pi}$

+ Diffuse

  慢反射可以认为物体表面发射的能量朝各个方向以均匀分布向外发射能量，且概率为$P=\frac{1}{2\pi}$
+ Mirror

  镜面材质在此认为是理想的镜面反射，则反射方向为固定的方向，概率为1
+ Glass

  玻璃材质认为是同时拥有折射和反射的材质，因此某个点发射的能量与折射、反射方向传播过来的能量有关
+ Metal

  金属材质认为是一种各向异性的材质，在此采用Ward提出的BRDF分布函数
  $f(i,o)=\frac{\rho_s}{4\pi\alpha_x\alpha_y}e^{-\tan^2\theta_h\left(\frac{\cos^2\phi_h}{\alpha_x^2}+\frac{\sin^2\phi_h}{\alpha_y^2}\right)}$

  其中$\rho_s$ ，$\alpha_x$ ，$\alpha_y$ 是参数，h是根据参数概率产生的半角方向，用来代替法向产生反射方向，则

  $\theta_h=\arctan\sqrt{\frac{-\log{u}}{\cos^2{\phi_h}/\alpha_x^2+\sin^2{\phi_h}/\alpha_y^2}}$

  $\phi_h=\arctan\left(\frac{\alpha_y}{\alpha_x}\tan2\pi v\right)$

  其中，$u$和$v$是$[0,1]$之间的均匀分布随机数。

3>双向蒙特卡罗光线求交

双向蒙特卡洛依据单向蒙特卡洛采样得到从视点出发的路径P1和从光源出发得到的路径P2，在两条路径上分别找两个结点相连构成一条路径进行计算，连接边的概率采用两个采样方向概率的和的一半。

##### 效果展示

单向蒙特卡洛光线追踪

![](https://github.com/wb-finalking/Path_Tracing/blob/master/result/1/sample10000_MCSD2.bmp?raw=true)

![](https://github.com/wb-finalking/Path_Tracing/blob/master/result/2/sample10000_MCSD2.bmp?raw=true)

![](https://github.com/wb-finalking/Path_Tracing/blob/master/result/3/sample10000_m.bmp?raw=true)

双向蒙特卡洛光线追踪

![](https://github.com/wb-finalking/Path_Tracing/blob/master/result/1/sample1500_MCBD.bmp?raw=true)

![](https://github.com/wb-finalking/Path_Tracing/blob/master/result/2/sample2000_MCBD.bmp?raw=true)