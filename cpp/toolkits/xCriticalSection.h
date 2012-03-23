#if !defined(_XCIRITCALSECTION_H_)
#define _XCIRITCALSECTION_H_
#pragma once

#include <windows.h>

class xCriticalSection  
{
public:
	void Unlock();
	void Lock();

	xCriticalSection();
	~xCriticalSection();
private:
	xCriticalSection(const xCriticalSection &);
	void operator=(const xCriticalSection &);
private:

	CRITICAL_SECTION m_CriticalSection;
};

xCriticalSection::xCriticalSection()
{
	InitializeCriticalSection(&m_CriticalSection);
}

xCriticalSection::~xCriticalSection()
{
	DeleteCriticalSection(&m_CriticalSection);
}
void xCriticalSection::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
}

void xCriticalSection::Unlock()
{
	LeaveCriticalSection(&m_CriticalSection);
}


#endif
