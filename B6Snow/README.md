*马逸东 | 2015210056 | myd15@mails.tsinghua.edu.cn*

## B6 实现简单的粒子效果-下雪
要求：  

* 实现下雪的粒子效果；
* 粒子的大小不一，运动方向模拟雪花运动方向；
* 开始时雪花较少，随着时间慢慢增多；
* 自己设计或者网上下载一张雪花图片即可。



### 实验环境

Windows 10 + Visual Studio 2013 + OpenGL3.3, 依赖库：SOIL, GLEW, GLM

### 实验原理

简单的粒子系统
公告板(Billboard)着色器

### 实验步骤

首先用径向渐变制作了雪花图片  
![雪花](snow.png)

编写了雪这个粒子系统类[Snow.h](Snow.h)，有`Snow`和`Snowflake`两个类，后一个是表示每个雪花的类，前一个存储一个向量的`Snowflake`。   
由于雪的模型一样，所以`Snowflake`类只需要存储缩放尺寸和所在位置即可，渲染时传递不同的模型空间矩阵即可。  

雪的渲染效果使用了Billboard的方式，即每个雪花的面片始终朝向摄像机。另外由于雪花图片有透明度，需要开启混合，而且对于渲染顺序有要求。所以在每次渲染前根据雪花到相机的距离进行排序(`flakeComparer`类)，先渲染后面的面片。  

交互操作：  
`W`: 前进  
`A`: 后退  
`S`: 向左  
`D`: 向右  
`ScrollUp`: 放大  
`ScrollDown`：缩小  
`Alt+MouseL`: Rotate  
`Alt+MouseR`: Pan  

*注：由于GLUT不支持OpenGL3.3+，并且GLEW不提供GUI功能，实验所有的交互不使用右键PopUpMenu，而都使用鼠标键盘。*  

### 实验效果

![实现效果](B6.jpg)

