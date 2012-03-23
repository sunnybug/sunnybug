// GameTimer.h: interface for the CGameTimer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMETIMER_H__059AED43_D21C_4072_A172_20A7D8B6DF52__INCLUDED_)
#define AFX_GAMETIMER_H__059AED43_D21C_4072_A172_20A7D8B6DF52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>
#include "BaseFunc.h"

//////////////////////////////////////////////////////////////////////
// 时钟类，每天、周、月触发一次的时钟。
// 格式：MMWWHHMMSS，MM不为0表示每月触发一次，WW不为0表示每周触发一次，否则为每天触发一次。SS通常为0。
template<int I>
class CGameTimer_
{
public:
	CGameTimer_(int nFormat = I) : m_nFormat(nFormat), m_bActive(false)	{}
//	virtual ~CGameTimer_() {}

public:
	void	SetTimer(int nFormat)			{ m_nFormat = nFormat; }
	// 时钟是否触发(只触发一次)，与SetTimer配合使用，可实现只触发一次的时钟。
	bool	ActiveTimer	()					{ return ActiveTimer(m_nFormat); }
	// 时钟是否触发(只触发一次)，可实现只触发一次的时钟。
	bool	ActiveTimer	(int nFormat)		{ if(!::IsActiveTime(time(NULL), nFormat)) return (m_bActive=false),false; if(!m_bActive) return (m_bActive=true),true; else return false; }

public: // get
	int		GetTimeStamp()					{ return m_nFormat; }

protected:
	int		m_nFormat;
	bool	m_bActive;
};
typedef	CGameTimer_<246060>	CGameTimer;		// =246060 : 用于数组初始化，不可能的时钟


#endif // !defined(AFX_GAMETIMER_H__059AED43_D21C_4072_A172_20A7D8B6DF52__INCLUDED_)
