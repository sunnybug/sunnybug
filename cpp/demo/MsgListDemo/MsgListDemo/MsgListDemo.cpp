// MsgListDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <process.h>
#include <iostream>
#include <vector>
#include <assert.h>
#include <xLockFreeQ.h>
#include <xMutexMsgList.h>

using namespace std;
//#define _MSGPTR
//#define _MSG_SECOND_MALLOC
#define MSG_SIZE 100

//////////////////////////////////////////////////////////////////////////
//type
struct ThreadInfo
{
	int id;
	int nProductMsgCount;
};
struct xMsgSecondNew 
{
	int id;
	char* info;
	xMsgSecondNew():id(0)
	{
		info = new char[MSG_SIZE];
		assert(info);
		info[0] = 0;
	}
	~xMsgSecondNew()
	{
		delete [] info;
	}

	xMsgSecondNew& operator = (const xMsgSecondNew& r)
	{
		delete [] info;
		info = new char[MSG_SIZE];
		memcpy(info, r.info, MSG_SIZE);
		return *this;
	
	}
};

struct xMsgStack
{
	int id;
	char info[MSG_SIZE];
	xMsgStack():id(0)
	{
		info[0] = 0;
	}
	~xMsgStack()
	{
	}

	xMsgStack& operator = (const xMsgStack& r)
	{
		return *this;
	}
};
#ifdef _MSG_SECOND_MALLOC
typedef xMsgSecondNew xMsg;
#else
typedef xMsgStack xMsg;
#endif


#ifdef _MSGPTR
typedef MSQueue<xMsg*> xMsgQueue;
#else
typedef MSQueue<xMsg> xMsgQueue;
#endif // _MSGPTR

//////////////////////////////////////////////////////////////////////////
//var
xMsgQueue g_MsgQueue;
bool g_bRun = true;
bool g_bConsumerRun = true;
const int nProductIDOffset = 100000000;
const char* MSG_HEAD = "消息0123456789012345678901234567890123456789";
const int nProductMsgCountPerThread = 1000000;
const int nProductThreadCount = 5;
const int nStaticMsgCount = 10000000;
volatile LONG g_nMsgCount = 0;

//////////////////////////////////////////////////////////////////////////
void PrintInfo(const char* fmt, ...)
{
	char buf[1024] = {0};
	_vsnprintf(buf, sizeof(buf)-1, fmt, (char*)((&fmt)+1));
	buf[sizeof(buf)-1] = '\0';
	printf("%s\n", buf);
}

//////////////////////////////////////////////////////////////////////////
static unsigned __stdcall ProducterThread(void* lparam)
{
	ThreadInfo* pInfo = (ThreadInfo*)lparam;
	PrintInfo("Producter[%d] begin", pInfo->id);
	
	//产生消息
	int nMsgID = pInfo->id*nProductIDOffset;
	int nMsgCount = 0;

	DWORD dwStart = ::GetTickCount();
	while(g_bRun && nMsgCount<pInfo->nProductMsgCount)
	{
		++ nMsgCount;
		if(nMsgCount % nStaticMsgCount == 0)
		{
			//统计下时间
			DWORD dwEnd = ::GetTickCount();
			//PrintInfo("Producter[%d] add msg count:%d %uMS", pInfo->id, nStaticMsgCount, dwEnd-dwStart);
			dwStart = dwEnd;
			Sleep(5000);
		}
#ifdef _MSGPTR
		xMsg* msg = new xMsg;
		msg->id = ++nMsgID;
		_snprintf(msg->info, sizeof(msg->info)-1, "%s%d", MSG_HEAD, msg->id);
#else
		xMsg msg;
		msg.id = ++nMsgID;
		_snprintf(msg.info, sizeof(msg.info)-1, "%s%d", MSG_HEAD, msg.id);
#endif // _MSGPTR
		g_MsgQueue.enqueue(msg);
		::InterlockedIncrement(&g_nMsgCount);
	}

	PrintInfo("Producter[%d] end", pInfo->id);
	delete pInfo;
	return 0;
}

