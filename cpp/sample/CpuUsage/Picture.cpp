// Picture.cpp : implementation file
//

#include "stdafx.h"
#include "tes.h"
#include "Picture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPicture

CPicture::CPicture()
{
}

CPicture::~CPicture()
{
}


BEGIN_MESSAGE_MAP(CPicture, CStatic)
	//{{AFX_MSG_MAP(CPicture)
	ON_WM_PAINT()
	ON_WM_ACTIVATE()
	ON_WM_CREATE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPicture message handlers

void CPicture::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	dc.SetTextColor(RGB(255, 255, 255));
	CRect rect;
	GetWindowRect(&rect);
	ScreenToClient(&rect);
//	CBrush brush(::GetSysColor(COLOR_BTNFACE));
	CBrush brush(RGB(0, 0, 0));
	dc.FillRect(&rect, &brush);
//	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(RGB(0, 0, 0));
	CTime time = CTime::GetCurrentTime();
	int year = time.GetYear();
	int month = time.GetMonth();
	int day = time.GetDay();
	CString s;
	s.Format("       %2d 年 %2d 月 %2d 日", year, month, day);
	dc.TextOut(0, 0, s);
	s = time.Format("        %H :%M: %S");
	dc.TextOut(15, 15, s);
	
	
	int i = cpu.GetCpuUsage();
	CString str;
	str.Format("        CPU使用率：%2d%% ", i);
	dc.TextOut(0, 45, str);
	
	MEMORYSTATUS   memstatus;   
	memstatus.dwLength   =sizeof(MEMORYSTATUS);   
	GlobalMemoryStatus(&memstatus); 
	DWORD	memoryLoad = memstatus.dwMemoryLoad;  
	char   buffer[255];   
	sprintf(buffer,"        内存使用率：%ld%%",memoryLoad);   
	dc.TextOut(0, 60, buffer);
	SetTimer(1, 1000, NULL);
	// Do not call CStatic::OnPaint() for painting messages
}

void CPicture::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CStatic::OnActivate(nState, pWndOther, bMinimized);
	
	// TODO: Add your message handler code here
	
}

int CPicture::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CPicture::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	Invalidate();
	CStatic::OnTimer(nIDEvent);
}
