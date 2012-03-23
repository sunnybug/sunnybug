 ////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		PreAllocator.cpp
//	Author:
//		Chun-Hyok, Chong.
//	Description:
//		The module that allocates the specific sized memory before using it.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "preallocator.h"
#include "safelocker.h"

#define DWORD_FULL								0xFFFFFFFF

void OPreAllocator::Init()
{
	m_pMem = 0;
	m_UnitPageSize = 0;
	m_NumberOfPages = 0;
	m_FreePageIndex = 0;
	m_pPageInfo = 0;
	m_AllocatedPageCount = 0;
	InterlockedExchange((volatile long *)&m_State, 0);

	m_SerialGenerator = 0;

	InitializeCriticalSection(&m_CSPageInfo);

	return;
} // Init()

void OPreAllocator::DeInit()
{
	if (0 != m_pPageInfo)
	{
		VirtualFree(m_pPageInfo, 0, MEM_RELEASE);
		m_pPageInfo = 0;
	}
	if (0 != m_pMem)
	{
		VirtualFree(m_pMem, 0, MEM_RELEASE);
		m_pMem = 0;
	}
	m_UnitPageSize = 0;
	m_NumberOfPages = 0;
	m_FreePageIndex = 0;
	m_AllocatedPageCount = 0;

	DeleteCriticalSection(&m_CSPageInfo);
	
	InterlockedExchange((volatile long *)&m_State, 0);
	
	return;
} // DeInit()

