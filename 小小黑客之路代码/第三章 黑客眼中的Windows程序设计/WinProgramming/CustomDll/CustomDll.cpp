#include "CustomDll.h"
#include <windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	//动态链接库被映射到某个进程的地址空间
    case DLL_PROCESS_ATTACH:  
    //应用程序创建新的线程
	case DLL_THREAD_ATTACH:
    //应用程序某个线程正常终止
	case DLL_THREAD_DETACH:
    //动态链接库将被卸载
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//自定义导出函数，求两个整数的和
int DLLFuncAdd(int a,int b)
{

	return a+b;  
}
