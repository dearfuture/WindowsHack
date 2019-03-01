/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2010-03-26
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
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

