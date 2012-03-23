// LoopCount.cpp: implementation of the CLoopCount class.
//
//////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "LoopCount.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoopCount::CLoopCount()
{
	m_nFrom		= 0;
	m_nTo		= 99;
	m_nStart	= 0;
	m_nCurr		= 0;
	m_nCount	= 0;
	m_nLimit	= 99;
	m_bValid	= false;
	m_nLast		= 0;
}

CLoopCount::~CLoopCount()
{

}

bool CLoopCount::Init(int nFrom, int nTo, int nStart, int nAllCount/*=0*/)
{
	CHECKF(nFrom != nTo);

	m_nFrom		= nFrom;
	m_nTo		= nTo;
	m_nStart	= nStart;
	m_nCurr		= nStart;
	m_nCount	= 0;
	m_nLimit	= nAllCount;
	m_bValid	= true;
	m_nLast		= m_nCurr;

	return true;
}

int	 CLoopCount::Next()
{
	ASSERT(m_bValid);

	if(m_nCount < m_nLimit)
	{
		m_nCount++;
		if(m_nCount >= m_nLimit)
			m_bValid = false;
	}

	m_nLast = m_nCurr;

	if(m_nFrom < m_nTo)
	{
		m_nCurr++;
		if(m_nCurr > m_nTo)
			m_nCurr	= m_nFrom;
	}
	else
	{
		m_nCurr--;
		if(m_nCurr < m_nTo)
			m_nCurr	= m_nFrom;
	}

	return m_nLast;
}

void CLoopCount::SetPos(int nPos)
{
	ASSERT(m_bValid);
	CHECK(nPos >= __min(m_nFrom,m_nTo) && nPos <= __max(m_nFrom,m_nTo));

	if(m_nCount < m_nLimit)
	{
		m_nCount++;
		if(m_nCount >= m_nLimit)
			m_bValid = false;
	}

	m_nCurr = nPos;
	m_nLast = m_nCurr;

	if(m_nFrom < m_nTo)
	{
		m_nCurr++;
		if(m_nCurr > m_nTo)
			m_nCurr	= m_nFrom;
	}
	else
	{
		m_nCurr--;
		if(m_nCurr < m_nTo)
			m_nCurr	= m_nFrom;
	}
}

