/**
 * ID:     122611
 * Author: zhouweidi
 */

#include "MemoryProfilerAdapter.h"
#include "MemoryProfiler.h"
#include <fstream>

namespace mp
{

//
// SMemoryUsage
//

inline void SMemoryUsage::operator +=(const SMemoryUsage &rhs)
{
	allocatedBytes	+= rhs.allocatedBytes;
	freedBytes		+= rhs.freedBytes;
}


//
// CCallTreeNode
//

CCallTreeNode* CCallTreeNode::Push(const SFunctionInfo *pFunc)
{
	assert( pFunc );

	if( !m_pLastSubCall || m_pLastSubCall->m_pFunc != pFunc )
	{
		auto it = std::find_if( m_subs.begin(), m_subs.end(), 
								[pFunc](const CCallTreeNode &node) { return node.m_pFunc == pFunc; } );

		if( it != m_subs.end() )
			m_pLastSubCall = &*it;
		else
		{
			m_subs.push_back( CCallTreeNode( this, pFunc ) );
			m_pLastSubCall = &m_subs.back();
		}
	}

	return m_pLastSubCall;
}

void CCallTreeNode::OnAllocate(size_t bytes)
{
	if( !bytes )
		return;

	Interlocked_ExchangeAdd( &m_exclusive.allocatedBytes, bytes );
}

void CCallTreeNode::OnFree(size_t bytes)
{
	if( !bytes )
		return;

	Interlocked_ExchangeAdd( &m_exclusive.freedBytes, bytes );
}

void CCallTreeNode::GenerateReport(TReport &report) const
{
	GenerateSelfReport( report );

	for( auto it = m_subs.begin(); it != m_subs.end(); ++it )
		it->GenerateReport( report );
}

void CCallTreeNode::GenerateSelfReport(TReport &report) const
{
	std::set<const SFunctionInfo*> processedFuncs;

	{
		processedFuncs.insert( m_pFunc );

		SMemoryUsageReport &mur = report[m_pFunc];

		mur.exclusive += m_exclusive;
		mur.inclusive += m_exclusive;
	}

	for( CCallTreeNode *pNode = m_pParent; pNode; pNode = pNode->m_pParent )
	{
		if( processedFuncs.find( pNode->m_pFunc ) != processedFuncs.end() )
			continue;

		processedFuncs.insert( pNode->m_pFunc );

		SMemoryUsageReport &mur = report[pNode->m_pFunc];
		mur.inclusive += m_exclusive;
	}
}


//
// CCallTree
//

const SFunctionInfo CCallTree::s_global( _T( "Global" ), _T( "No source" ), 0 );


//
// CThreadTracker
//

CThreadTracker::CThreadTracker()
	: m_internalState( true )
{
	Profiler().RegisterThreadTracker( *this );

	m_internalState = false;
}

void CThreadTracker::Push(const SFunctionInfo &func)
{
	CInternalState guard( *this );

	m_callTree.Push( func );
}

void CThreadTracker::Pop()
{
	CInternalState guard( *this );

	m_callTree.Pop();
}

void CThreadTracker::GenerateReport(TReport &report) const
{
	m_callTree.Root()->GenerateReport( report );
}

CThreadTracker&	CThreadTracker::PerThreadInstance()
{
	__declspec( thread ) static CThreadTracker *pThreadTracker;

	if( !pThreadTracker )
	{
		pThreadTracker = static_cast<CThreadTracker*>( std::malloc( sizeof( CThreadTracker ) ) );
		new( pThreadTracker ) CThreadTracker;
	}

	return *pThreadTracker;
}


//
// CProfiler
//

CProfiler& CProfiler::Instance()
{
	static CProfiler instance;
	return instance;
}

void CProfiler::RegisterThreadTracker(CThreadTracker &tracker)
{
	CLock lock( m_csTrackers );

	m_trackers.push_back( &tracker );
}

void CProfiler::Dump(const tchar *pszFileName, tchar delimiter)
{
	if( !pszFileName )
		throw std::runtime_error( "Null memory profiler dump file name" );

	CInternalState internalState;

	CLock lock( m_csTrackers );

	// Open dump file
	tofstream file( pszFileName );
	if( !file )
		throw std::runtime_error( "Open dump file faield" );

	// Collect all function info
	TReport report;
	for( auto itTracker = m_trackers.begin(); itTracker != m_trackers.end(); ++itTracker )
	{
		CThreadTracker *pTracker = *itTracker;

		pTracker->GenerateReport( report );
	}

	// Dump them
	DumpFieldNames( file, delimiter );

	for( auto itEntry = report.begin(); itEntry != report.end(); ++itEntry )
		DumpEntry( file, *itEntry->first, itEntry->second, delimiter );
}

void CProfiler::DumpFieldNames(tostream &os, tchar delimiter)
{
	os << _T( "Name" )			<< delimiter
	   << _T( "Source" )		<< delimiter
	   << _T( "ExAllocated" )	<< delimiter
	   << _T( "ExFreed" )		<< delimiter
	   << _T( "ExHeld" )		<< delimiter
	   << _T( "InAllocated" )	<< delimiter
	   << _T( "InFreed" )		<< delimiter
	   << _T( "InHeld" )		<< _T( '\n' );
}

void CProfiler::DumpEntry(tostream &os, const SFunctionInfo &func, const SMemoryUsageReport &mur, tchar delimiter)
{
	os << func.name						<< delimiter
	   << func.file						<< _T( ',' )
	   << func.line						<< delimiter
	   << mur.exclusive.allocatedBytes	<< delimiter
	   << mur.exclusive.freedBytes		<< delimiter
	   << static_cast<ssize_t>( mur.exclusive.allocatedBytes ) - static_cast<ssize_t>( mur.exclusive.freedBytes )
										<< delimiter
	   << mur.inclusive.allocatedBytes	<< delimiter
	   << mur.inclusive.freedBytes		<< delimiter
	   << static_cast<ssize_t>( mur.inclusive.allocatedBytes ) - static_cast<ssize_t>( mur.inclusive.freedBytes )
										<< _T( '\n' );
}

#pragma pack( push, 1 )
struct SMemoryTag
{
	size_t			bytes;
	CCallTreeNode	*pNode;
};
#pragma pack( pop )

void* CProfiler::OperatorNew(size_t bytes)
{
	SMemoryTag *pTag = static_cast<SMemoryTag*>( AllocateMemory( bytes + sizeof( SMemoryTag ) ) );

	pTag->bytes	= bytes;

	CThreadTracker &tracker = CurrentThreadTracker();

	if( tracker.InternalState() )
		pTag->pNode	= 0;
	else
	{
		CCallTreeNode *pCurrent = tracker.Current();

		pTag->pNode = pCurrent;

		pCurrent->OnAllocate( bytes );
	}

	return pTag + 1;
}

void CProfiler::OperatorDelete(void *pMemory)
{
	if( !pMemory )
		return;

	SMemoryTag *pTag = static_cast<SMemoryTag*>( pMemory ) - 1;

	if( pTag->pNode )
		pTag->pNode->OnFree( pTag->bytes );

	FreeMemory( pTag );
}

} // mp
