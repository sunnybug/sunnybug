// ProcessMoniterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessMoniter.h"
#include "ProcessMoniterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CProcessMoniterDlg dialog




CProcessMoniterDlg::CProcessMoniterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessMoniterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessMoniterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_EDT_INFO, m_edtInfo);
	DDX_Control(pDX, IDC_LIST_PERF, m_lstPerf);
}

BEGIN_MESSAGE_MAP(CProcessMoniterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CProcessMoniterDlg::OnBnClickedOk)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CProcessMoniterDlg message handlers

BOOL CProcessMoniterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_process_info.Init();
	m_perm.Initialize();
	CString str;
	GetDlgItemText(IDC_EDIT_PNAME, str);
	CString str2;
	str2.Format(CNTR_PROCESS_THREADCOUNT_s, str);
	m_idxProcessThreadCount = m_perm.AddCounter(str2);
	SetTimer(1, 500, NULL);

	//m_edtInfo.SetWindowText(CPDH::GetPDHInfo().c_str());
	CPDH::BrownsePDH();

	//init perf list control

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProcessMoniterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProcessMoniterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProcessMoniterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CProcessMoniterDlg::OnBnClickedOk()
{
	const DWORD dwPID = m_process_info.GetPID(_T("ProcessMoniter.exe"));
}

void CProcessMoniterDlg::OnTimer(UINT_PTR nIDEvent)
{
	m_process_info.OnTimer();

	m_Info.SetWindowText(m_process_info.GetProcessData().m_txt);
	//m_edtInfo.SetWindowText(CPDH::GetPDHInfo().c_str());
	
// 	//pid
// 	CString ProcessName;
// 	GetDlgItemText(IDC_EDIT_PNAME, ProcessName);
// 	const DWORD dwPID = m_process_info.GetPID(ProcessName);
// 
// 	//get thread count
// 	const int nThreadCount = m_perm.GetCounterValue(m_idxProcessThreadCount);
// 	str.Format("Thread Count:%d\r\n", nThreadCount);
// 
// 	//thread info
// 	CString strTmp;
// 	int idx  = 0;
// 	int val = 0;
// 	CString strThreadInfo;
// 	
// 	for (int i=0; i<nThreadCount; ++i)
// 	{
// 		strTmp.Format("%d", )
// 		strThreadInfo.Format(CNTR_THREAD_PRIVILEGEDTIME_s_d, (LPCSTR)ProcessName, i);
// 		int idx = m_perm.AddCounter((LPCSTR)strThreadInfo);
// 		ASSERT(idx >= 0);
// 		val = m_perm.GetCounterValue(idx);
// 		ASSERT(val >= 0);
// 		strTmp.Format("PrivilegedTime:%d")
// 	}
// 	m_edtInfo.SetWindowText(str);

	CDialog::OnTimer(nIDEvent);
}
