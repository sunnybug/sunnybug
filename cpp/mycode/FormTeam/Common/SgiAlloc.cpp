// SgiAlloc.cpp: implementation of the CSgiAlloc class.
//
//////////////////////////////////////////////////////////////////////

#include "afx.h"
#include "windows.h"
#include "SgiAlloc.h"
#include "common.h"
#include "logfile.h"

#ifdef __STL_CONFIG_H
#include <alloc.h>
#pragma warning(disable:4786)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSgiAlloc::CSgiAlloc(LPCTSTR pszClassName)
{
//	m_hAlloc	=::AllocCreate(0, 0, 0);
//	CHECK(m_hAlloc);
//	::OutputDebugString("CSgiAlloc\n");

	m_uNumAllocsInAlloc	= 0;
	m_szClassName[0]	= 0;
	if(pszClassName)
		SafeCopy(m_szClassName, pszClassName, CLASSNAME_SIZE);
}

//////////////////////////////////////////////////////////////////////
CSgiAlloc::~CSgiAlloc()
{
//	if (m_hAlloc)
//	{
//		::AllocDestroy(m_hAlloc);
//		m_hAlloc = 0;
//	}
//	::OutputDebugString("~CSgiAlloc\n");

	if(m_uNumAllocsInAlloc != 0)
	{
		if(strlen(m_szClassName) > 0)
			LOGWARNING("CSgiAlloc::~CSgiAlloc() 的[%s]有[%d]个内存块没有释放", m_szClassName, m_uNumAllocsInAlloc);
		else
			LOGWARNING("CSgiAlloc::~CSgiAlloc() 有[%d]个内存块没有释放", m_uNumAllocsInAlloc);
	}
}

//////////////////////////////////////////////////////////////////////
void * CSgiAlloc::Alloc(size_t size)
{
//	CHECKF(m_hAlloc);
	if(size == 0)
		return false;

	// alloc mem for new obj
	void* p = NULL;
	DEBUG_TRY
//		p	=::AllocAlloc(m_hAlloc, HEAP_ZERO_MEMORY, size);
		p	= std::alloc::allocate(size);

//	#ifndef _DEBUG
//	}catch(CMemoryException* ) { ::LogSave("CATCH: *** {" "AllocAlloc memory failed[%s][%d]." "} new failed! ***", m_szClassName, size);
//	#endif
	DEBUG_CATCH3("AllocAlloc [%s][%d]", m_szClassName, size)

	if (p)

		InterlockedIncrement(&m_uNumAllocsInAlloc);

	
	return p;
}

//////////////////////////////////////////////////////////////////////
void CSgiAlloc::Free(void* p, size_t size)
{
	if(!p)
		return ;

	// check param
//	CHECK(m_hAlloc);
	IF_NOT(m_uNumAllocsInAlloc > 0)
		LOGWARNING("CSgiAlloc::Free() 的[%s]内存块数量错误!", m_szClassName);

	// free it...
//	if (::AllocFree(m_hAlloc, 0, p))
	std::alloc::deallocate(p, size);
	{
		InterlockedDecrement(&m_uNumAllocsInAlloc);
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CSgiAlloc::IsValidPt(void* p)
{
//	if (!m_hAlloc || !p)
//		return false;
	
	if ((DWORD)p < 0x00010000 || (DWORD)p >= 0x7FFEFFFF)	// user address range from 1M--2G-64k
		return false;
	
	if (::IsBadCodePtr((FARPROC)p))
		return false;
	
//	return ::AllocValidate(m_hAlloc, 0, p);
	return true;
}
#endif







