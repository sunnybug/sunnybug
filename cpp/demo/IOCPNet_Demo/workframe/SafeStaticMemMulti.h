////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		SafeStaticMemMulti.h
//	Author:
//		Chun-Hyok, Chong.
//	Description:
//		The class that insures the release of the multiple used memory that allocated by PreAllocator.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SAFESTATICMEMMULTI_H__
#define __SAFESTATICMEMMULTI_H__

#include "PreAllocator.h"
#include "linkedliststatic.h"

#ifndef __OPOINTER__
#define __OPOINTER__
struct OPointer
{
	void *AllocPointer;
};
#endif // __OPOINTER__

class OSafeStaticMemMulti
{
private:
	OPreAllocator m_PAList;
	// 'OLinkedListStatic<OPointer> m_AllocPointerList(&m_PAList);' is not available in the declaration of the class.
	OLinkedListStatic<OPointer> m_AllocPointerList;
	CRITICAL_SECTION m_CriSec;
	OPreAllocator *m_pPreAllocator;
	
public:
	OSafeStaticMemMulti()
	{
		Init();
	}
	OSafeStaticMemMulti(OPreAllocator *pPreAllocator)
	{
		Init(pPreAllocator);
	}
	~OSafeStaticMemMulti()
	{
		DeInit();
	}

	void Init();
	void Init(OPreAllocator *pPreAllocator);
	void DeInit();
	BOOL SetPreAllocator(OPreAllocator *pPreAllocator);
	void *Allocate(DWORD Size);
	BOOL Free(void *AllocPointer);
	OPreAllocator *GetPreAllocator();
};

#endif // __SAFESTATICMEMMULTI_H__