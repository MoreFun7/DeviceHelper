# UpdateTool

#### 介绍
基于QT的串口升级工具

#### 软件架构
软件架构说明
需要配置QT环境，编译工具是MingW 32位，QT使用是5.15.2版本。
目前为1.0版本，只支持针对特定的S10产品开发。
未来拟加入更多产品。


#### 安装教程

1.  xxxx
2.  xxxx
3.  xxxx

#### 使用说明

1.  配置好QT后，左下角选择release进行编译，生成.exe可执行文件
2.  在QT直接点运行即可打开可执行文件或者在文件夹中双击打开
3.  xxxx

#### 如何打包为一个exe

1.	首先在QT中左下角选择Release，点一下运行，编译出exe。
2.	找到该exe的文件夹，只将exe复制到一个新建的文件夹中。
3.	用Win+S搜索QT，打开对应的编译器，本项目打开的是QT 5.15.2(MinGW 8.10 32bit)。
4.	输入windeployat.exe ，后面跟上刚才那个文件夹中的exe全路径，如windeployqt.exe E:\Object\DeviceHelper\DeviceHelper.exe，回车即可编译完成。
5.	运行enigma virtual box(Enigma)，选择exe后，将windeployat编译好的dll和文件夹分别拖入(除了需要打包的那个exe)，文件设置选择Compress Files，点击打包即可。

详情可参考B站视频：https://www.bilibili.com/video/BV1c14y1B7dg/?spm_id_from=333.999.0.0&vd_source=f25c562d3cbc81ca27f3d7ba286bd22f

#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request


#### 特技

1.  使用 Readme\_XXX.md 来支持不同的语言，例如 Readme\_en.md, Readme\_zh.md
2.  Gitee 官方博客 [blog.gitee.com](https://blog.gitee.com)
3.  你可以 [https://gitee.com/explore](https://gitee.com/explore) 这个地址来了解 Gitee 上的优秀开源项目
4.  [GVP](https://gitee.com/gvp) 全称是 Gitee 最有价值开源项目，是综合评定出的优秀开源项目
5.  Gitee 官方提供的使用手册 [https://gitee.com/help](https://gitee.com/help)
6.  Gitee 封面人物是一档用来展示 Gitee 会员风采的栏目 [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)
