// DrawRect.h : main header file for the DRAWRECT application
//

#if !defined(AFX_DRAWRECT_H__6F15D3EC_2B2E_4FC2_A621_2116F02A33F0__INCLUDED_)
#define AFX_DRAWRECT_H__6F15D3EC_2B2E_4FC2_A621_2116F02A33F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

//////////////////////////////////////////////////////////////////////////
//mode
enum DrawMode
{
	DRAWMODE_FORMTEAM,				//队形
	DRAWMODE_ASTART,				//A*寻路
	DRAWMODE_CALC_PATHNODE,			//计算路径节点（直线，不考虑掩码）
};
extern const TCHAR* GetModeTxt();
extern DrawMode GetMode();

/////////////////////////////////////////////////////////////////////////////
// CDrawRectApp:
// See DrawRect.cpp for the implementation of this class
//

class CDrawRectApp : public CWinApp
{
public:
	CDrawRectApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawRectApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDrawRectApp)
	afx_msg void OnAppAbout();
	afx_msg void SetFormTeamMode();
	afx_msg void SetAStartMode();
	afx_msg void SetLine1Mode();
	afx_msg void SetLineSimpleMode();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWRECT_H__6F15D3EC_2B2E_4FC2_A621_2116F02A33F0__INCLUDED_)
