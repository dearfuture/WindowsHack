/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2011-04-28
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"

long WINAPI FilterFunc(DWORD dwExceptionCode)

{
    return (dwExceptionCode == STATUS_STACK_OVERFLOW) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    DWORD dwTime = 500;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
        {
            __try
            {
                int nNum = 1+ul_reason_for_call;
            }
            __except ( FilterFunc(GetExceptionCode()) )
            {
                // �����ջ��������д���
                MessageBox(NULL,L"Exception Test",L"Test",MB_OK);
            }
        }
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}