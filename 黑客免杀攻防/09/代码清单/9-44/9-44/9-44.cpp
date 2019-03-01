/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2010-03-24
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int  nArray[3] = {0x10,0x20,0x300};
	int *pPtr      = nArray;

	printf("%x %x %x\r\n", pPtr+0, pPtr+1, pPtr+2);
	printf("%x %x %x\r\n", *(pPtr+0), *(pPtr+1), *(pPtr+2));

	printf("%x %x %x\r\n", &nArray[0], &nArray[1], &nArray[2]);
	printf("%x %x %x\r\n", nArray[0], nArray[1], nArray[2]);

	return 0;
}

