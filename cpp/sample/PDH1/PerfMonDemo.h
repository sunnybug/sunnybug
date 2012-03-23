// PerfMonDemo.h : main header file for the PERFMONDEMO application
//

#if !defined(AFX_PERFMONDEMO_H__9D3A449D_7396_4114_89EC_32F497D6956A__INCLUDED_)
#define AFX_PERFMONDEMO_H__9D3A449D_7396_4114_89EC_32F497D6956A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPerfMonDemoApp:
// See PerfMonDemo.cpp for the implementation of this class
//

class CPerfMonDemoApp : public CWinApp
{
public:
	CPerfMonDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPerfMonDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPerfMonDemoApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERFMONDEMO_H__9D3A449D_7396_4114_89EC_32F497D6956A__INCLUDED_)
