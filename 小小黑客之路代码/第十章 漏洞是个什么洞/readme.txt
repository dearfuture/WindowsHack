第十章	漏洞是个什么洞

1.案例说明：
ShellCode&Exploit文件夹下共有三个项目,分别用于演示各种漏洞的代码编写及利用技巧:
MessageBox ShellCode项目用于演示一个简单的MessageBox的ShellCode编写;
MS06-040 本地溢出利用项目演示了一个Windows早期的漏洞：MS06-040;
URLDownloadToFile ShellCode项目演示了一个高级的ShellCode编写技巧: URLDownloadToFile编写。

2.推荐测试环境：
项目一,MessageBox ShellCode :
操作系统：	Windows XP sp3
编译器：	Visual C++ 6.0
编译选项：	默认设置
build版本：	debug版本
项目二,MS06-040 本地溢出利用 :
操作系统：	Windows XP sp3
编译器：	Visual C++ 6.0
编译选项：	默认设置
build版本：	release版本
项目三,URLDownloadToFile ShellCode :
操作系统：	Windows XP sp3
编译器：	Visual C++ 6.0
编译选项：	默认设置
build版本：	debug版本

3.使用说明：
MS06-040项目中务必将其中自带的netapi32.dll与项目和生成的应用程序放在同一目录下.
可能因为绝对路径太长而不能出现文章中的测试效果，若这种情况出现，将URLDownloadToFile ShellCode.exe移动到路径较短的目录下即可。
