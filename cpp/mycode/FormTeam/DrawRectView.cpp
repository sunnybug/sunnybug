// DrawRectView.cpp : implementation of the CDrawRectView class
//

#include "stdafx.h"
#include "DrawRect.h"

#include "DrawRectDoc.h"
#include "DrawRectView.h"
#include "pathfind.h"

/////////////////////////////////////////////////////////////////////////////
// CDrawRectView

IMPLEMENT_DYNCREATE(CDrawRectView, CView)

BEGIN_MESSAGE_MAP(CDrawRectView, CView)
	//{{AFX_MSG_MAP(CDrawRectView)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawRectView construction/destruction

CDrawRectView::CDrawRectView()
:m_ptStart(-1, -1)
,m_ptEnd(-1, -1)
{
	// TODO: add construction code here
// 	Pix pix;
// 	pix.x = 47;
// 	pix.y = 17;
// 	OBJPOS pos;
// 	Pix2Cell(pix, pos);
}

CDrawRectView::~CDrawRectView()
{
}

BOOL CDrawRectView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDrawRectView drawing
void CDrawRectView::OnDraw(CDC* pDC)
{
	CDrawRectDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	
	CMatrix::Instance()->Init(this);
	
	//画矩阵中的格子状态
	CMatrix::Instance()->Draw(m_MemDC);
	DrawPath(m_MemDC);
	DrawTeam(m_MemDC);

	CRect rc;
	GetClientRect(&rc);
	pDC->BitBlt(0,0,rc.Width(),rc.Height(), &m_MemDC,0,0,SRCCOPY);
}

/////////////////////////////////////////////////////////////////////////////
// CDrawRectView printing

BOOL CDrawRectView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDrawRectView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDrawRectView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDrawRectView diagnostics

#ifdef _DEBUG
void CDrawRectView::AssertValid() const
{
	CView::AssertValid();
}

void CDrawRectView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDrawRectDoc* CDrawRectView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDrawRectDoc)));
	return (CDrawRectDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDrawRectView message handlers

void CDrawRectView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CStatusBar* pStatusBar = (CStatusBar*)AfxGetMainWnd()->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	ASSERT(pStatusBar);
	if (pStatusBar)
	{
		OBJPOS ptMatrixPos;
		GetCurMatrixPos(ptMatrixPos);
		bool bInSegment = ::PointIsOnSegment(CPoint(ptMatrixPos.x, ptMatrixPos.y), m_ptStart, m_ptEnd);

		CString str;
		str.Format("Mode[%s] Matrix[%d,%d] Pix[%d,%d] PathFinder[%d,%d] Dir[%d°]\t\t[%dms][%d,%d]->[%d,%d][是否在线段:%d]", 
			GetModeTxt(),
			ptMatrixPos.x, ptMatrixPos.y, point.x, point.y,
			m_FormTeamTeam.GetPathFinderIdx().x,
			m_FormTeamTeam.GetPathFinderIdx().y,
			m_FormTeamTeam.GetDir(),
			m_Tick.GetUsedMS(),
			m_ptStart.x, m_ptStart.y, m_ptEnd.x, m_ptEnd.y, bInSegment
			);
		pStatusBar->SetWindowText(str);
	}

	CView::OnMouseMove(nFlags, point);
}

void CDrawRectView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	OBJPOS ptCell;
	GetCurMatrixPos(ptCell);

	CMatrix::Instance()->SetMask(ptCell);

	CView::OnLButtonUp(nFlags, point);
}

void CDrawRectView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	OBJPOS ptCell;
	GetCurMatrixPos(ptCell);

	CMatrix::Instance()->SetMask(ptCell, false);

	CView::OnRButtonUp(nFlags, point);
}

void CDrawRectView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	OBJPOS ptCell;
	GetCurMatrixPos(ptCell);

	if (nChar == 'S')
	{
		SetStart(ptCell);
		DrawPath(m_MemDC);
	}
	else if (nChar == 'E')
	{
		SetEnd(ptCell);
		DrawPath(m_MemDC);
	}
	else if (nChar == 'G')
	{
		switch (GetMode())
		{
		case DRAWMODE_FORMTEAM:
			{
				m_FormTeamTeam.SetTarget(ptCell);
				m_ptStart = m_FormTeamTeam.GetFinderPos();
				m_ptEnd = ptCell;
			}
			break;
		case DRAWMODE_ASTART:
			{
				m_ptEnd = ptCell;
				CPathFinder::FindPath(m_lstPath, m_ptStart, m_ptEnd);
			}
			break;
		case DRAWMODE_CALC_PATHNODE:
			{
				m_Tick.Start();
				for(int i=0; i<100000; ++i)
				{
					CPathFinder::GetCrossPoint(OBJLINE(m_ptStart, m_ptEnd), m_lstPath);
					//::PointIsOnSegment(CPoint(ptCell.x, ptCell.y), m_ptStart, m_ptEnd);
				}
				m_Tick.Finish();
			}
			break;
		}
	}
	else if (nChar == 'N')
	{
		if(GetMode() == DRAWMODE_FORMTEAM)
			m_FormTeamTeam.OnTimer();
	}
	Invalidate(false);

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

