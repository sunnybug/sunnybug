// MyRand.h: interface for the CMyRand class.
// paled, 2006-2-15
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYRAND_H__83AC6CF4_7E0C_4243_BD22_361C0A3FB14A__INCLUDED_)
#define AFX_MYRAND_H__83AC6CF4_7E0C_4243_BD22_361C0A3FB14A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mycom.h"

const int MY_RAND_MAX = 32767;
class CMyRand  
{
	COM_STYLE(CMyRand)
public:
	CMyRand();
	virtual ~CMyRand();

public:
	void	SetSeed(int nSeed);
	DWORD	GetSeed()			{ return m_dwLastRand; }
	int		GetRand();
	int		GetRand(int nRange)	{ if(nRange>0) return GetRand() % nRange; else return 0; }

protected:
	DWORD	m_dwLastRand;
};

#endif // !defined(AFX_MYRAND_H__83AC6CF4_7E0C_4243_BD22_361C0A3FB14A__INCLUDED_)
