////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		SafeLocker.h
//	Author:
//		Chun-Hyok, Chong.
//	Description:
//		It is a safe locker for the critical section used to be forgotten to release.
//		It is a just lock count checker, so the critical section must be initialized before using.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SAFELOCKER_H_
#define __SAFELOCKER_H_

#include "workframe.h"
#include "workframereturncode.h"

class OSafeLocker
{
private:
	CRITICAL_SECTION *m_pCriticalSection;
	BOOL m_bLockedLocal;

public:
	OSafeLocker(CRITICAL_SECTION *pCS)
	{
		Init(pCS);
	}
	~OSafeLocker()
	{
		DeInit();
	}

	void Init(CRITICAL_SECTION *pCS);
	void DeInit();
	int Lock();
	int Unlock();
};

#endif // __SAFELOCKER_H_