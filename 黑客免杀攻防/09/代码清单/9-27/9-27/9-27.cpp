/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-01-28
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	switch (argc)
	{
	case 0:
		printf("argc=0",argc);
		break;
	case 1:
		printf("argc=%d",argc);
		break;
	case 6:
		printf("argc=%d",argc);
		break;
	case 7:
		printf("argc=%d",argc);
		break;
	case 9:
		printf("argc=%d",argc);
		break;
	default:
		printf("argc=%d,error!",argc);
	}

	return 0;
}

