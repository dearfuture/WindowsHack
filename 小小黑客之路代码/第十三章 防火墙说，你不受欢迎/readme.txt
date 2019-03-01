第十三章	防火墙说，你不受欢迎

1.案例说明
FireWall为本案例对应的解决方案;
FireWall.sln为解决方案文件,使用Visual Studio 2008打开;
Release文件夹下的FireWall.exe为本案例的可执行文件,需要和FireWallDll.dll放在同一文件夹内。

案例实现了一个SPI防火墙,共有两个项目组成:
FireWall项目为防火墙UI实现等;
FireWallDll项目为SPI过滤动态链接库。

2.推荐测试环境
操作系统：	Windows XP sp3
编译器：	Visual Studio 2008
编译选项：	默认设置
build版本：	release版本

3.使用说明
启动防火墙后，在托盘区会出现一个图标，在该图标上单击右键会出现功能菜单;
防火墙刚启动时，处于未打开保护状态，关闭防火墙时会自动关闭保护;
该案例仅应用与基于WinSock的TCP和UDP通信，对于ICMP及ARP无效;
添加新的IP黑名单时要重新启动浏览器后再访问该IP，直接刷新网页不会遭到拦截。