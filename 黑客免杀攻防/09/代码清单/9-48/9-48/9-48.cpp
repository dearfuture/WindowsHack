/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2010-03-29
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "stdafx.h"

int g_nNum = 1;			// 全局变量
int _tmain(int argc, _TCHAR* argv[])
{
	int        nNum = 2;		// 局部变量
	static int sNum = 3;		// 静态局部变量
	int*       pNum = new int;	// 堆变量
	*pNum = 4;

	printf("%X %X %X %X ",g_nNum,nNum,sNum,*pNum);

	delete pNum;
	return 0;
}


