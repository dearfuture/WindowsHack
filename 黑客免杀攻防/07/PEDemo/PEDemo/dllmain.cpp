/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2011-04-28
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
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
                // 如果是栈溢出，进行处理。
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