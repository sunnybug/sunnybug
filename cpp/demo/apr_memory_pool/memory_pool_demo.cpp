// memory_pool_demo.cpp : Defines the entry point for the console application.
//

#include <ObjBase.h>
#include "stdafx.h"
#include <apr_pools.h>
#include <apr_buckets.h>
#include <stdio.h>
#include <new>
#include <vector>
#include <process.h>
#include <xTickCount.h>
#include <iostream>
#include "MyHeap.h"
#include <boost/pool/pool.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <slab.h>
#include <xCriticalSection.h>

using namespace std;

#pragma comment(lib, "libapr-1.lib")
#pragma comment(lib, "libaprutil-1.lib")

bool g_exit = false;

//////////////////////////////////////////////////////////////////////////
const int COUNT_MALLOC = 100*1024;
const int COUNT_THREAD = 2;
const int OBJ_SIZE = 2048;

//////////////////////////////////////////////////////////////////////////

struct CStudent
{
	char name[OBJ_SIZE];
};
typedef std::vector<CStudent*> StudentPtrVec;

typedef boost::singleton_pool<CStudent,sizeof(CStudent)>  StudentObjPool;

struct ThreadInfo
{
	CMyHeap* my_heap;
	apr_bucket_alloc_t* balloc;
	boost::pool<>*	boost_pool;
	CCache<xCriticalSection>*	pSlabCache;
	bool	boost_pool_thd;
	int n;

	ThreadInfo():my_heap(NULL),balloc(NULL),boost_pool(NULL),pSlabCache(NULL),boost_pool_thd(false),n(0)
	{

	}
};

void TestFunc( int &nCount, ThreadInfo* pInfo, DWORD &dwMallocTick, DWORD&  dwFreeTick, StudentPtrVec& vecObj) 
{
	++nCount;

	xTickCount xTick;
	vecObj.clear();
	vecObj.reserve(COUNT_MALLOC);
	CStudent* obj = NULL;
	DWORD dwTotalTime = 0;

	//malloc
	for (int i=0; i<COUNT_MALLOC; ++i)
	{
		xTick.Start();
		if(pInfo->balloc)
		{
			// 	内存申请与释放
			obj= (CStudent*)apr_bucket_alloc(sizeof(CStudent), pInfo->balloc);
		}
		else if(pInfo->boost_pool)
		{
			obj=(CStudent *)pInfo->boost_pool->malloc();
		}
		else if(pInfo->boost_pool_thd)
		{
			obj=(CStudent *)StudentObjPool::malloc();
		}
		else if(pInfo->pSlabCache)
		{
			obj=(CStudent *)pInfo->pSlabCache->allocObj();
		}
		else if(pInfo->my_heap)
		{
			obj=(CStudent *)pInfo->my_heap->Alloc(sizeof(*obj));
		}
		else
		{
			 obj= (CStudent*)malloc(sizeof(CStudent));
		}
		if (obj == NULL)
		{
			printf("err\r\n");
			system("pause");
			return;
		}
		xTick.Finish();
		dwMallocTick += xTick.GetUsedMS();
		dwTotalTime += xTick.GetUsedMS();
		vecObj.push_back(obj);
	}

	//free
	for (int i=COUNT_MALLOC-1; i>=0; --i)
	{
		obj = vecObj[i];
		xTick.Start();
		if(pInfo->balloc)
		{
			apr_bucket_free(obj);
		}
		else if(pInfo->boost_pool)
		{
			pInfo->boost_pool->free(obj);
		}
		else if(pInfo->boost_pool_thd)
		{
			StudentObjPool::free(obj);
		}
		else if(pInfo->pSlabCache)
		{
			pInfo->pSlabCache->freeObj(obj);
		}
		else if(pInfo->my_heap)
		{
			pInfo->my_heap->Free(obj);
		}
		else
		{
			free(obj);
		}
		xTick.Finish();
		dwFreeTick += xTick.GetUsedMS();
		dwTotalTime += xTick.GetUsedMS();
		vecObj.pop_back();
	}

	printf("[%d]:[%d] %dms\r\n", pInfo->n, nCount, dwTotalTime);
}

