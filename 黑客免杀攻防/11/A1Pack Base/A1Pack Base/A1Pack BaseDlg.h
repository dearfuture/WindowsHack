
// A1Pack BaseDlg.h : 头文件
//

#pragma once
#define A1PACK_BASE_EXPORTS
#include "../A1Pack_Base/A1Pack_Base.h"

#ifdef _DEBUG
#pragma comment(lib, "../Debug/A1Pack_Base.lib")
#else
#pragma comment(lib, "../Release/A1Pack_Base.lib")
#endif


// CA1PackBaseDlg 对话框
class CA1PackBaseDlg : public CDialogEx
{
// 构造
public:
	CA1PackBaseDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_A1PACKBASE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSelete();
	afx_msg void OnBnClickedStart();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedShowMsg();
	CString m_strPath;
	BOOL m_bShowMsg;
};
