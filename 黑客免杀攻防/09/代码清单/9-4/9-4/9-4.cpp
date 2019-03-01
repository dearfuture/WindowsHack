/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2010-01-03
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "stdafx.h"

int __cdecl fun_a(int nNumA, int nNumB, int nNumC)	// C规范
{
	return nNumA+nNumB+nNumC;
}
int __fastcall fun_b(int nNumA, int nNumB, int nNumC)// 快速调用
{
	return nNumA+nNumB-nNumC;
}
int __stdcall fun_c(int nNumA, int nNumB, int nNumC)	// 标准调用
{
	return nNumA-nNumB-nNumC;
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf(
		"a=%d, b=%d, c=%d",
		fun_a(argc,1,2), fun_b(argc,1,2), fun_c(argc,1,2));
	return 0;
}


