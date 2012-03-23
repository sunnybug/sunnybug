// MyCriticalSection.h: interface for the CMyCriticalSection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYCRITICALSECTION_H__DE6404E3_AF66_4899_BFB1_CFF2D11922C0__INCLUDED_)
#define AFX_MYCRITICALSECTION_H__DE6404E3_AF66_4899_BFB1_CFF2D11922C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
class CMyCriticalSection  
{
public:
	virtual void Unlock();
	virtual void Lock();

	CMyCriticalSection();
	virtual ~CMyCriticalSection();
private:

	CRITICAL_SECTION m_CriticalSection;
};

#endif // !defined(AFX_MYCRITICALSECTION_H__DE6404E3_AF66_4899_BFB1_CFF2D11922C0__INCLUDED_)
