/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-03-29
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"

int g_nNum = 1;			// ȫ�ֱ���
int _tmain(int argc, _TCHAR* argv[])
{
	int        nNum = 2;		// �ֲ�����
	static int sNum = 3;		// ��̬�ֲ�����
	int*       pNum = new int;	// �ѱ���
	*pNum = 4;

	printf("%X %X %X %X ",g_nNum,nNum,sNum,*pNum);

	delete pNum;
	return 0;
}


