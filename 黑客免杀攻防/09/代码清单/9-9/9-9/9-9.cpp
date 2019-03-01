/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2010-01-05
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int nTest = 1;

	if (nTest>0)  // 第一个if-else
		printf("Hello!\r\n");
	else 
		printf("Hello everybody!\r\n");

	if (nTest>0)  // 第二个if-else
		printf("World!\r\n"); 
	else
		printf("Hello everybody!\r\n");

	printf("End!\r\n");

	return 0;
}

