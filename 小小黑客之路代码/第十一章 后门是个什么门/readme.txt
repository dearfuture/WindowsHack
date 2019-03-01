第十一章	后门是个什么门

1.案例说明：
BackDoor为本章案例对应的解决方案;
BackDoor.sln为包含了所有项目的解决方案文件,使用Visual Studio 2008打开;
Release文件夹下的Client.exe、Loader.exe为案例对应的可执行文件。

案例用于演示后门程序的编写技巧,其中包含多个项目:
Loader项目用于实现后门的加载过程;
Dll项目包含后门真正的控制和功能实现代码;
Client项目为客户端控制的实现;
Server项目为服务端整体,包含了Loader、Dll作为资源文件。

2.推荐测试环境：
操作系统：	Windows XP sp3
虚拟机：	VMWare 6.5 + Windows XP sp3
编译器：	Visual Studio 2008
编译选项：	字符集设置为未设置
build版本：	release版本

3.使用说明：
推荐客户端使用实体机、服务端使用虚拟机进行测试;
Client项目内的目地IP和源IP需要根据实际情况进行配置;
由于Windows XP sp3不支持回环地址测试，请勿使用127.0.0.1作为服务端地址;
测试时必须保证客户端与服务端连接畅通。