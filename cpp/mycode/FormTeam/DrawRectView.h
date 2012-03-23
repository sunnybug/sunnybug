// DrawRectView.h : interface of the CDrawRectView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWRECTVIEW_H__9DBD9A60_9DCC_4BAC_9472_A4BB48F99695__INCLUDED_)
#define AFX_DRAWRECTVIEW_H__9DBD9A60_9DCC_4BAC_9472_A4BB48F99695__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Matrix.h"
#include "FormTeam.h"
#include <xTickCount.h>

class CDrawRectView : public CView
{
protected: // create from serialization only
	CDrawRectView();
	DECLARE_DYNCREATE(CDrawRectView)

// Attributes
public:
	CDrawRectDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawRectView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDrawRectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDrawRectView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void OnMouseMove(UINT nFlags, CPoint point);

	//////////////////////////////////////////////////////////////////////////
	//tick
protected:
	xTickCount m_Tick;
		
	//////////////////////////////////////////////////////////////////////////
protected:
	//draw
	void DrawPath(CDC& dc);

	//calc
	//计算鼠标所在的矩阵位置
	bool GetCurMatrixPos(OBJPOS& ptMatrixPos);
	//根据像素坐标，计算出对应的矩阵位置
	bool CalcMatrixPos(OBJPOS& ptMatrixPos, const POINT& pos);
protected:
	CDC		m_MemDC;
	CBitmap	m_MemBitmap;

	//////////////////////////////////////////////////////////////////////////
	//path find
protected:
	void	SetStart(const CPoint& ptStart=POINT_NONE);
	BOOL	GetStart(CPoint& pt);
	
	void	SetEnd(const CPoint& ptEnd=POINT_NONE);
	BOOL	GetEnd(CPoint& pt);
	
protected:
	CPoint	m_ptStart;
	CPoint	m_ptEnd;
	PathList	m_lstPath;

	//////////////////////////////////////////////////////////////////////////
	//team
protected:
	void	DrawTeam(CDC& dc);
protected:
	CFormTeam	m_FormTeamTeam;
};

#ifndef _DEBUG  // debug version in DrawRectView.cpp
inline CDrawRectDoc* CDrawRectView::GetDocument()
   { return (CDrawRectDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWRECTVIEW_H__9DBD9A60_9DCC_4BAC_9472_A4BB48F99695__INCLUDED_)
