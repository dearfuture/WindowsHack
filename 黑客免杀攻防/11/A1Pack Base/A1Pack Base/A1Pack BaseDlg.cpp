
// A1Pack BaseDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "A1Pack Base.h"
#include "A1Pack BaseDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CA1PackBaseDlg �Ի���




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


// CA1PackBaseDlg ��Ϣ�������

BOOL CA1PackBaseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	// ��ʼ����ѡ��״̬
	m_bShowMsg = TRUE;
	// ʹ����֧���ļ���ק�Ĺ���
	DragAcceptFiles(TRUE);

	UpdateData(false);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CA1PackBaseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CA1PackBaseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CA1PackBaseDlg::OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	::ShellExecute(m_hWnd,L"open",pNMLink->item.szUrl,NULL,NULL,SW_SHOWNORMAL);
	*pResult = 0;
}


void CA1PackBaseDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CString strPath;
	LPTSTR  szFile;
	LPTSTR  szSuffix;

	// 1. ��ȡ��˦�ļ�����Ϣ
	DragQueryFile(hDropInfo,0,strPath.GetBuffer(MAX_PATH), MAX_PATH);
	szFile   = PathFindFileName(strPath);
	szSuffix = PathFindExtension(szFile);

	// 2. �ж����Ƿ���һ���Ϸ��ļ�����׺Ϊ.exe��
	if (_wcsicmp(szSuffix,L".exe") &&
		_wcsicmp(szSuffix,L"") )
	{
		MessageBox(L"��ѡ��Ĳ���EXE�ļ���");
		CDialogEx::OnDropFiles(hDropInfo);
		return;
	}

	m_strPath = strPath;
	UpdateData(false);

	CDialogEx::OnDropFiles(hDropInfo);
}


void CA1PackBaseDlg::OnBnClickedSelete()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);

	// 1. ѡ���ļ�
	CFileDialog dlg(                                // ����һ��CFileDialog����
		TRUE,                                       // �����ļ��򿪶Ի���
		L"*.exe",                                   // ȱʡ�ļ���չ��
		NULL,                                       // ��ʼ��ʾ���ļ����༭���е��ļ���
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,       // �Ի����־
		L"Executable file(*.exe)|*.exe|All(*.*)|*.*||", // ָ��������
		NULL);                                      // ָ���ļ��Ի�����󸸴���

	if ( dlg.DoModal() == IDCANCEL)
	{
		return;
	}

	// 2. ��ȡ·��
	m_strPath = dlg.GetPathName();

	UpdateData(false);
}


void CA1PackBaseDlg::OnBnClickedStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	if ( !A1Pack_Base(m_strPath.GetBuffer(),m_bShowMsg) )
		MessageBox(L"����ʧ��-_-!");
	else
	    MessageBox(L"���ܳɹ���");
}


void CA1PackBaseDlg::OnBnClickedShowMsg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
}
