/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2010-01-22
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int nNum = 2;
	switch (nNum)
	{
	case 0:
		printf("nNum=0");
		break;
	case 1:
		printf("nNum=1");
		break;
	case 2:
		printf("nNum=2");
		break;
	default:
		printf("nNum=%d,error!",nNum);
	}

	return 0;
}

