/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-03-02
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int nNum, nA = 8;
	nNum = argc - nA;      // ��ʽ1������������
	printf("%d\r\n",nNum);
	nNum = argc - 9;       // ��ʽ2������������
	printf("%d\r\n",nNum);
	nNum = nNum - 1;       // ��ʽ3��������1
	printf("%d\r\n",nNum);

	return 0;
}

