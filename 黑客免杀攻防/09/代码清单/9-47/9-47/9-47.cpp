/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-03-26
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"
#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	struct _TEST 
	{
		int    nNum;
		double dFloat;
		char   szStr[0x10];
	}stcTEST;
	stcTEST.nNum   = 0xAAAA;
	stcTEST.dFloat = 12.345;
	strcpy_s(stcTEST.szStr,"Hello World!");

	printf("%X %f %s",stcTEST.nNum,stcTEST.dFloat,stcTEST.szStr);

	return 0;
}

