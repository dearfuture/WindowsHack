/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2010-01-15
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int nNum = 26;
	printf("Mom! I can sing my ABC!\r\n");

	while(nNum>0)  // 听！小Baby开始唱ABC了……
	{
		printf("%c ",0x41+(26-nNum) );
		nNum--;
	}

	return 0;
}

