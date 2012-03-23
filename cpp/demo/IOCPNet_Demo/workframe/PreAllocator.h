////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		PreAllocator.h
//	Author:
//		Chun-Hyok, Chong.
//	Description:
//		The module that allocates the specific sized memory before using it.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PREALLOCATOR_H__
#define __PREALLOCATOR_H__

#include "workframe.h"
#include "workframereturncode.h"
#include <windows.h>
#include <time.h>

struct OPageInfo
{
	BOOL bUsed;
	DWORD Serial;
};

class OPreAllocator
{
private:
	void *m_pMem;
	DWORD m_UnitPageSize;
	DWORD m_NumberOfPages;
	DWORD m_FreePageIndex;
	OPageInfo *m_pPageInfo;
	CRITICAL_SECTION m_CSPageInfo;
	DWORD m_SerialGenerator;
	DWORD m_AllocatedPageCount;

	DWORD m_State; // there may be the case that outer calls PreAllocator.FreePreAllocatedMemory() first and calls PreAllocator.Free() in outer's destructor second.

	DWORD GetSerial();

public:
	OPreAllocator()
	{
		Init();
	}
	~OPreAllocator()
	{
		DeInit();
	}

	void Init();
	void DeInit();
	BOOL PreAllocate(DWORD UnitPageSize, DWORD NumberOfPages);
	BOOL FreePreAllocatedMemory();
	void *Allocate(DWORD SizeInByte);
	BOOL Free(void *pPointer);
	DWORD GetUnitPageSize();
	DWORD GetNumberOfPages();
	DWORD GetUsedMemorySizeInByte();
	void *GetAllocPoint();
};

#endif // __PREALLOCATOR_H__