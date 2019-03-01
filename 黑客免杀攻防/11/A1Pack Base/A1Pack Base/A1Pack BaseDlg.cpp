
// A1Pack BaseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "A1Pack Base.h"
#include "A1Pack BaseDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CA1PackBaseDlg 对话框




CA1PackBaseDlg::CA1PackBaseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CA1PackBaseDlg::IDD, pParent)
	, m_bShowMsg(FALSE)
	, m_strPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CA1PackBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, CHK_SHOW_MSG, m_bShowMsg);
	DDX_Text(pDX, EDIT_PATH, m_strPath);
}

BEGIN_MESSAGE_MAP(CA1PackBaseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CA1PackBaseDlg::OnNMClickSyslink1)
	ON_BN_CLICKED(BTN_SELETE, &CA1PackBaseDlg::OnBnClickedSelete)
	ON_BN_CLICKED(BTN_START, &CA1PackBaseDlg::OnBnClickedStart)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(CHK_SHOW_MSG, &CA1PackBaseDlg::OnBnClickedShowMsg)
END_MESSAGE_MAP()


// CA1PackBaseDlg 消息处理程序

BOOL CA1PackBaseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	// 初始化单选框状态
	m_bShowMsg = TRUE;
	// 使窗口支持文件拖拽的功能
	DragAcceptFiles(TRUE);

	UpdateData(false);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CA1PackBaseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CA1PackBaseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CA1PackBaseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CA1PackBaseDlg::OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	::ShellExecute(m_hWnd,L"open",pNMLink->item.szUrl,NULL,NULL,SW_SHOWNORMAL);
	*pResult = 0;
}


void CA1PackBaseDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString strPath;
	LPTSTR  szFile;
	LPTSTR  szSuffix;

	// 1. 获取拖甩文件的信息
	DragQueryFile(hDropInfo,0,strPath.GetBuffer(MAX_PATH), MAX_PATH);
	szFile   = PathFindFileName(strPath);
	szSuffix = PathFindExtension(szFile);

	// 2. 判断这是否是一个合法文件（后缀为.exe）
	if (_wcsicmp(szSuffix,L".exe") &&
		_wcsicmp(szSuffix,L"") )
	{
		MessageBox(L"您选择的不是EXE文件！");
		CDialogEx::OnDropFiles(hDropInfo);
		return;
	}

	m_strPath = strPath;
	UpdateData(false);

	CDialogEx::OnDropFiles(hDropInfo);
}


void CA1PackBaseDlg::OnBnClickedSelete()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);

	// 1. 选择文件
	CFileDialog dlg(                                // 构造一个CFileDialog对象
		TRUE,                                       // 创建文件打开对话框
		L"*.exe",                                   // 缺省文件扩展名
		NULL,                                       // 初始显示于文件名编辑框中的文件名
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,       // 对话框标志
		L"Executable file(*.exe)|*.exe|All(*.*)|*.*||", // 指定过滤器
		NULL);                                      // 指向文件对话框对象父窗口

	if ( dlg.DoModal() == IDCANCEL)
	{
		return;
	}

	// 2. 获取路径
	m_strPath = dlg.GetPathName();

	UpdateData(false);
}


void CA1PackBaseDlg::OnBnClickedStart()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if ( !A1Pack_Base(m_strPath.GetBuffer(),m_bShowMsg) )
		MessageBox(L"加密失败-_-!");
	else
	    MessageBox(L"加密成功！");
}


void CA1PackBaseDlg::OnBnClickedShowMsg()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
}
