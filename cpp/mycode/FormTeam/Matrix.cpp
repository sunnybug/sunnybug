// Matrix.cpp: implementation of the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Matrix.h"

//////////////////////////////////////////////////////////////////////////
CMatrix::CMatrix()
:m_pDrawMatrix(NULL)
,m_sz(0, 0)
{
	LoadMatrixFile();
}

CMatrix::~CMatrix()
{
	SaveMatrixFile();
}

bool CMatrix::Init(CWnd* pDrawMatrix)
{
	m_pDrawMatrix = pDrawMatrix;

	CRect rect;
    m_pDrawMatrix->GetClientRect(&rect);
	if(rect == m_LastWndRect)
		return true;
	m_LastWndRect = rect;

	//计算格子数
	const int nCellXCount = rect.Width()/ MATRIX_WIDTH;
	const int nCellYCount = rect.Height()/MATRIX_HEIGHT;

	InitMatrix(nCellXCount, nCellYCount);

	return true;
}

bool CMatrix::InitMatrix( const int nCellXCount, const int nCellYCount )
{
	CHECKF(nCellXCount > 0);
	CHECKF(nCellYCount > 0);

	m_sz.cx = nCellXCount;
	m_sz.cy = nCellYCount;

	set_rows_count(nCellXCount);
	set_cols_count(nCellYCount);
	
	m_martrix.resize(nCellXCount);
	OBJPOS ptCell;
	
	for (Matrix::iterator itRow=m_martrix.begin(); itRow!=m_martrix.end(); ++itRow)
	{
		itRow->resize(nCellYCount);
		ptCell.y = 0;
		for (MatrixRow::iterator itCell=itRow->begin(); itCell!=itRow->end(); ++itCell)
		{
			itCell->pt = ptCell;
			++ptCell.y;
		}
		++ptCell.x;
	}

	return true;
}

bool CMatrix::SetMask( const CPoint& ptCell, const bool bMask/*=true*/ )
{
	MatrixCell* pCell = GetCellPtr(ptCell);
	CHECKF(pCell);

	pCell->bMask = bMask;

	//只重绘改变的格子
	CRect rect;
	CalcClientRect(rect, pCell->pt);
	InvalidateRect(m_pDrawMatrix->GetSafeHwnd(), &rect, false);

	return true;
}

bool CMatrix::IsMask( const OBJPOS& pt )
{
	MatrixCell* pCell = GetCellPtr(pt);
	CHECKF(pCell);
	
	return pCell->bMask;
}

CMatrix::MatrixCell* CMatrix::GetCellPtr( const CPoint& pt )
{
	CHECKF(pt.x>=0 && pt.y>=0);
	if(m_martrix.size() < pt.x+1)
		return NULL;
	
	MatrixRow& row = m_martrix[pt.x];
	if(row.size() < pt.y+1)
		return NULL;
	
	return &row[pt.y];
}

const CMatrix::MatrixCell* CMatrix::GetCell( const CPoint& pt )
{
	return GetCellPtr(pt);
}

bool CMatrix::Draw( CDC& dc )
{
	DrawBoard(dc);

	for (Matrix::iterator itRow=m_martrix.begin(); itRow!=m_martrix.end(); ++itRow)
	{
		for (MatrixRow::iterator itCell=itRow->begin(); itCell!=itRow->end(); ++itCell)
		{
			DrawMatrix(dc, *itCell);
		}
	}
	
	return true;
}

void CMatrix::DrawBoard(CDC& dc)
{
	CRect rect;
    m_pDrawMatrix->GetClientRect(&rect);
	
	//计算格子数
	const int nCellXCount = rect.Width()/ MATRIX_WIDTH;
	const int nCellYCount = rect.Width()/MATRIX_HEIGHT;
	
	//画背景
	CBrush brush(dc.GetBkColor());
	dc.FillRect(rect,&brush);
	
	//定义画线的画笔并选中
	CPen pen(PS_SOLID, 1, RGB(0,0,0));
	CPen *pOldPen = dc.SelectObject(&pen);
	
	//画X线
	LONG lXIndex =  MATRIX_WIDTH;
	while (lXIndex < rect.Width())
	{
		dc.MoveTo(lXIndex, 0);
		dc.LineTo(lXIndex, rect.Height());
		lXIndex +=  MATRIX_WIDTH;
	}
	
	//画Y线
	LONG lYIndex = MATRIX_HEIGHT;
	while (lYIndex < rect.Height())
	{
		dc.MoveTo(0, lYIndex);
		dc.LineTo(rect.Width(), lYIndex);
		lYIndex += MATRIX_HEIGHT;
	}
	
	if(pOldPen)
		dc.SelectObject(pOldPen);
}

bool CMatrix::DrawMatrix(CDC& dc, const MatrixCell& stInfo)
{
	CRect rect;
	CalcClientRect(rect, stInfo.pt);
	
	CBrush brush;
	if (!stInfo.bMask)
	{
	//	brush.CreateSolidBrush(dc.GetBkColor());
	//	dc.FillRect(rect, &brush);
	}
	else
	{
		brush.CreateSolidBrush(COLOR_MASK);
		dc.FillRect(rect, &brush);
	}
	
	return true;	
}

bool CMatrix::CalcClientRect(RECT& rect, const OBJPOS& ptMatrixPos)
{
	rect.left	= ptMatrixPos.x * MATRIX_WIDTH;
	rect.right	= (ptMatrixPos.x+1) * MATRIX_WIDTH;
	rect.top	= ptMatrixPos.y * MATRIX_HEIGHT;
	rect.bottom	= (ptMatrixPos.y + 1) * MATRIX_HEIGHT;
	return true;
}

bool CMatrix::LoadMatrixFile()
{
	FILE* pFile = NULL;
	pFile = fopen(FILE_MATRIX, "r");
	if(!pFile)
		return false;

	/* Set pointer to beginning of file: */
	fseek( pFile, 0L, SEEK_SET );
	
	int nWidth = 0;
	int nHeight = 0;
	fscanf(pFile, "%d %d", &nWidth, &nHeight);
	IF_OK (nWidth>0 && nHeight>0)
	{
		InitMatrix(nWidth, nHeight);

		int nMask = 0;
		CPoint pt(0, 0);
		while (fscanf(pFile, "%d %d %d", &pt.x, &pt.y, &nMask))
		{
			MatrixCell* pCell = GetCellPtr(pt);
			IF_OK(pCell)
			{
				pCell->bMask = (nMask!=0);
			}
		
			if (pt.x+1>=nWidth && pt.y+1>=nHeight)
			{
				break;
			}
		}
	}
	
	fclose(pFile);
	return true;
}


bool CMatrix::SaveMatrixFile()
{
	FILE* pFile = NULL;
	pFile = fopen(FILE_MATRIX, "w+");
	if(!pFile)
		return false;

	if(m_martrix.empty())
		return false;
	
	TCHAR acBuf[256] = {0};
	fprintf(pFile, "%d %d\r\n", m_martrix.size(), m_martrix[0].size());

	for (Matrix::iterator itRow=m_martrix.begin(); itRow!=m_martrix.end(); ++itRow)
	{
		for (MatrixRow::iterator itCell=itRow->begin(); itCell!=itRow->end(); ++itCell)
		{
			fprintf(pFile, "%d %d %d\r\n", itCell->pt.x, itCell->pt.y, itCell->bMask);
		}
	}
	
	fclose(pFile);
	return true;
}

CMatrix* CMatrix::Instance()
{
	static CMatrix obj;
	return &obj;
}
