/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2010-05-05
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
	~CObj()
	{
		printf("CObj() Destructor...\r\n");
	}
	virtual void Show(int nID)
	{
		m_Obj_1 = 1;
		printf("\r\nID:%d Who is your God? I am!\r\n",nID);
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
		printf("CPeople() Destructor...\r\n");
	}
	virtual void LoveYou() = 0;
	virtual void KissYou() = 0;
private:
	int  m_People_1;
	WORD m_People_2;
};

class CBoy : public CPeople
{
public:
	CBoy():m_Boy_1(0x11111111),m_Boy_2(0x2222)
	{
		printf("CBoy() Constructor...\r\n");
	}
	~CBoy()
	{
		printf("CBoy() Destructor...\r\n");
	}
	void Show(int nID)
	{
		printf("ID:%d Boy!\r\n",nID);
	}
	void LoveYou()
	{
		printf("\r\nJessica, I love you, marry me!\r\n");
	}
	void KissYou()
	{
		printf("Jessica,you are too sexy,I want to kiss you.\r\n");
	}
private:
	int  m_Boy_1;
	WORD m_Boy_2;
};

class CGirl : public CPeople
{
public:
	CGirl():m_Girl_1(0x11111111),m_Girl_2(0x2222)
	{
		printf("CGirl() Constructor...\r\n");
	}
	~CGirl()
	{
		printf("CGirl() Destructor...\r\n");
	}
	void Show(int nID)
	{
		printf("ID:%d Girl!\r\n\r\n",nID);
	}
	void LoveYou()
	{
		printf("I love you too...But I am too shy to say...\r\n");
	}
	void KissYou()
	{
		printf("You do what you want...\r\n\r\n");
	}
private:
	int  m_Girl_1;
	WORD m_Girl_2;
};


int _tmain(int argc, _TCHAR* argv[])
{
	CObj    obj;
	CBoy    boy;
	CGirl   girl;

	// ����Ա�ϳ�����
	CObj *pobj;
	pobj = &obj;
	pobj->Show(0);
	pobj = &boy;
	pobj->Show(1);
	pobj = &girl;
	pobj->Show(2);

	// ��ʼ��һĻ��׷��
	CPeople *A1Pass  = new CBoy;  // A1Pass�����к�
	CPeople *Jessica = new CGirl; // Jessica����Ů��
	A1Pass->LoveYou();
	Jessica->LoveYou();
	A1Pass->KissYou();
	Jessica->KissYou();
	delete A1Pass;  // �ݳ���ϣ���Աжױ
	delete Jessica; // �ݳ���ϣ���Աжױ

	return 0;
}
// ---------- ������ ----------
// CObj() Constructor...
// CObj() Constructor...
// CPeople() Constructor...
// CBoy() Constructor...
// CObj() Constructor...
// CPeople() Constructor...
// CGirl() Constructor...
//
// ID:0 Who is your God? I am!
// ID:1 Boy!
// ID:2 Girl!
//
// CObj() Constructor...
// CPeople() CPeople
// CBoy() Constructor...
// CObj() Constructor...
// CPeople() Constructor...
// CGirl() Constructor...
//
// Jessica, I love you, marry me!
// I love you too...But I am too shy to say...
// Jessica, you are too sexy, I want to kiss you...
// You do what you want...
//
// CPeople() Destructor...
// CObj() Destructor...
// CPeople() Destructor...
// CObj() Destructor...
// CGirl() Destructor...
// CPeople() Destructor...
// CObj() Destructor...
// CBoy() Destructor...
// CPeople() Destructor...
// CObj() Destructor...
// CObj() Destructor...
// ----------------------------
