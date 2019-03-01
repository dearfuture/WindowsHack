/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2010-04-19
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
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
	~CObj()  // 新添加的析构函数
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


