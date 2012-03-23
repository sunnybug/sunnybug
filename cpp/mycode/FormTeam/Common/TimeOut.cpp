// TimeOut.cpp: implementation of the CTimeOut class.
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "TimeOut.h"

//////////////////////////////////////////////////////////////////////
// CTimeOut
//////////////////////////////////////////////////////////////////////
// 带时间积累的定时时钟
bool CTimeOut::ToNextTick(int nSecs)
{
	if(IsTimeOut(nSecs))
	{
		if(time(NULL) >= m_tUpdateTime + nSecs*2)
			return Update(),true;
		else
			return (m_tUpdateTime+=nSecs),true;
	}
	else
		return false;
}
//////////////////////////////////////////////////////////////////////
bool CTimeOut::MakeTimeOut()
{
	m_tUpdateTime = (clock()/1000);
	if(m_tUpdateTime > m_nInterval)
	{
	   m_tUpdateTime -= m_nInterval;
	   return true;
	}
	else
	{
		m_tUpdateTime = 1;		
	}
	return false;
}	
//////////////////////////////////////////////////////////////////////
// CTimeOutMS
//////////////////////////////////////////////////////////////////////
// 带时间积累的定时时钟
bool CTimeOutMS::ToNextTick(int nMilliSecs)
{
	if(IsTimeOut(nMilliSecs))
	{
		if(clock() >= m_tUpdateTime + nMilliSecs*2)
			return Update(),true;
		else
			return (m_tUpdateTime+=nMilliSecs),true;
	}
	else
		return false;
}
//////////////////////////////////////////////////////////////////////
bool CTimeOutMS::MakeTimeOut()
{
	m_tUpdateTime = (clock()/1000);
	if(m_tUpdateTime > m_nInterval)
	{
		m_tUpdateTime -= m_nInterval;
		return true;
	}
	else
	{
		m_tUpdateTime = 1;
	}
	return false;
}	

