/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-03-24
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int  nArray[3] = {0x10,0x20,0x300};
	int *pPtr      = nArray;

	printf("%x %x %x\r\n", pPtr+0, pPtr+1, pPtr+2);
	printf("%x %x %x\r\n", *(pPtr+0), *(pPtr+1), *(pPtr+2));

	printf("%x %x %x\r\n", &nArray[0], &nArray[1], &nArray[2]);
	printf("%x %x %x\r\n", nArray[0], nArray[1], nArray[2]);

	return 0;
}

