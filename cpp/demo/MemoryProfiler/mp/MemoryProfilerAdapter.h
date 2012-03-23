#pragma once

/**
 * ID:     122611
 * Author: zhouweidi
 */

#include <algorithm>
#include <list>
#include <set>
#include <map>
#include <iosfwd>
#include <cassert>
#include <tchar.h>
#include <Windows.h>

#if defined( _UNICODE ) || defined( UNICODE )
	#define MP_UNICODE
#else	// ANSI
	#define MP_ANSI
#endif

#define __M_BASE_Check( cond )	\
	if( !( cond ) )				\
	{							\
		assert( 0 );			\
		exit( -1 );				\
	}

namespace mp
{

//
// Common types
//

#if defined( MP_UNICODE )
	typedef wchar_t			tchar;
	typedef std::wostream	tostream;
	typedef std::wofstream	tofstream;
#else	// ANSI
	typedef char			tchar;
	typedef std::ostream	tostream;
	typedef std::ofstream	tofstream;
#endif

typedef SSIZE_T				ssize_t;


//
// CCriticalSection
//

class CCriticalSection
{
public:
	CCriticalSection()		{ ::InitializeCriticalSection( &m_cs ); }
	~CCriticalSection()		{ ::DeleteCriticalSection( &m_cs ); }

	void Lock()				{ ::EnterCriticalSection( &m_cs ); }
	void Unlock() throw()	{ ::LeaveCriticalSection( &m_cs ); }

private:
	CCriticalSection(const CCriticalSection&);
	CCriticalSection&	operator =(const CCriticalSection&);

private:
	CRITICAL_SECTION	m_cs;
};


//
// CLock
//

class CLock
{
public:
	explicit CLock(CCriticalSection &obj)
		: m_pObject( &obj )	{ obj.Lock(); }
	~CLock()				{ m_pObject->Unlock(); }

private:
	CLock(const CLock&);
	CLock&	operator =(const CLock&);

private:
	CCriticalSection	*m_pObject;
};


//
// Interlocked
//

#define __BASE_DEFINE_Interlocked_Function_2P( name, bit, func, type )	\
	template<typename T1, typename T2>									\
	static T1 name(volatile T1 *pTarget, T2 value)						\
	{																	\
		assert( pTarget );												\
																		\
		static_assert( sizeof( T1 ) == bit / 8 &&						\
					   sizeof( T2 ) == bit / 8,							\
					   "Type is not supported" );						\
																		\
		type r = func( reinterpret_cast<volatile type*>( pTarget ),		\
					   *reinterpret_cast<type*>( &value ) );			\
																		\
		return *reinterpret_cast<T1*>( &r );							\
	}

template<size_t n>
struct SInterlockedTraits;

template<>
struct SInterlockedTraits<4>
{
	__BASE_DEFINE_Interlocked_Function_2P( Add, 32, _InterlockedExchangeAdd,	long    )
};

template<>
struct SInterlockedTraits<8>
{
	__BASE_DEFINE_Interlocked_Function_2P( Add, 64, _InterlockedExchangeAdd64,	__int64 )
};

template<typename T1, typename T2>
inline T1 Interlocked_ExchangeAdd(volatile T1 *pTarget, T2 value)
{
	return SInterlockedTraits<sizeof(T1)>::Add( pTarget, value );
}

} // mp