static unsigned __stdcall ConsumerThread(void* lparam)
{
	g_bConsumerRun = true;
	ThreadInfo* pInfo = (ThreadInfo*)lparam;
	PrintInfo("Consumer begin");

#ifdef _MSGPTR
	xMsg* msg = NULL;
#else
	xMsg msg;
#endif // _MSGPTR
	int nMsgCount = pInfo->nProductMsgCount;

	int nConsumerMsg = 0;
	DWORD dwStart = ::GetTickCount();
	const DWORD dwStart0 = dwStart;
	while(nMsgCount>0)
	{
		if(g_MsgQueue.dequeue(msg))
		{
			--nMsgCount;
			++nConsumerMsg;
			if(nConsumerMsg % nStaticMsgCount == 0)
			{
				//统计下时间
				DWORD dwEnd = ::GetTickCount();
				PrintInfo("Consumer msg count:%d %uMS CurMsgCount=%d", nStaticMsgCount, dwEnd-dwStart, g_nMsgCount);
				dwStart = dwEnd;
			}
#ifdef _MSGPTR
			delete msg;
#endif // _MSGPTR
			::InterlockedDecrement(&g_nMsgCount);
		}
	}

	PrintInfo(">>>Consumer end %uMS", GetTickCount()-dwStart0);
	delete pInfo;
	g_bConsumerRun = false;
	return 0;
}

void TestMsgList()
{
	typedef std::vector<HANDLE> HANDLE_LIST;
	g_bRun = true;
	g_bConsumerRun = true;
	unsigned int uiThreadID = 0;
	HANDLE_LIST lstThread;

	int idProducter = 0;
	for (int i=0; i<nProductThreadCount; ++i)
	{
		ThreadInfo* pInfo = new ThreadInfo;
		pInfo->id = ++idProducter;
		pInfo->nProductMsgCount = nProductMsgCountPerThread;
		HANDLE h = (HANDLE)_beginthreadex(NULL,NULL,ProducterThread, pInfo, 0, &uiThreadID);
		lstThread.push_back(h);
	}

	ThreadInfo* pInfo = new ThreadInfo;
	pInfo->nProductMsgCount = nProductThreadCount*nProductMsgCountPerThread;
	HANDLE h = (HANDLE)_beginthreadex(NULL,NULL,ConsumerThread, pInfo, 0, &uiThreadID);
	lstThread.push_back(h);


// 	char c = 0;
// 	cin.get(c);
// 	g_bRun = false;
	
	for (;;)
	{
		if(!g_bConsumerRun)
			break;
		Sleep(10);
	}
	PrintInfo("exit");
	for (HANDLE_LIST::iterator it=lstThread.begin(); it!=lstThread.end(); ++it)
	{
		CloseHandle(*it);
	}
}

//////////////////////////////////////////////////////////////////////////
//测试new
#define TEST_NEW_COUNT 1000000
#define TEST_NEW_SIZE 2048
struct NewInfo 
{
	char info[TEST_NEW_SIZE];
};
struct SecondNewInfo 
{
	char* info;
	SecondNewInfo()
	{
		info = new char[100];
	}
	~SecondNewInfo()
	{
		delete [] info;
	}
};
void TestNew(const int nTestNewCount)
{
	DWORD dw1 = ::GetTickCount();
	for (int i=0; i<nTestNewCount; ++i)
	{
		NewInfo* p = new NewInfo;
		delete p;
	}
	PrintInfo("new1 count=%d size=%d time=%uMS", nTestNewCount, TEST_NEW_SIZE, GetTickCount()-dw1);

	dw1 = ::GetTickCount();
	for (int i=0; i<nTestNewCount; ++i)
	{
		SecondNewInfo* p = new SecondNewInfo;
		delete p;
	}
	PrintInfo("new2 count=%d size=%d time=%uMS", nTestNewCount, TEST_NEW_SIZE, GetTickCount()-dw1);
}

//////////////////////////////////////////////////////////////////////////
//比较string和char的效率
void TestString()
{
	const char* TEST_STRING = "01234567890123456789012345678901234567890123456789";
	const int TEST_STRINTG_SIZE = strlen(TEST_STRING);
	const int nTestNewCount = 1000000;
	DWORD dw1 = ::GetTickCount();
	for (int i=0; i<nTestNewCount; ++i)
	{
		string* p = new string;
		*p = TEST_STRING;
		delete p;
	}
	PrintInfo("string count=%d size=%d time=%uMS", nTestNewCount, TEST_STRINTG_SIZE, GetTickCount()-dw1);

	dw1 = ::GetTickCount();
	for (int i=0; i<nTestNewCount; ++i)
	{
		char* p = new char[8096];
		strcpy(p, TEST_STRING);
		delete p;
	}
	PrintInfo("char count=%d size=%d time=%uMS", nTestNewCount, TEST_STRINTG_SIZE, GetTickCount()-dw1);
	
}

//////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	//TestNew(TEST_NEW_COUNT);
	for (int i=0;i<500;++i)
	{
		TestMsgList();
	}
	system("pause");
	return 0;
}

