// 诊断程序
// 仙剑修, 2002.1.13

#ifndef	CRITSECT_H
#define CRITSECT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include "common.h"


/*
#include <afxmt.h>
#define	CCritSect	CCriticalSection
#define CSLock		CSingleLock
//*/
//*else
class	CCritSect
{
public:
	CCritSect() { InitializeCriticalSection(&m_cs); }
	~CCritSect() { DeleteCriticalSection(&m_cs); }
protected:
	CRITICAL_SECTION	m_cs; 
	friend class	CSLock;
};

class	CSLock
{
public:
	CSLock(CCritSect * xCtrl, bool nLock = true) 
	{ 
		m_pCritSect = xCtrl; 
		CHECK(m_pCritSect);
		EnterCriticalSection(&(m_pCritSect->m_cs)); 
	}
	~CSLock()	{ if(m_pCritSect) LeaveCriticalSection(&(m_pCritSect->m_cs)); }
	void Unlock()	{ if(m_pCritSect) LeaveCriticalSection(&(m_pCritSect->m_cs)); m_pCritSect=NULL; }
protected:
	CCritSect * m_pCritSect;
};
//*/

/*
用法举例：

定义一个对象：
CCritSect	g_xCtrl;

使用：
	{
		CSLock	m_lock(g_xCtrl);	// 锁定相关数据

		// 操作共享数据的代码
		// ...

	}	// 离开作用区时自动解锁
*/

#endif	// CRITSECT_H
