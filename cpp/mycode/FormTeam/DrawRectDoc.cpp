// DrawRectDoc.cpp : implementation of the CDrawRectDoc class
//

#include "stdafx.h"
#include "DrawRect.h"

#include "DrawRectDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawRectDoc

IMPLEMENT_DYNCREATE(CDrawRectDoc, CDocument)

BEGIN_MESSAGE_MAP(CDrawRectDoc, CDocument)
	//{{AFX_MSG_MAP(CDrawRectDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawRectDoc construction/destruction

CDrawRectDoc::CDrawRectDoc()
{
	// TODO: add one-time construction code here

}

CDrawRectDoc::~CDrawRectDoc()
{
}

BOOL CDrawRectDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDrawRectDoc serialization

void CDrawRectDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDrawRectDoc diagnostics

#ifdef _DEBUG
void CDrawRectDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDrawRectDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDrawRectDoc commands
