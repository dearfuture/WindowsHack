/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2010-03-05
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int a = 1, b, c, d, e, f, g;
	b = argc+a*4+6;	// 形式1
	c = argc+a*3+6; // 形式2
	d = argc*2; 	// 形式3
	e = argc*3; 	// 形式4
	f = argc*4; 	// 形式5
	g = argc*11; 	// 形式6
	printf("%d %d %d %d %d %d", b, c, d, e, f, g);

	return 0;
}

