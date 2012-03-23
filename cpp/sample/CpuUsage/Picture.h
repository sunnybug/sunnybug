#if !defined(AFX_PICTURE_H__CE6B2C27_1A86_4968_8196_DEF788210573__INCLUDED_)
#define AFX_PICTURE_H__CE6B2C27_1A86_4968_8196_DEF788210573__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Picture.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPicture window
#include "cpuusage.h"

class CPicture : public CStatic
{
// Construction
public:
	CPicture();

// Attributes
public:
	CCpuUsage cpu;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPicture)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPicture();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPicture)
	afx_msg void OnPaint();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICTURE_H__CE6B2C27_1A86_4968_8196_DEF788210573__INCLUDED_)