static unsigned __stdcall ConsumerThread(void* lparam)
{
	ThreadInfo* pInfo = (ThreadInfo*)lparam;
	int nCount = 0;
	DWORD dwMallockTime = 0;
	DWORD dwFreeTime = 0;
	StudentPtrVec vecObj;
	while (!g_exit)
	{
		TestFunc(nCount, pInfo, dwMallockTime, dwFreeTime, vecObj);
		Sleep(1);
	}
	printf("[%d]:COUNT;%d AvgMalloc=%dms AvgFree=%dms \r\n", pInfo->n, COUNT_MALLOC, dwMallockTime/(nCount), dwFreeTime/(nCount));

	if(pInfo->balloc)
	{
		printf("apr\r\n");
	}
	else if(pInfo->boost_pool)
	{
		printf("boost\r\n");
	}
	else if(pInfo->my_heap)
	{
		printf("my_heap\r\n");
	}
	else if(pInfo->boost_pool_thd)
	{
		printf("boost_mutex\r\n");
	}
	else if(pInfo->pSlabCache)
	{
		printf("slab\r\n");
	}
	else
	{
		printf("malloc\r\n");
	}
	delete pInfo;
	return 0;
}

void apr_pool_test()
{
	apr_pool_t *root = NULL;
	apr_pool_initialize();//初始化全局分配子（allocator），并为它设置mutext，以用于多线程环境，初始化全局池，指定全局分配子的owner是全局池
	apr_pool_create(&root,NULL);//创建根池(默认父池是全局池)，根池生命期为进程生存期。分配子默认为全局分配子

	{
		apr_pool_t *child = NULL;
		apr_pool_create(&child,root);//创建子池，指定父池为root。分配子默认为父池分配子

		void *pBuff=apr_palloc(child,sizeof(int));//从子池分配内存
		int *pInt=new (pBuff)  int(5);//随便举例下基于已分配内存后，面向对象构造函数的调用。
		printf("pInt=%d\n",*pInt);

		apr_pool_destroy(child);//释放子池，将内存归还给分配子
	}

	apr_pool_destroy(root);//释放父池，
	apr_pool_terminate();//释放全局池，释放全局allocator，将内存归还给系统
}

void apr_chunk_pool_test()
{
// 	初始化操作
	apr_pool_t *pool;
	apr_allocator_t* alloc;
	apr_bucket_alloc_t* balloc;


	bool bResult = true;
	unsigned int uiThreadID;
	for (int i=0; i<COUNT_THREAD; ++i)
	{
		apr_allocator_create(&alloc);
		apr_pool_create_ex(&pool, NULL, NULL, alloc);
		balloc = apr_bucket_alloc_create(pool);

		ThreadInfo* pInfo = new ThreadInfo;;
		pInfo->balloc = balloc;
		pInfo->n = i;
		bResult &= (_beginthreadex(NULL,NULL,ConsumerThread, pInfo, 0, &uiThreadID)!=0);
	}

	char c = 0;
	cin.get(c);
	g_exit = true;

	//release
	apr_bucket_alloc_destroy(balloc);
	apr_pool_destroy(pool);
	apr_allocator_destroy(alloc);
	cin.get(c);
}

void apr_chunk_pool_test_mutex()
{
	// 	初始化操作
	apr_pool_t *pool;
	apr_allocator_t* alloc;
	apr_bucket_alloc_t* balloc;

	apr_allocator_create(&alloc);
	apr_pool_create_ex(&pool, NULL, NULL, alloc);
	balloc = apr_bucket_alloc_create(pool);

	apr_thread_mutex_t* pMutex = NULL;
	apr_thread_mutex_create(&pMutex, APR_THREAD_MUTEX_DEFAULT, pool);
	apr_allocator_mutex_set(apr_pool_allocator_get(pool), pMutex);

	bool bResult = true;
	unsigned int uiThreadID;
	for (int i=0; i<COUNT_THREAD; ++i)
	{
		ThreadInfo* pInfo = new ThreadInfo;;
		pInfo->balloc = balloc;
		pInfo->n = i;
		bResult &= (_beginthreadex(NULL,NULL,ConsumerThread, pInfo, 0, &uiThreadID)!=0);
	}

	char c = 0;
	cin.get(c);
	g_exit = true;

	//release
	apr_bucket_alloc_destroy(balloc);
	apr_pool_destroy(pool);
	apr_allocator_destroy(alloc);
	cin.get(c);
}

void boost_pool_test()
{
	bool bResult = true;
	unsigned int uiThreadID;
	for (int i=0; i<COUNT_THREAD; ++i)
	{
		ThreadInfo* pInfo = new ThreadInfo;;
		pInfo->boost_pool = new boost::pool<>(sizeof(CStudent));
		pInfo->n = i;
		bResult &= (_beginthreadex(NULL,NULL,ConsumerThread, pInfo, 0, &uiThreadID)!=0);
	}

	char c = 0;
	cin.get(c);
	g_exit = true;
	cin.get(c);
}

