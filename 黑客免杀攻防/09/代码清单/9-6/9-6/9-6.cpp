/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-01-04
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"


__declspec(naked) int fun(int nNumA, int nNumB, int nNumC)
{
	__asm
	{
		push ebp
			mov ebp,esp
			sub esp,0x4
	}
	nNumA += (nNumB+nNumC);	// ע�⣬����Ϊc��䡣
	__asm 
	{
		mov eax,nNumA
			add esp,0x4
			mov esp,ebp
			pop ebp
			retn
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf("fun=%d", fun(argc,1,2));
	return 0;
}


