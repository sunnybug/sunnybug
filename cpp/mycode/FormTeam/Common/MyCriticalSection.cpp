// MyCriticalSection.cpp: implementation of the CMyCriticalSection class.
//
//////////////////////////////////////////////////////////////////////

#include "MyCriticalSection.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyCriticalSection::CMyCriticalSection()
{
	InitializeCriticalSection(&m_CriticalSection);
}

CMyCriticalSection::~CMyCriticalSection()
{
	DeleteCriticalSection(&m_CriticalSection);
}
void CMyCriticalSection::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
}

void CMyCriticalSection::Unlock()
{
	LeaveCriticalSection(&m_CriticalSection);
}
