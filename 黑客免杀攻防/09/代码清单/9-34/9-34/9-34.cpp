/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2010-03-02
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int nNum, nA = 8;
	nNum = argc - nA;      // 形式1：变量减变量
	printf("%d\r\n",nNum);
	nNum = argc - 9;       // 形式2：变量减常量
	printf("%d\r\n",nNum);
	nNum = nNum - 1;       // 形式3：变量减1
	printf("%d\r\n",nNum);

	return 0;
}

