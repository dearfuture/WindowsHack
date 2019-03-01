/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2010-03-16
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	printf("03=%d",argc/3);	//-*
	printf("05=%d",argc/5);	// |-> 注意，这些除数的倒数在上一段“1.9.2
	printf("11=%d",argc/11);// |    节”中有提及
	printf("59=%d",argc/59);//-*
	printf("04=%d",argc/4);
	printf("64=%d",argc/64);

	return 0;
}

