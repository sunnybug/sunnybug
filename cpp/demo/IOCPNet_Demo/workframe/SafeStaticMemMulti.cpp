////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		SafeStaticMemMulti.cpp
//	Author:
//		Chun-Hyok, Chong.
//	Description:
//		The class that insures the release of the multiple used memory that allocated by PreAllocator.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SafeStaticMemMulti.h"

void OSafeStaticMemMulti::Init()
{
	InitializeCriticalSection(&m_CriSec);
	
	m_pPreAllocator = 0;
} // Init()

void OSafeStaticMemMulti::Init(OPreAllocator *pPreAllocator)
{
	InitializeCriticalSection(&m_CriSec);

	m_pPreAllocator = pPreAllocator;
	m_PAList.PreAllocate(sizeof (OPointer), m_pPreAllocator->GetNumberOfPages());
	// 'OLinkedListStatic<OPointer> m_AllocPointerList(&m_PAList);' is not available in the declaration of the class.
	m_AllocPointerList.SetPreAllocator(&m_PAList);
} // Init()

void OSafeStaticMemMulti::DeInit()
{
	OPointer *pEle;

	EnterCriticalSection(&m_CriSec);
	pEle = m_AllocPointerList.MoveFirst();
	while (0 != pEle)
	{
		if (0 != pEle->AllocPointer)
		{
			m_pPreAllocator->Free(pEle->AllocPointer);
		}
		m_AllocPointerList.SubtractCurrent();
		pEle = m_AllocPointerList.MoveFirst();
	}
	LeaveCriticalSection(&m_CriSec);

	m_pPreAllocator = 0;
	m_PAList.FreePreAllocatedMemory();

	DeleteCriticalSection(&m_CriSec);
} // DeInit()

BOOL OSafeStaticMemMulti::SetPreAllocator(OPreAllocator *pPreAllocator)
{
	m_pPreAllocator = pPreAllocator;
	m_PAList.PreAllocate(sizeof (OPointer), m_pPreAllocator->GetNumberOfPages());
	// 'OLinkedListStatic<OPointer> m_AllocPointerList(&m_PAList);' is not available in the declaration of the class.
	m_AllocPointerList.SetPreAllocator(&m_PAList);
	
	return 1;
} // SetPreAllocator()

void *OSafeStaticMemMulti::Allocate(DWORD Size)
{
	OPointer Pointer;
	
	Pointer.AllocPointer = (void *)m_pPreAllocator->Allocate(Size);
	if (0 == Pointer.AllocPointer)
	{
		return Pointer.AllocPointer;
	}
	EnterCriticalSection(&m_CriSec);
	m_AllocPointerList.Add(Pointer);
	LeaveCriticalSection(&m_CriSec);

	return Pointer.AllocPointer;
} // Allocate()

BOOL OSafeStaticMemMulti::Free(void *AllocPointer)
{
	OPointer *pEle;
	BOOL bFound;

	if (0 == AllocPointer)
	{
		return 0;
	}
	
	bFound = 0;
	EnterCriticalSection(&m_CriSec);
	pEle = m_AllocPointerList.MoveFirst();
	while (0 != pEle)
	{
		if (pEle->AllocPointer == AllocPointer)
		{
			m_pPreAllocator->Free(pEle->AllocPointer);
			m_AllocPointerList.SubtractCurrent();
			bFound = 1;
			break;
		}
		pEle = m_AllocPointerList.MoveNext();
	}
	LeaveCriticalSection(&m_CriSec);

	if (1 == bFound)
	{
		return 1;
	}
	
	return 0;
} // Free()

OPreAllocator *OSafeStaticMemMulti::GetPreAllocator()
{
	return m_pPreAllocator;
} // GetPreAllocator()