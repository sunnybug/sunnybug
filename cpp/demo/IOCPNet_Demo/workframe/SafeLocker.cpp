 ////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		SafeLocker.cpp
//	Author:
//		Chun-Hyok, Chong.
//	Description:
//		It is a safe locker for the critical section used to be forgotten to release.
//		It is a just lock count checker, so the critical section must be initialized before using.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SafeLocker.h"

// This class is not for global use. Use it in a local function.

void OSafeLocker::Init(CRITICAL_SECTION *pCS)
{
	m_pCriticalSection = pCS;
	m_bLockedLocal = 0;
} // Init()

void OSafeLocker::DeInit()
{
	if (1 == m_bLockedLocal)
	{
		LeaveCriticalSection(m_pCriticalSection);
		m_bLockedLocal = 0;
	}
} // DeInit()

int OSafeLocker::Lock()
{
	// Blocking is more helpful to fine a bug.

	EnterCriticalSection(m_pCriticalSection);
	m_bLockedLocal = 1;

	return RET_SUCCESS;
} // Lock()

int OSafeLocker::Unlock()
{
	if (0 == m_bLockedLocal)
	{
		return RET_FAIL;
	}

	LeaveCriticalSection(m_pCriticalSection);
	m_bLockedLocal = 0;

	return RET_SUCCESS;
} // Unlock()