int CDrawRectView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDC* pDC = GetDC();
	IF_OK(pDC)
	{
		const int x=GetSystemMetrics(SM_CXSCREEN);
		const int y=GetSystemMetrics(SM_CYSCREEN);

		m_MemDC.CreateCompatibleDC(pDC);
		m_MemBitmap.CreateCompatibleBitmap(pDC, x, y);
		m_MemDC.SelectObject(&m_MemBitmap);

		CBrush brush(COLOR_BOARD);
		m_MemDC.FillRect(CRect(0,0,x,y),&brush);    //设置客户区背景

		ReleaseDC(pDC);
	}

	m_FormTeamTeam.Create();
//	SetTimer(TIMERID_FORMTEAM, 10, NULL);
	
	return 0;
}

bool CDrawRectView::GetCurMatrixPos(OBJPOS& ptMatrixPos)
{
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);//转换成客户端坐标系
	
	IF_NOT(CalcMatrixPos(ptMatrixPos, ptCursor))
		return false;
	return true;	
}

bool CDrawRectView::CalcMatrixPos(OBJPOS& ptMatrixPos, const POINT& pos)
{
	ptMatrixPos.x = pos.x/MATRIX_WIDTH;
	ptMatrixPos.y = pos.y/MATRIX_HEIGHT;

	CRect rtClient;
	GetClientRect(&rtClient);

	ptMatrixPos.x = max(ptMatrixPos.x, 0);
	ptMatrixPos.x = min(ptMatrixPos.x, rtClient.Width()/MATRIX_WIDTH-1);

	ptMatrixPos.y = max(ptMatrixPos.y, 0);
	ptMatrixPos.y = min(ptMatrixPos.y, rtClient.Height()/MATRIX_HEIGHT-1);
	return true;	
}


void CDrawRectView::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent)
	{
	case 0:
		{
		}
		break;
	default:
		ASSERT(false);
		break;
	}
	
	CView::OnTimer(nIDEvent);
}

void CDrawRectView::DrawPath(CDC& dc)
{
	OBJPOS ptStart;
	if (GetStart(ptStart))
	{
		CRect rtStart;
		CMatrix::Instance()->CalcClientRect(rtStart, ptStart);
		dc.FillRect(rtStart, &CBrush(COLOR_START));
	}
	
	OBJPOS ptEnd;
	if (GetEnd(ptEnd))
	{
		CRect rtEnd;
		CMatrix::Instance()->CalcClientRect(rtEnd, ptEnd);
		dc.FillRect(rtEnd, &CBrush(COLOR_END));
	}
	
	if (!m_lstPath.empty())
	{
		CRect rt;
		for (PathList::const_iterator itPath=m_lstPath.begin(); itPath!=m_lstPath.end(); ++itPath)
		{
			CMatrix::Instance()->CalcClientRect(rt, *itPath);
			dc.FillRect(rt, &CBrush(COLOR_PATH));
		}
	}
}

BOOL CDrawRectView::GetStart( CPoint& pt )
{
	pt = m_ptStart;
	return (m_ptStart != POINT_NONE);
}

BOOL CDrawRectView::GetEnd( CPoint& pt )
{
	pt = m_ptEnd;
	return (m_ptEnd != POINT_NONE);
}

void CDrawRectView::SetStart( const CPoint& ptStart )
{
	m_ptStart = ptStart;
}

void CDrawRectView::SetEnd( const CPoint& ptEnd )
{
	m_ptEnd = ptEnd;
}

void CDrawRectView::DrawTeam( CDC& dc )
{
	CFormTeam::CFormTeamMemberSet* pSet = m_FormTeamTeam.QueryMemberSet();
	CFormTeam::CFormTeamMemberSet::Iterator it = pSet->NewEnum();
	while(it.Next())
	{
		CFormTeamMember* pNpc = it;
		IF_NOT(pNpc)
			continue;

		CRect rt;
		CMatrix::Instance()->CalcClientRect(rt, pNpc->QueryNpc()->GetPos());

		CString str;
		str.Format("%u", pNpc->QueryNpc()->GetID());

		dc.DrawText(str, &rt, DT_VCENTER|DT_CENTER);
	}
}
