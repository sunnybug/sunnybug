#include "slab.h"
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <stdlib.h>
#include <memory.h>

#define MAX_FREE_SLABS		8
#ifdef _SLAB_USE_LOCK
	#define SLAB_GUARD(g,l) xGuard<LOCK> g(l)
#else
	#define SLAB_GUARD(g,l)
#endif

#ifndef DEBUG_TRY
#define DEBUG_TRY
#define DEBUG_CATCH
#endif

template<class LOCK>
CCache<LOCK> * CCache<LOCK>::g_cacheList = NULL;

template<class LOCK>
LOCK CCache<LOCK>::g_cacheListLock;


template<class LOCK>
int CCache<LOCK>::reclaimAll()
{
	SLAB_GUARD(g, g_cacheListLock);
	int ret = 0;
	for (CCache *p = g_cacheList; p; p = p->m_nextCache)
	{
		
		ret += p->reclaim();
	}
	return ret;
}

template<class LOCK>
CCache<LOCK>::CCache(int size, int n)
{
	m_objSize = size + sizeof(OBJ);
	m_numObjs = n;
	m_numFreeSlabs = 0;
	m_firstNotFull = &m_slabList;

	m_slabSize = sizeof(SLAB) + m_objSize * n;

#ifdef _SLAB_USE_HEAP
	m_hHeap	=::HeapCreate(0, 0, 0);
#endif

	// Add it to the cache chain
	SLAB_GUARD(g, g_cacheListLock);
	m_nextCache = g_cacheList;
	g_cacheList = this;
}

template<class LOCK>
CCache<LOCK>::~CCache()
{
	SLAB_GUARD(g, m_lock);

	while (!m_slabList.isEmpty()) {
		CListHead *pos = m_slabList.removeHead();
		SLAB *slab = LIST_ENTRY(pos, SLAB, item);
		
#ifdef _SLAB_USE_HEAP
		::HeapFree(m_hHeap, 0, slab);
#else
		free(slab);
#endif
	}
#ifdef _SLAB_USE_HEAP
	if (m_hHeap)
	{
		::HeapDestroy(m_hHeap);
		m_hHeap = 0;
	}
#endif
}

/*
 * Reclaim empty slabs in this cache
 */
template<class LOCK>
int CCache<LOCK>::reclaim(int n)
{
	SLAB *slab;
	CListHead *pos, *t = m_firstNotFull;

	int i = 0;
	while (i < n && (pos = m_slabList.prev) != &m_slabList) {
		slab = LIST_ENTRY(pos, SLAB, item);
		if (slab->inuse)
			break;

		i++;
		if (m_firstNotFull == pos)
			m_firstNotFull = pos->prev;
		pos->remove();
#ifdef _SLAB_USE_HEAP
		::HeapFree(m_hHeap, 0, slab);
#else
		free(slab);
#endif
	}

	if (m_firstNotFull != t && m_firstNotFull != &m_slabList) {
		slab = LIST_ENTRY(m_firstNotFull, SLAB, item);
		if (slab->inuse == m_numObjs)
			m_firstNotFull = &m_slabList;
	}
//	numFreeSlabs += i;
		m_numFreeSlabs -= i;
	return i;
}

/*
 * Alloc a new slab
 */
template<class LOCK>
SLAB *CCache<LOCK>::newSlab()
{
#ifdef _SLAB_USE_HEAP
	SLAB *slab = NULL;
	DEBUG_TRY
	slab	=(SLAB *)::HeapAlloc(m_hHeap, HEAP_ZERO_MEMORY, m_slabSize);
	DEBUG_CATCH("HeapAlloc in NewSlab");
#else
	SLAB *slab = (SLAB *) malloc(m_slabSize);
#endif
	
	if (!slab) {
		if (reclaimAll() > 0) {
#ifdef _SLAB_USE_HEAP
			DEBUG_TRY
			slab	=(SLAB *)::HeapAlloc(m_hHeap, HEAP_ZERO_MEMORY, m_slabSize);
			DEBUG_CATCH("HeapAlloc in NewSlab");
#else
			slab = (SLAB *) malloc(m_slabSize);
#endif
			if (!slab)
				return NULL;
		}
	}

	slab->inuse = 0;

	OBJ *obj = (OBJ *) (slab + 1);
	slab->free = obj;
	for (int i = 0; i < m_numObjs - 1; ++i) {
		OBJ *next = (OBJ *) ((char *) obj + m_objSize);
		obj->next = next;
		obj->slab = slab;
		obj = next;
	}
	obj->next = NULL;
	obj->slab = slab;

	m_slabList.add(&slab->item);
	m_firstNotFull = &slab->item;

	return slab;
}

/*
 * Alloc an object from the cache
 */
template<class LOCK>
void *CCache<LOCK>::allocObj()
{
	void *obj = NULL;
	SLAB *slab;

	SLAB_GUARD(g, m_lock);

	if (m_firstNotFull == &m_slabList)
		slab = newSlab();
	else {
		slab = LIST_ENTRY(m_firstNotFull, SLAB, item);
		if (!slab->inuse)
			m_numFreeSlabs--;
	}

	if (slab) {
		slab->inuse++;
		obj = slab->free + 1;
		slab->free = slab->free->next;

		if (!slab->free)
			m_firstNotFull = slab->item.next;
	}

	return obj;
}

/*
 * Free an object in the cache
 */
template<class LOCK>
void CCache<LOCK>::freeObj(void *p)
{
#ifdef _DEBUG
	memset(p, 0xdd, m_objSize - sizeof(OBJ));
#endif

	SLAB_GUARD(g, m_lock);


	OBJ *obj = (OBJ *) p - 1;
	SLAB *slab = obj->slab;

	obj->next = slab->free;
	slab->free = obj;

	CListHead *pos;
	if (slab->inuse-- == m_numObjs) {
		CListHead *t = m_firstNotFull;
		pos = &slab->item;
		m_firstNotFull = pos;
		if (pos->next != t) {
			pos->remove();
			t->add(pos);
		}
	} else if (!slab->inuse) {
		int n = ++m_numFreeSlabs - MAX_FREE_SLABS;
		if (n > 0)
			reclaim(n);

		CListHead *t = m_firstNotFull->prev;
		pos = &slab->item;

		pos->remove();
		m_slabList.add(pos);
		if (m_firstNotFull == &slab->item)
			m_firstNotFull = t->next;
	}
}
