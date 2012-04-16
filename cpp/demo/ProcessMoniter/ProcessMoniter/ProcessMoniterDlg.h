// ProcessMoniterDlg.h : header file
//

#pragma once

#include "ProcessInfo.h"
#include "xPDH.h"
#include "afxwin.h"
#include <perfmon.h>

// CProcessMoniterDlg dialog
class CProcessMoniterDlg : public CDialog
{
// Construction
public:
	CProcessMoniterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PROCESSMONITER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	ProcessInfo	m_process_info;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_Info;
	CEdit m_edtInfo;
	CListCtrl m_lstPerf;

	CPerfMon	m_perm;

	void ResetPdh();
	//////////////////////////////////////////////////////////////////////////
	//每个线程的cpu
	int m_idxProcessThreadCount;
	CString m_strProcessName;
	typedef std::vector<int> IntList;
	IntList m_ThdCpuIdxs;
	IntList m_ThdIDIdxs;
};
