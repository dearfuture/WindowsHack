/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2010-04-25
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
	~CObj()
	{
		printf("CObj() Destructor...\r\n");
	}
	virtual void Show(int nID)     // 注意这里
	{
		m_Obj_1 = 1;
		printf("ID:%d Who is your God? I am!\r\n",nID);
	}
private:
	int  m_Obj_1;
	WORD m_Obj_2;
};

class CPeople : public CObj
{
public:
	CPeople():m_People_1(0xCCCCCCCC),m_People_2(0xDDDD)
	{
		printf("CPeople() Constructor...\r\n");
	}
	~CPeople()
	{
		printf("CObj() Destructor...\r\n");
	}
	void Show(int nID)
	{
		printf("ID:%d People!\r\n",nID);
	}
private:
	int  m_People_1;
	WORD m_People_2;
};


int _tmain(int argc, _TCHAR* argv[])
{
	CObj obj;
	CPeople people;
	CObj *pobj;

	pobj = &obj;
	pobj->Show(0);
	pobj = &people;
	pobj->Show(1);
	return 0;
}
// ---------- 输出结果 ----------
// CObj() Constructor...
// CObj() Constructor...
// CPeople() Constructor...
// ID:0 Who is your God? I am!
// ID:1 People!
// CPeople() Destructor...
// CObj() Destructor...
// CObj() Destructor...
// ----------------------------

