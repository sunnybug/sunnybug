// PerfMonDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PerfMonDemo.h"
#include "PerfMonDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPerfMonDemoDlg dialog

CPerfMonDemoDlg::CPerfMonDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPerfMonDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPerfMonDemoDlg)
	m_strCpu = _T("0%");
	m_strMem = _T("0%");
	m_strCpuMax = _T("N/A");
	m_strCpuMean = _T("N/A");
	m_strCpuMin = _T("N/A");
	m_strMemMax = _T("N/A");
	m_strMemMean = _T("N/A");
	m_strMemMin = _T("N/A");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPerfMonDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPerfMonDemoDlg)
	DDX_Control(pDX, IDC_PROGRESS_MEM, m_wndMem);
	DDX_Control(pDX, IDC_PROGRESS_CPU, m_wndCpu);
	DDX_Text(pDX, IDC_STATIC_CPU, m_strCpu);
	DDX_Text(pDX, IDC_STATIC_MEM, m_strMem);
	DDX_Text(pDX, IDC_STATIC_CPUMAX, m_strCpuMax);
	DDX_Text(pDX, IDC_STATIC_CPUMEAN, m_strCpuMean);
	DDX_Text(pDX, IDC_STATIC_CPUMIN, m_strCpuMin);
	DDX_Text(pDX, IDC_STATIC_MEMMAX, m_strMemMax);
	DDX_Text(pDX, IDC_STATIC_MEMMEAN, m_strMemMean);
	DDX_Text(pDX, IDC_STATIC_MEMMIN, m_strMemMin);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPerfMonDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CPerfMonDemoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPerfMonDemoDlg message handlers

BOOL CPerfMonDemoDlg::OnInitDialog()
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

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	if (!m_PerfMon.Initialize())
	{
		AfxMessageBox("Could not initialize CPerfMon!");
		EndDialog(IDCANCEL);
		return true;
	}

	// add counters
	char ac[MAX_PATH] = {0};
	_snprintf(ac, sizeof(ac)-1, CNTR_PROCESS_CPU_s, "wandoujia");
	m_nCPU = m_PerfMon.AddCounter(ac);
	m_nMEM = m_PerfMon.AddCounter(CNTR_MEMINUSE_PERCENT);

	// init progress ctrls
	m_wndCpu.SetRange32(0, 100);
	m_wndCpu.SetPos(0);
	m_wndCpu.SetStep(1);
	m_wndMem.SetRange32(0, 100);
	m_wndMem.SetPos(0);
	m_wndMem.SetStep(1);

	// init timer
	SetTimer(1, 500, NULL); // every half second refresh the counters
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPerfMonDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPerfMonDemoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

HCURSOR CPerfMonDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPerfMonDemoDlg::OnTimer(UINT nIDEvent) 
{
	// collect the data
	if (!m_PerfMon.CollectQueryData())
	{
		AfxMessageBox("Failed Query!");
		return;
	}

	// update counters
	long lcpu = m_PerfMon.GetCounterValue(m_nCPU);
	long lmem = m_PerfMon.GetCounterValue(m_nMEM);

	// update dialog
	if (lcpu != -999)
	{
		m_strCpu.Format("%d%%", lcpu);
		m_wndCpu.SetPos(lcpu);
		long lMin, lMax, lMean;
		if (m_PerfMon.GetStatistics(&lMin, &lMax, &lMean, m_nCPU))
		{
			m_strCpuMin.Format("%d", lMin);
			m_strCpuMax.Format("%d", lMax);
			m_strCpuMean.Format("%d", lMean);
		}
		else
		{
			m_strCpuMin = "N/A";
			m_strCpuMax = "N/A";
			m_strCpuMean = "N/A";
		}
	}
	else
	{
		m_wndCpu.SetPos(0);
		m_strCpu = "FAIL";
	}

	if (lmem != -999)
	{
		m_strMem.Format("%d%%", lmem);
		m_wndMem.SetPos(lmem);
		long lMin, lMax, lMean;
		if (m_PerfMon.GetStatistics(&lMin, &lMax, &lMean, m_nMEM))
		{
			m_strMemMin.Format("%d", lMin);
			m_strMemMax.Format("%d", lMax);
			m_strMemMean.Format("%d", lMean);
		}
		else
		{
			m_strMemMin = "N/A";
			m_strMemMax = "N/A";
			m_strMemMean = "N/A";
		}
	}
	else
	{
		m_strMem = "FAIL";
		m_wndMem.SetPos(0);
	}

	UpdateData(false);
	
	CDialog::OnTimer(nIDEvent);
}

void CPerfMonDemoDlg::OnClose() 
{
	KillTimer(1);	
	m_PerfMon.Uninitialize();
	CDialog::OnClose();
}
