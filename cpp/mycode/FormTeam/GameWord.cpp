// GameWord.cpp: implementation of the CGameWord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameWord.h"

#ifdef _NOGAME
#include "pathfind.h"
#include "Matrix.h"
#endif // _NOGAME

#ifdef _NOGAME
void Cell2Pix( const OBJPOS& cell, Pix& pix )
{
//	CHECK(cell.x>=0 && cell.y>=0);
	if (cell.x > 0)
		pix.x = cell.x * MATRIX_WIDTH + MATRIX_WIDTH/2;
	else
		pix.x = cell.x * MATRIX_WIDTH - MATRIX_WIDTH/2;

	if (cell.y > 0)
		pix.y = cell.y * MATRIX_HEIGHT + MATRIX_HEIGHT/2;
	else
		pix.y = cell.y * MATRIX_HEIGHT - MATRIX_HEIGHT/2;
}

void Pix2Cell( const Pix& pix, OBJPOS& cell )
{
// 	const int nX = pix.x/MATRIX_WIDTH;
// 	cell.x = nX + (pix.x%MATRIX_WIDTH) ? 1 : 0;
// 
// 	const int nY = pix.y/MATRIX_HEIGHT;
// 	cell.y = nY + (pix.y%MATRIX_HEIGHT ) ? 1 : 0;

// 	cell.x = pix.x / MATRIX_WIDTH;
// 	cell.y = pix.y / MATRIX_HEIGHT;

	cell.x = pix.x / MATRIX_WIDTH;
	cell.y = pix.y / MATRIX_HEIGHT;

//	CHECK(cell.x>=0 && cell.y>=0);
}
#endif // _NOGAME

bool CalcDelta(OBJPOS& pt, int nDir)
{
	pt.x += CalcDeltaX(nDir);
	pt.y += CalcDeltaY(nDir);
	return true;
}
int	CalcDeltaX(int nDir)
{
	ASSERT(nDir>=0 && nDir<=MAX_DIRSIZE);
	nDir = nDir % MAX_DIRSIZE;
	return _DELTA_X[nDir];
}

int	CalcDeltaY(int nDir)
{
	ASSERT(nDir>=0 && nDir<=MAX_DIRSIZE);
	nDir = nDir % MAX_DIRSIZE;
	return _DELTA_Y[nDir];
}

bool IsMaskCell(const OBJPOS& ptCell)
{
	return CMatrix::Instance()->IsMask(ptCell);
}
