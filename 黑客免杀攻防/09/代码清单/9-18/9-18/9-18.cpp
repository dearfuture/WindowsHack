/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-01-16
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	printf("Mom! I can sing my ABC!\r\n");

	while(argc>0)  // ����СBaby��ʼ��ABC�ˡ���
	{
		printf("%c ",0x41+(26-argc) );
		argc--;
	}

	return 0;
}

