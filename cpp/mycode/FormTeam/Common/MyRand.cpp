// MyRand.cpp: implementation of the CMyRand class.
//
//////////////////////////////////////////////////////////////////////

#include <limits.h>
#include "MyRand.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMyRand::CMyRand()
{
	m_dwLastRand	= 0;
}

CMyRand::~CMyRand()
{

}

void CMyRand::SetSeed(int nSeed)
{
	m_dwLastRand = nSeed;
}

int CMyRand::GetRand()
{
	int a = 1103515245;
	int b = 12345;
	m_dwLastRand = ((unsigned __int64)m_dwLastRand * a + b) % ((unsigned __int64)UINT_MAX + 1);
	return ((unsigned __int64)m_dwLastRand / (1<<16) ) % (MY_RAND_MAX+1);
}
