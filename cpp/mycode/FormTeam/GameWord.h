// GameWord.h: interface for the CGameWord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEWORD_H__DE500D17_D6B9_4FCE_8072_18F8E4F71F92__INCLUDED_)
#define AFX_GAMEWORD_H__DE500D17_D6B9_4FCE_8072_18F8E4F71F92__INCLUDED_

#pragma once

#include "autolink.h"

#ifdef _NOGAME
void Cell2Pix(const OBJPOS& cell, Pix& pix);
void Pix2Cell(const Pix& pix, OBJPOS& cell);
bool IsMaskCell(const OBJPOS& ptCell);	//是否有掩码
#endif // _NOGAME

int CalcDeltaX(int nDir);
int CalcDeltaY(int nDir);
bool CalcDelta(OBJPOS& pt, int nDir);

class CNpc
{
public:
	OBJID	m_id;
	CNpc():m_nDir(MAX_DIRSIZE),m_id(0) { m_link.Init(this);}
public:
	void	SetPos(const int nX, const int nY, const bool bSync=false) { m_ptCell.x = nX; m_ptCell.y = nY;}
	int		GetPosX() { return m_ptCell.x;}
	int		GetPosY() { return m_ptCell.y;}
	int		GetDir() { return m_nDir;}
	OBJID	GetID()	{ return m_id;}

	const AUTOLINK_NOTE* QueryLink() const	{ return m_link.QueryNote(); }
	
	//test
	const OBJPOS	GetPos() { return m_ptCell;}
	const Pix		GetPix() { Pix pix;Cell2Pix(m_ptCell, pix); return pix;}
protected:
	OBJPOS	m_ptCell;		//在地图上的坐标
	int		m_nDir;
	CAutoLink<CNpc>	m_link;
};

#endif // !defined(AFX_GAMEWORD_H__DE500D17_D6B9_4FCE_8072_18F8E4F71F92__INCLUDED_)
