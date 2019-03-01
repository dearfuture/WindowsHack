第十二章	特洛伊那匹不吃草的马

1.案例说明：
NsDoor为本章案例对应的解决方案;
NsDoor.sln为包含了所有项目的解决方案文件,使用Visual Studio 2008打开;
Release文件夹下的Client.exe、Server.exe、Loader.exe为案例对应的可执行文件。

案例用于演示木马的编写技巧,其中包含多个项目:
Loader项目用于实现木马的加载过程;
HookLib项目实现键盘监控的功能,用于记录按键;
Client项目为客户端控制的实现;
Server项目为服务端整体,实现了各种远程控制的功能。

2.推荐测试环境：
操作系统：	Windows XP sp3
虚拟机：	VMWare 6.5 + Windows XP sp3
编译器：	Visual Studio 2008
编译选项：	字符集设置为未设置
build版本：	release版本

3.使用说明：
推荐客户端使用实体机、服务端使用虚拟机进行测试;
Server项目的Header.h中的反弹上线地址需要根据实际情况进行填写;
测试时必须保证客户端与服务端连接畅通。
