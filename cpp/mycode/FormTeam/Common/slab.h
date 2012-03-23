#ifndef _SLAB_H
#define _SLAB_H

#include "list.h"
#include <stdio.h>

#define _SLAB_USE_HEAP
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

	CCache *nextCache;
	CListHead slabList;
	CListHead *firstNotFull;

	int objSize;
	int numObjs;
	int numFreeSlabs;
	int slabSize;
#ifdef _SLAB_USE_HEAP	
protected:
	HANDLE			m_hHeap;
#endif

	static CCache *cacheList;
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

#endif