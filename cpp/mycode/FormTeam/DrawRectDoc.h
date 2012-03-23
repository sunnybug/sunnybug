// DrawRectDoc.h : interface of the CDrawRectDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWRECTDOC_H__E9FE7F7E_AB2E_4BD2_BE4D_D6875802800F__INCLUDED_)
#define AFX_DRAWRECTDOC_H__E9FE7F7E_AB2E_4BD2_BE4D_D6875802800F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDrawRectDoc : public CDocument
{
protected: // create from serialization only
	CDrawRectDoc();
	DECLARE_DYNCREATE(CDrawRectDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawRectDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDrawRectDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDrawRectDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWRECTDOC_H__E9FE7F7E_AB2E_4BD2_BE4D_D6875802800F__INCLUDED_)