BOOL OPreAllocator::PreAllocate(DWORD UnitPageSize, DWORD NumberOfPages)
{
	DWORD Loop;

	if (0 == UnitPageSize || 0 == NumberOfPages)
	{
		return 0;
	}

	if (0 != m_pMem)
	{
		VirtualFree(m_pMem, 0, MEM_RELEASE);
		m_pMem = 0;
	}

	m_UnitPageSize = UnitPageSize;
	m_NumberOfPages = NumberOfPages;
	m_AllocatedPageCount = 0;

	m_pPageInfo = (OPageInfo *)VirtualAlloc(0, m_NumberOfPages * sizeof (OPageInfo), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (0 == m_pPageInfo)
	{
		return 0;
	}
	for (Loop = 0; Loop < m_NumberOfPages; Loop++)
	{
		(m_pPageInfo + Loop)->bUsed = 0;
		(m_pPageInfo + Loop)->Serial = 0;
	}

	m_pMem = (void *)VirtualAlloc(0, m_UnitPageSize * m_NumberOfPages, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (0 == m_pMem)
	{
		VirtualFree(m_pPageInfo, 0, MEM_RELEASE);
		m_pPageInfo = 0;

		return 0;
	}
		
	InterlockedExchange((volatile long *)&m_State, 1);

	return 1;
} // PreAllocator()

BOOL OPreAllocator::FreePreAllocatedMemory()
{
	if (0 == InterlockedExchange((volatile long *)&m_State, m_State) || 0 == m_pPageInfo || 0 == m_pMem)
	{
		return 0;
	}

	if (0 != m_pPageInfo)
	{
		VirtualFree(m_pPageInfo, 0, MEM_RELEASE);
		m_pPageInfo = 0;
	}
	if (0 != m_pMem)
	{
		VirtualFree(m_pMem, 0, MEM_RELEASE);
		m_pMem = 0;
	}
	m_UnitPageSize = 0;
	m_NumberOfPages = 0;
	m_FreePageIndex = 0;
	m_AllocatedPageCount = 0;

	InterlockedExchange((volatile long *)&m_State, 0);
	
	return 1;
} // FreePreAllocatedMemory()

void *OPreAllocator::Allocate(DWORD SizeInByte)
{
	DWORD PageCount;
	DWORD LoopTotal;
	DWORD LoopInner;
	BOOL bSettable;
	OSafeLocker SL(&m_CSPageInfo);
	void *pResult;
	DWORD Serial;

	if (0 == SizeInByte) // very important checking!
	{
		return 0;
	}

	if (0 == InterlockedExchange((volatile long *)&m_State, m_State))
	{
		return 0;
	}

	pResult = 0;
	
	PageCount = SizeInByte / m_UnitPageSize;
	if (0 < SizeInByte % m_UnitPageSize)
	{
		PageCount++;
	}

	// The shape of the memory area is not ring, don't confuse.
	SL.Lock();
	LoopTotal = 0;
	while (LoopTotal < (m_NumberOfPages + 1)) // +1 : when 1 page count, 1st allocate -> 2nd allocate. think condition (PageCount <= m_NumberOfPages - m_FreePageIndex).
	{
		if ((PageCount <= m_NumberOfPages - m_FreePageIndex) && (0 == (m_pPageInfo + m_FreePageIndex)->bUsed))
		{
			bSettable = 1;
			for (LoopInner = 0; LoopInner < PageCount; LoopInner++)
			{
				if (1 == (m_pPageInfo + m_FreePageIndex + LoopInner)->bUsed)
				{
					bSettable = 0;
					break;
				}
			}

			if (0 == bSettable)
			{
				m_FreePageIndex += LoopInner; // more 1 added at below.
				LoopTotal += LoopInner; // more 1 added at below.
			}
			else // if (1 == bSettable)
			{
				Serial = GetSerial();

				for (LoopInner = 0; LoopInner < PageCount; LoopInner++)
				{
					(m_pPageInfo + m_FreePageIndex + LoopInner)->bUsed = 1;
					(m_pPageInfo + m_FreePageIndex + LoopInner)->Serial = Serial;
					m_AllocatedPageCount++;
				}

				pResult = (void *)(((char *)m_pMem) + (m_UnitPageSize * m_FreePageIndex));
				m_FreePageIndex += PageCount;
				ZeroMemory(pResult, m_UnitPageSize * PageCount);
				break;
			}
		}

		m_FreePageIndex++;
		if (m_NumberOfPages <= m_FreePageIndex)
		{
			m_FreePageIndex = 0;
		}
		LoopTotal++;
	}
	SL.Unlock();
	
	return pResult;
} // Allocate()

BOOL OPreAllocator::Free(void *pPointer)
{
	DWORD Loop;
	DWORD PageIndex;
	OSafeLocker SL(&m_CSPageInfo);
	DWORD Serial;

	if (0 == pPointer || 0 == InterlockedExchange((volatile long *)&m_State, m_State))
	{
		return 0;
	}

	PageIndex = (DWORD)(((char *)pPointer - (char *)m_pMem) / m_UnitPageSize);

	if (0 != ((char *)pPointer - (char *)m_pMem) % m_UnitPageSize)
	{
		return 0;
	}
	
	SL.Lock();
	Loop = PageIndex;
	Serial = (m_pPageInfo + Loop)->Serial;
	while (m_NumberOfPages > Loop)
	{
		if (Serial != (m_pPageInfo + Loop)->Serial)
		{
			break;
		}

		(m_pPageInfo + Loop)->bUsed = 0;
		(m_pPageInfo + Loop)->Serial = 0;

		Loop++;
		m_AllocatedPageCount--;
	}
	SL.Unlock();

	return 1;
} // Free()

DWORD OPreAllocator::GetUnitPageSize()
{
	return m_UnitPageSize;
} // GetUnitPageSize()

DWORD OPreAllocator::GetNumberOfPages()
{
	return m_NumberOfPages;
} // GetNumberOfPages()

DWORD OPreAllocator::GetSerial()
{
	DWORD Result;

	Result = InterlockedIncrement((long *)&m_SerialGenerator);

	return Result;
} // GetSerial()

DWORD OPreAllocator::GetUsedMemorySizeInByte()
{
	OSafeLocker SL(&m_CSPageInfo);
	DWORD ReturnValue;

	ReturnValue = 0;
	SL.Lock();
	ReturnValue = m_AllocatedPageCount * m_UnitPageSize;
	SL.Unlock();

	return ReturnValue;
} // GetUsedMemorySizeInByte()

void *OPreAllocator::GetAllocPoint()
{
	return m_pMem;
} // GetAllocPoint()