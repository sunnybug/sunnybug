#include "slab.h"
#include <stdlib.h>
#include <memory.h>
#include "common.h"

#define MAX_FREE_SLABS		8


CCache *CCache::cacheList = NULL;


int CCache::reclaimAll()
{
	int ret = 0;
	for (CCache *p = cacheList; p; p = p->nextCache)
		ret += p->reclaim();
	return ret;
}

CCache::CCache(int size, int n)
{
	objSize = size + sizeof(OBJ);
	numObjs = n;
	numFreeSlabs = 0;
	firstNotFull = &slabList;

	slabSize = sizeof(SLAB) + objSize * n;

	// Add it to the cache chain
	nextCache = cacheList;
	cacheList = this;
#ifdef _SLAB_USE_HEAP
	m_hHeap	=::HeapCreate(0, 0, 0);
	CHECK(m_hHeap);
#endif
}

CCache::~CCache()
{
	while (!slabList.isEmpty()) {
		CListHead *pos = slabList.removeHead();
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
int CCache::reclaim(int n)
{
	SLAB *slab;
	CListHead *pos, *t = firstNotFull;

	int i = 0;
	while (i < n && (pos = slabList.prev) != &slabList) {
		slab = LIST_ENTRY(pos, SLAB, item);
		if (slab->inuse)
			break;

		i++;
		if (firstNotFull == pos)
			firstNotFull = pos->prev;
		pos->remove();
#ifdef _SLAB_USE_HEAP
		::HeapFree(m_hHeap, 0, slab);
#else
		free(slab);
#endif
	}

	if (firstNotFull != t && firstNotFull != &slabList) {
		slab = LIST_ENTRY(firstNotFull, SLAB, item);
		if (slab->inuse == numObjs)
			firstNotFull = &slabList;
	}
//	numFreeSlabs += i;
		numFreeSlabs -= i;
	return i;
}

/*
 * Alloc a new slab
 */
SLAB *CCache::newSlab()
{
#ifdef _SLAB_USE_HEAP
	SLAB *slab = NULL;
	DEBUG_TRY
	slab	=(SLAB *)::HeapAlloc(m_hHeap, HEAP_ZERO_MEMORY, slabSize);
	DEBUG_CATCH("HeapAlloc in NewSlab")
#else
	SLAB *slab = (SLAB *) malloc(slabSize);
#endif
	
	if (!slab) {
		if (reclaimAll() > 0) {
#ifdef _SLAB_USE_HEAP
			DEBUG_TRY
			slab	=(SLAB *)::HeapAlloc(m_hHeap, HEAP_ZERO_MEMORY, slabSize);
			DEBUG_CATCH("HeapAlloc in NewSlab")
#else
			slab = (SLAB *) malloc(slabSize);
#endif
			if (!slab)
				return NULL;
		}
	}

	slab->inuse = 0;

	OBJ *obj = (OBJ *) (slab + 1);
	slab->free = obj;
	for (int i = 0; i < numObjs - 1; ++i) {
		OBJ *next = (OBJ *) ((char *) obj + objSize);
		obj->next = next;
		obj->slab = slab;
		obj = next;
	}
	obj->next = NULL;
	obj->slab = slab;

	slabList.add(&slab->item);
	firstNotFull = &slab->item;

	return slab;
}

/*
 * Alloc an object from the cache
 */
void *CCache::allocObj()
{
	void *obj = NULL;
	SLAB *slab;

	if (firstNotFull == &slabList)
		slab = newSlab();
	else {
		slab = LIST_ENTRY(firstNotFull, SLAB, item);
		if (!slab->inuse)
			numFreeSlabs--;
	}

	if (slab) {
		slab->inuse++;
		obj = slab->free + 1;
		slab->free = slab->free->next;

		if (!slab->free)
			firstNotFull = slab->item.next;
	}

	return obj;
}

/*
 * Free an object in the cache
 */
void CCache::freeObj(void *p)
{
#ifdef _DEBUG
	memset(p, 0xdd, objSize - sizeof(OBJ));
#endif

	OBJ *obj = (OBJ *) p - 1;
	SLAB *slab = obj->slab;

	obj->next = slab->free;
	slab->free = obj;

	CListHead *pos;
	if (slab->inuse-- == numObjs) {
		CListHead *t = firstNotFull;
		pos = &slab->item;
		firstNotFull = pos;
		if (pos->next != t) {
			pos->remove();
			t->add(pos);
		}
	} else if (!slab->inuse) {
		int n = ++numFreeSlabs - MAX_FREE_SLABS;
		if (n > 0)
			reclaim(n);

		CListHead *t = firstNotFull->prev;
		pos = &slab->item;

		pos->remove();
		slabList.add(pos);
		if (firstNotFull == &slab->item)
			firstNotFull = t->next;
	}
}
