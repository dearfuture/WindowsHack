/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-01-17
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	printf("Mom! I can sing my ABC!\r\n");

	// ����СBaby��ʼ��ABC�ˡ���
	for (int nNum = 26; nNum>0; nNum--)
		printf("%c ",0x41+(26-nNum) );

	return 0;
}

