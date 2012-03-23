#ifndef _SLAB_H
#define _SLAB_H

#include "slablist.h"
#include <stdio.h>
#include <xGuard.h>

#define _SLAB_USE_HEAP
#define _SLAB_USE_LOCK

typedef void *HANDLE;

struct SLAB;

struct OBJ
{
	OBJ *next;
	SLAB *slab;
};

struct SLAB
{
	CListHead item;
	int inuse;
	OBJ *free;
};

template<class LOCK>
class CCache
{
public:
	CCache(int size, int n);
	~CCache();

	void *allocObj();
	void freeObj(void *p);

	static int reclaimAll();

private:
	SLAB *newSlab();
	int reclaim(int n = 0xffff);
	
#ifdef _SLAB_USE_LOCK
	LOCK	m_lock;
#endif // _SLAB_USE_LOCK

	CCache *m_nextCache;
	CListHead m_slabList;
	CListHead *m_firstNotFull;

	int m_objSize;
	int m_numObjs;
	int m_numFreeSlabs;
	int m_slabSize;

#ifdef _SLAB_USE_HEAP	
protected:
	HANDLE			m_hHeap;
#endif

	static CCache<LOCK> *g_cacheList;
	static LOCK g_cacheListLock;
};


#define MYSLAB_DECLARATION(type)		\
private:	\
	static CCache type##_cache;	\
public:	\
	void *operator new(size_t) {	\
		return type##_cache.allocObj();	\
	}	\
	void operator delete(void *p) {	\
		type##_cache.freeObj(p);	\
	}

#define MYSLAB_IMPLEMENTATION(type, num)	\
	CCache type::type##_cache(sizeof(type), num);

//#define MYSLAB_DECLARATION(type) 
///#define MYSLAB_IMPLEMENTATION(type, num)

#include "slab.inl"

#endif