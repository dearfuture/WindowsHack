/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2011-06-02
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"
#include <Windows.h>

// TLS����
__declspec (thread) int  g_nNum    = 0x11111111;
__declspec (thread) char g_szStr[] = "TLS g_nNum = 0x%p ...\r\n";

// TLS�ص�����A
void NTAPI TlsCallBack_A(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
    if (DLL_THREAD_DETACH == Reason)
        printf("t_TlsCallBack_A -> ThreadDetach!\r\n");
    return;
}
// TLS�ص�����B
void NTAPI TlsCallBack_B(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
    if (DLL_THREAD_DETACH == Reason)
        printf("t_TlsCallBack_B -> ThreadDetach!\r\n");
    return;
}
/* 
* ".CRT$XLB"�ĺ����ǣ�
* CRT����ʹ��C RunTime����
* X��ʾ��ʶ�����
* L��ʾTLS callback section
* B��ʵҲ����ΪB-Y������һ����ĸ
*/
#pragma data_seg(".CRT$XLB")
PIMAGE_TLS_CALLBACK p_thread_callback[] = {TlsCallBack_A,TlsCallBack_B,NULL};
#pragma data_seg()
// �̺߳�����������ʾTLS������������
DWORD WINAPI t_ThreadFun(PVOID pParam)
{
    printf("t_Thread ->  first printf:");
    printf(g_szStr,g_nNum);
    g_nNum = 0x22222222; // ע������
    printf("t_Thread -> second printf:");
    printf(g_szStr,g_nNum);
    return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
    printf("_tmain -> TlsDemo.exe is runing...\r\n\r\n");

    CreateThread(NULL,0,t_ThreadFun,NULL,0,NULL);
    Sleep(100);  // ˯��100��������ȷ����һ���߳�ִ�����
    printf("\r\n");
    CreateThread(NULL,0,t_ThreadFun,NULL,0,NULL);

    system("pause");
	return 0;
}