void boost_pool_test_mutex()
{
	bool bResult = true;
	unsigned int uiThreadID;
	for (int i=0; i<COUNT_THREAD; ++i)
	{
		ThreadInfo* pInfo = new ThreadInfo;;
		pInfo->boost_pool_thd = true;
		pInfo->n = i;
		bResult &= (_beginthreadex(NULL,NULL,ConsumerThread, pInfo, 0, &uiThreadID)!=0);
	}

	char c = 0;
	cin.get(c);
	g_exit = true;
	cin.get(c);
}

void malloc_test()
{
	bool bResult = true;
	unsigned int uiThreadID;
	for (int i=0; i<COUNT_THREAD; ++i)
	{
		ThreadInfo* pInfo = new ThreadInfo;;
		pInfo->n = i;
		bResult &= (_beginthreadex(NULL,NULL,ConsumerThread, pInfo, 0, &uiThreadID)!=0);
	}

	char c = 0;
	cin.get(c);
	g_exit = true;
	cin.get(c);
}

void myheap_test()
{
	bool bResult = true;
	unsigned int uiThreadID;
	CMyHeap* pHeap = new CMyHeap();
	for (int i=0; i<COUNT_THREAD; ++i)
	{
		ThreadInfo* pInfo = new ThreadInfo;;
		pInfo->my_heap = pHeap;
		pInfo->n = i;
		bResult &= (_beginthreadex(NULL,NULL,ConsumerThread, pInfo, 0, &uiThreadID)!=0);
	}

	char c = 0;
	cin.get(c);
	g_exit = true;
	cin.get(c);
}

void slab_test()
{
	bool bResult = true;
	unsigned int uiThreadID;
	CCache<xCriticalSection>* p = new CCache<xCriticalSection>(sizeof(CStudent), 1);
	for (int i=0; i<COUNT_THREAD; ++i)
	{
		ThreadInfo* pInfo = new ThreadInfo;;
	//	pInfo->pSlabCache = new CCache<xCriticalSection>(sizeof(CStudent), 1);
		pInfo->pSlabCache = p;
		pInfo->n = i;
		bResult &= (_beginthreadex(NULL,NULL,ConsumerThread, pInfo, 0, &uiThreadID)!=0);
	}

	char c = 0;
	cin.get(c);
	g_exit = true;
	cin.get(c);
	CCache<xCriticalSection>::reclaimAll();
}

void TestCoMem(const int COUNT)
{
	typedef std::vector<void*> PTR_VEC;
	PTR_VEC vec;
	vec.reserve(COUNT);
	DWORD dwStart = GetTickCount();
	for (int i=0; i<COUNT; ++i)
	{
		if (i % 1024 == 0)
		{
			//printf("%d use %u ms\r\n", i, GetTickCount() - dwStart);
			dwStart = GetTickCount();
		}
		vec.push_back(CoTaskMemAlloc(4096));
	}

	for (int i=0; i<COUNT; ++i)
	{
		CoTaskMemFree(vec[i]);
	}
}

void TestMalloc(const int COUNT)
{1
	typedef std::vector<void*> PTR_VEC;
	PTR_VEC vec;
	vec.reserve(COUNT);
	DWORD dwStart = GetTickCount();
	for (int i=0; i<COUNT; ++i)
	{
		if (i % 1024 == 0)
		{
		//	printf("%d use %u ms\r\n", i, GetTickCount() - dwStart);
			dwStart = GetTickCount();
		}
		vec.push_back(new char[4096]);
	}

	for (int i=0; i<COUNT; ++i)
	{
		delete [] (vec[i]);
	}
}


//////////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	//apr_chunk_pool_test();
	//apr_chunk_pool_test_mutex();
	//malloc_test();
	//boost_pool_test();
	//boost_pool_test_mutex();

	DWORD dwStart = GetTickCount();
 	const int COUNT = 100*1024;
	while (true)
	{ 
		TestMalloc(COUNT);
 		printf("use %u ms\r\n", GetTickCount() - dwStart);
	}

	while (true)
	{
		dwStart = GetTickCount();
		TestCoMem(COUNT);
		printf("use %u ms\r\n", GetTickCount() - dwStart);
	}
	return 0;
}

