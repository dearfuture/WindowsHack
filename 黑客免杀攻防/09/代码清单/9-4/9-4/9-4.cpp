/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-01-03
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"

int __cdecl fun_a(int nNumA, int nNumB, int nNumC)	// C�淶
{
	return nNumA+nNumB+nNumC;
}
int __fastcall fun_b(int nNumA, int nNumB, int nNumC)// ���ٵ���
{
	return nNumA+nNumB-nNumC;
}
int __stdcall fun_c(int nNumA, int nNumB, int nNumC)	// ��׼����
{
	return nNumA-nNumB-nNumC;
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf(
		"a=%d, b=%d, c=%d",
		fun_a(argc,1,2), fun_b(argc,1,2), fun_c(argc,1,2));
	return 0;
}


