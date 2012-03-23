// PerfMonDemoDlg.h : header file
//

#if !defined(AFX_PERFMONDEMODLG_H__90E6AB5B_33D0_471A_A181_ACE0F4B4F0B1__INCLUDED_)
#define AFX_PERFMONDEMODLG_H__90E6AB5B_33D0_471A_A181_ACE0F4B4F0B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPerfMonDemoDlg dialog

#include "perfmon.h"

class CPerfMonDemoDlg : public CDialog
{
// Construction
public:
	CPerfMonDemoDlg(CWnd* pParent = NULL);	// standard constructor

	CPerfMon m_PerfMon;
	int m_nCPU;
	int m_nMEM;

// Dialog Data
	//{{AFX_DATA(CPerfMonDemoDlg)
	enum { IDD = IDD_PERFMONDEMO_DIALOG };
	CProgressCtrl	m_wndMem;
	CProgressCtrl	m_wndCpu;
	CString	m_strCpu;
	CString	m_strMem;
	CString	m_strCpuMax;
	CString	m_strCpuMean;
	CString	m_strCpuMin;
	CString	m_strMemMax;
	CString	m_strMemMean;
	CString	m_strMemMin;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPerfMonDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPerfMonDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERFMONDEMODLG_H__90E6AB5B_33D0_471A_A181_ACE0F4B4F0B1__INCLUDED_)
