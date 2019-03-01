/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2010-02-08
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	switch (argc)
	{
	case 1:
		printf("argc1=0",argc);
		break;
	case 92:
		printf("argc12=%d",argc);
		break;
	case 262:
		printf("argc1=%d",argc);
		break;
	case 118:
		printf("argc118=%d",argc);
		break;
	case 25:
		printf("argc25=%d",argc);
		break;
	case 456:
		printf("argc456=%d",argc);
		break;
	case 588:
		printf("argc588=%d",argc);
		break;
	case 896:
		printf("argc896=0",argc);
		break;
	case 1000:
		printf("argc1000=%d",argc);
		break;
	case 1090:
		printf("argc1090=%d",argc);
		break;
	case 2100:
		printf("argc2100=%d",argc);
		break;
	default:
		printf("default nNum=%d,error!",argc);
	}

	return 0;
}

