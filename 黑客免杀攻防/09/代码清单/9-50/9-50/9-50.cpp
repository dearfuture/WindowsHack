/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-04-06
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"


class CObj
{
public:
	void ShowMsg(int nID)
	{
		printf("ID:%d Who is your God? I am!\r\n",nID);
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	CObj obj;
	obj.ShowMsg(9);
	return 0;
}
