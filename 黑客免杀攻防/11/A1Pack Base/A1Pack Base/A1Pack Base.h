
// A1Pack Base.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CA1PackBaseApp:
// �йش����ʵ�֣������ A1Pack Base.cpp
//

class CA1PackBaseApp : public CWinApp
{
public:
	CA1PackBaseApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CA1PackBaseApp theApp;