/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2011-06-02
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "stdafx.h"
#include <Windows.h>

// TLS变量
__declspec (thread) int  g_nNum    = 0x11111111;
__declspec (thread) char g_szStr[] = "TLS g_nNum = 0x%p ...\r\n";

// TLS回调函数A
void NTAPI TlsCallBack_A(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
    if (DLL_THREAD_DETACH == Reason)
        printf("t_TlsCallBack_A -> ThreadDetach!\r\n");
    return;
}
// TLS回调函数B
void NTAPI TlsCallBack_B(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
    if (DLL_THREAD_DETACH == Reason)
        printf("t_TlsCallBack_B -> ThreadDetach!\r\n");
    return;
}
/* 
* ".CRT$XLB"的含义是：
* CRT表名使用C RunTime机制
* X表示标识名随机
* L表示TLS callback section
* B其实也可以为B-Y的任意一个字母
*/
#pragma data_seg(".CRT$XLB")
PIMAGE_TLS_CALLBACK p_thread_callback[] = {TlsCallBack_A,TlsCallBack_B,NULL};
#pragma data_seg()
// 线程函数，用于演示TLS变量的特殊性
DWORD WINAPI t_ThreadFun(PVOID pParam)
{
    printf("t_Thread ->  first printf:");
    printf(g_szStr,g_nNum);
    g_nNum = 0x22222222; // 注意这里
    printf("t_Thread -> second printf:");
    printf(g_szStr,g_nNum);
    return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
    printf("_tmain -> TlsDemo.exe is runing...\r\n\r\n");

    CreateThread(NULL,0,t_ThreadFun,NULL,0,NULL);
    Sleep(100);  // 睡眠100毫秒用于确保第一个线程执行完毕
    printf("\r\n");
    CreateThread(NULL,0,t_ThreadFun,NULL,0,NULL);

    system("pause");
	return 0;
}