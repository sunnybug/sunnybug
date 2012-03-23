// Matrix.h: interface for the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__2EF76F7B_A6BF_4C82_AC1B_38B6A689E3EB__INCLUDED_)
#define AFX_MATRIX_H__2EF76F7B_A6BF_4C82_AC1B_38B6A689E3EB__INCLUDED_

#pragma once

#include <vector>
#include "pathfind.h"

//////////////////////////////////////////////////////////////////////////

const COLORREF COLOR_BOARD	= RGB(255,255,255);
const COLORREF COLOR_MASK	= RGB(128,128,128);
const COLORREF COLOR_START	= RGB(199,251,255);
const COLORREF COLOR_PATH	= RGB(0,0,0);
const COLORREF COLOR_END	= RGB(255,255,0);	//yellow

const TCHAR* const FILE_MATRIX	= "matrix.bin";


//////////////////////////////////////////////////////////////////////////
class CMatrix  
{
public:
	struct MatrixCell
	{
		bool bMask;
		OBJPOS pt;
		MatrixCell():bMask(false){}
	};

public:
	static CMatrix* Instance();
	CMatrix();
	~CMatrix();

	//////////////////////////////////////////////////////////////////////////
	//data
public:
	bool	Init(CWnd* pDrawMatrix);
	bool	SetMask(const CPoint& ptCell, const bool bMask=true);
	bool	IsMask(const OBJPOS& pt);
	const CSize& GetSize() const { return m_sz;}
	const MatrixCell* GetCell(const CPoint& pt);
protected:
	typedef std::vector<MatrixCell> MatrixRow;
	typedef std::vector<MatrixRow> Matrix;
	
protected:
	bool	InitMatrix( const int nCellXCount, const int nCellYCount );
	MatrixCell* GetCellPtr(const CPoint& pt);
protected:
	Matrix	m_martrix;
	CSize	m_sz;

	//////////////////////////////////////////////////////////////////////////
	//draw
public:
	//画矩阵
	bool	Draw(CDC& dc);
	//根据矩阵坐标，计算出对应的客户端区域
	bool	CalcClientRect(RECT& rect, const OBJPOS& ptMatrixPos);
protected:
	void	DrawBoard(CDC& dc);
	bool	DrawMatrix(CDC& dc, const MatrixCell& stInfo);
protected:
	CWnd*	m_pDrawMatrix;
	CRect	m_LastWndRect;

	//////////////////////////////////////////////////////////////////////////
	///config file
protected:
	bool	LoadMatrixFile();
	bool	SaveMatrixFile();
};


#endif // !defined(AFX_MATRIX_H__2EF76F7B_A6BF_4C82_AC1B_38B6A689E3EB__INCLUDED_)
