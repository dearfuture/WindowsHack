/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-01-19
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	printf("Mom! I can sing my ABC!\r\n");

	// ����СBaby��ʼ��ABC�ˡ���
	for (int nNum = 24; nNum>0; nNum--)
	{
		argc = (int)argv;  // �������������ͺܷ��������Ż���ƥ��������
		printf("%c ",0x41+(26-nNum) );
	}
	printf("%p",argc);

	return 0;
}

