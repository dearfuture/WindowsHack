/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-03-05
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int a = 1, b, c, d, e, f, g;
	b = argc+a*4+6;	// ��ʽ1
	c = argc+a*3+6; // ��ʽ2
	d = argc*2; 	// ��ʽ3
	e = argc*3; 	// ��ʽ4
	f = argc*4; 	// ��ʽ5
	g = argc*11; 	// ��ʽ6
	printf("%d %d %d %d %d %d", b, c, d, e, f, g);

	return 0;
}

