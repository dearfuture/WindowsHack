/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-01-05
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc>0)
	{
		if (argc == 1)
			printf("Hello!\r\n"); 
		else
			printf("Hello everybody!\r\n");
	}
	else
	{
		if (argc == 1)
			printf("World!\r\n"); 
		else
			printf("Hello everybody!\r\n");
	}

	return 0;
}

