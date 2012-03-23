#pragma once

/**
 * ID:     122611
 * Author: zhouweidi
 */

#include "MemoryProfilerAdapter.h"

namespace mp
{

//
// SMemoryUsage
//

struct SMemoryUsage
{
	SMemoryUsage()
		: allocatedBytes( 0 )
		, freedBytes	( 0 )
	{}

	void			operator +=(const SMemoryUsage &rhs);

	size_t			allocatedBytes;
	size_t			freedBytes;
};


//
// SMemoryUsageReport
//

struct SMemoryUsageReport
{
	SMemoryUsage	exclusive, inclusive;
};


//
// SFunctionInfo
//

struct SFunctionInfo
{
	SFunctionInfo(const tchar *name_, const tchar *file_, int line_)
		: name	( name_ )
		, file	( file_ )
		, line	( line_ )
	{}

	const tchar		*name;
	const tchar		*file;
	int				line;
};

typedef std::map<const SFunctionInfo*, SMemoryUsageReport> TReport;


//
// CCallTreeNode
//

class CCallTreeNode
{
public:
	CCallTreeNode(CCallTreeNode *pParent, const SFunctionInfo *pFunc)
		: m_pParent		( pParent )
		, m_pFunc		( pFunc )
		, m_pLastSubCall( 0 )
	{}

	CCallTreeNode*			Push(const SFunctionInfo *pFunc);
	void					OnAllocate(size_t bytes);
	void					OnFree(size_t bytes);
	void					GenerateReport(TReport &report) const;
	CCallTreeNode*			Parent() const				{ return m_pParent; }
	const SFunctionInfo*	Func() const				{ return m_pFunc; }

private:
	void					GenerateSelfReport(TReport &report) const;

private:
	typedef std::list<CCallTreeNode>	TNodeList;

	CCallTreeNode			*m_pParent;
	TNodeList				m_subs;
	const SFunctionInfo		*m_pFunc;
	CCallTreeNode			*m_pLastSubCall;
	SMemoryUsage			m_exclusive;
};


//
// CCallTree
//

class CCallTree
{
public:
	CCallTree()
		: m_root	( 0, &s_global )
		, m_pCurrent( &m_root )
	{}

	void						Push(const SFunctionInfo &func)	{ __M_BASE_Check( m_pCurrent ); m_pCurrent = m_pCurrent->Push( &func ); }
	void						Pop()							{ __M_BASE_Check( m_pCurrent && m_pCurrent != &m_root ); m_pCurrent = m_pCurrent->Parent(); }
	const CCallTreeNode*		Root() const					{ return &m_root; }
	CCallTreeNode*				Current() const					{ return m_pCurrent; }

private:
	static const SFunctionInfo	s_global;

	CCallTreeNode				m_root;
	CCallTreeNode				*m_pCurrent;
};


//
// CThreadTracker
//

class CThreadTracker
{
public:
	void					Push(const SFunctionInfo &func);
	void					Pop();
	CCallTreeNode*			Current() const				{ return m_callTree.Current(); }
	bool					InternalState() const		{ return m_internalState; }
	void					GenerateReport(TReport &report) const;
	static CThreadTracker&	PerThreadInstance();

private:
	CThreadTracker();

	friend class CInternalState;

private:
	bool					m_internalState;
	CCallTree				m_callTree;
};

inline CThreadTracker&		CurrentThreadTracker()		{ return CThreadTracker::PerThreadInstance(); }


//
// CInternalState
//

class CInternalState
{
public:
	CInternalState(CThreadTracker &tracker = CurrentThreadTracker())
		: m_pInternalState( &tracker.m_internalState )
	{
		__M_BASE_Check( !*m_pInternalState );
		*m_pInternalState = true;
	}

	~CInternalState()
	{
		__M_BASE_Check( *m_pInternalState );
		*m_pInternalState = false;
	}

private:
	bool	*m_pInternalState;
};
	

//
// CFunctionBarrier
//

class CFunctionBarrier
{
public:
	CFunctionBarrier(const SFunctionInfo &func)	{ CurrentThreadTracker().Push( func ); }
	~CFunctionBarrier()							{ CurrentThreadTracker().Pop(); }
};


//
// CProfiler
//

class CProfiler
{
public:
	void					Dump(const tchar *pszFileName, tchar delimiter = _T( '#' ));
	static CProfiler&		Instance();
	static void*			OperatorNew(size_t bytes);
	static void				OperatorDelete(void *pMemory);

private:
	friend class CThreadTracker;

	void					RegisterThreadTracker(CThreadTracker &tracker);
	static void				DumpFieldNames(tostream &os, tchar delimiter);
	static void				DumpEntry(tostream &os, const SFunctionInfo &func, const SMemoryUsageReport &mur, tchar delimiter);

private:
	typedef std::list<CThreadTracker*>	TTrackerList;

	CCriticalSection		m_csTrackers;
	TTrackerList			m_trackers;
};

inline CProfiler&			Profiler()				{ return CProfiler::Instance(); }


//
// Custom allocator
//

void*	AllocateMemory(size_t bytes);
void	FreeMemory(void *pMemory);

} // mp


//
// Macros
//

#define M_MEMORYPROFILE_FUNC													\
	static const mp::SFunctionInfo __MemoryProfiler_Func( _T( __FUNCSIG__ ),	\
																_T( __FILE__ ),	\
																__LINE__ );		\
	mp::CFunctionBarrier __MemoryProfiler_Barrier( __MemoryProfiler_Func );

#define M_MEMORYPROFILE_OPERATOR_NEW				\
	void* operator new(size_t bytes)				\
	{												\
		return mp::CProfiler::OperatorNew( bytes );	\
	}												\
	void operator delete(void *pMemory)				\
	{												\
		mp::CProfiler::OperatorDelete( pMemory );	\
	}												\
	void* operator new[](size_t bytes)				\
	{												\
		return operator new( bytes );				\
	}												\
	void operator delete[](void *pMemory)			\
	{												\
		operator delete( pMemory );					\
	}

#define M_MEMORYPROFILE_DEFAULT_ALLOCATOR												\
	namespace mp																		\
	{																					\
		void*	AllocateMemory(size_t bytes)	{ return std::malloc( bytes ); }		\
		void	FreeMemory(void *pMemory)		{ if( pMemory ) std::free( pMemory ); }	\
	}
