/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-04-19
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"
#include <Windows.h>

class CObj
{
public:
	CObj():m_Obj_1(0xAAAAAAAA),m_Obj_2(0xBBBB)
	{
		printf("CObj() Constructor...\r\n");
	}
	~CObj()  // ����ӵ���������
	{
		printf("CObj() Destructor...\r\n");
	}
	void Show(int nID)
	{
		m_Obj_1 = 1;
		printf("ID:%d Who is your God? I am!\r\n",nID);
	}
private:
	int  m_Obj_1;
	WORD m_Obj_2;
};

int _tmain(int argc, _TCHAR* argv[])
{
	CObj obj;
	obj.Show(0);
	return 0;
}


