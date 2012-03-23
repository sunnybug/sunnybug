#ifndef	COMMON_H
#define	COMMON_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "time.h"

#include <vector>
#include <utility>
void* _cdecl operator new(size_t nSize, LPCSTR lpszFileName, int nLine);
void _cdecl operator delete(void* p, LPCSTR lpszFileName, int nLine);

#include <time.h>
#include "logfile.h"
#include <assert.h>
#include "ModDump.h"	// BaseSupport
#include "DbgFunc.h"
#include "IniFile.h"

void	LogSave		(const char* fmt, ...);
void	LogSaveE	(PEXCEPTION_POINTERS pException, const char* fmt, ...);
/////////////////////////////////////////////////////////////////////////////
// function
/////////////////////////////////////////////////////////////////////////////
bool SafeCopy(char * pTarget, const char * pSource, int nBufLen);
bool AddChar(char* pTarget, char cData, int nBufLen);
//////////////////////////////////////////////////////////////////////
#define		TYPENAME	class
template<TYPENAME T>
inline void		safe_delete(T*& pT)			{ if(pT) delete pT; pT=NULL; }
template<TYPENAME T>
inline void		safe_release(T*& pT)		{ if(pT) pT->Release(); pT=NULL; }
#define SAFE_DELETE(ptr)	do{ try{ safe_delete(ptr); }catch(...){ LogSave("SAFE_DELETE(" #ptr ") in %s %d", __FILE__, __LINE__); } }while(0)
#define SAFE_RELEASE(ptr)	do{ try{ safe_release(ptr); }catch(...){ LogSave("SAFE_RELEASE(" #ptr ") in %s %d", __FILE__, __LINE__); } }while(0)

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x)	if( x!=NULL ) { delete [] x; x=NULL; }
#endif

//////////////////////////////////////////////////////////////////////
// ÆäËü

inline void	log_assert(const char* type, const char* str, const char* file, int line) { char szFuncDump[1024];LOGERROR("¡ï%s(%s)¡ï in %s, %d : %s", type, str, file, line, DumpFuncAddress(6, szFuncDump)); }

#ifdef	_DEBUG
#undef	ASSERT
#define		ASSERT(x)	(void)( (x) || (log_assert("ASSERT", #x, __FILE__, __LINE__),assert(!"(#x)"), 0) )
#define		CHECK(x)	do{ if(!(x)) { log_assert("CHECK", #x, __FILE__, __LINE__),assert(!(#x)); return; } }while(0)
#define		CHECKF(x)	do{ if(!(x)) { log_assert("CHECKF", #x, __FILE__, __LINE__),assert(!(#x)); return 0; } }while(0)
#define		CHECK_(r,x)	do{ if(!(x)) { log_assert("CHECK_", #x, __FILE__, __LINE__),assert(!(#x)); return (r); } }while(0)
#define		IF_NOT(x)	if( !(x) ? ( log_assert("IF_NOT", #x, __FILE__, __LINE__),assert(!(#x)),1 ) : 0 )
#define		IF_NOT_(x)	if( !(x) ? ( log_assert("IF_NOT_", #x, __FILE__, __LINE__),assert(!"(#x)"),1 ) : 0 )
#define		IF_OK(x)	if( (x) ? 1 : ( log_assert("IF_OK", #x, __FILE__, __LINE__),assert(!(#x)),0 ) )
#define		IF_OK_(x)	if( (x) ? 1 : ( log_assert("IF_OK_", #x, __FILE__, __LINE__),assert(!"(#x)"),0 ) )
#define		PURE_VIRTUAL_FUNCTION_0		= 0;
#define		PURE_VIRTUAL_FUNCTION		= 0;
#define		PURE_VIRTUAL_FUNCTION_(x)	= 0;
#define		PURE_VIRTUAL_FUNCTION_X		= 0;
#else // LOCAL_DEBUG
#undef	ASSERT
#define		ASSERT(x)	(void)( (x) || (log_assert("ASSERT", #x, __FILE__, __LINE__), 0) )
#define		CHECK(x)	do{ if(!(x)) { log_assert("CHECK", #x, __FILE__, __LINE__); return; } }while(0)
#define		CHECKF(x)	do{ if(!(x)) { log_assert("CHECKF", #x, __FILE__, __LINE__); return 0; } }while(0)
#define		CHECK_(r,x)	do{ if(!(x)) { log_assert("CHECK_", #x, __FILE__, __LINE__); return (r); } }while(0)
#define		IF_NOT(x)	if( !(x) ? ( log_assert("IF_NOT", #x, __FILE__, __LINE__),1 ) : 0 )
#define		IF_NOT_(x)	if( !(x) ? ( log_assert("IF_NOT_", #x, __FILE__, __LINE__),1 ) : 0 )
#define		IF_OK(x)	if( (x) ? 1 : ( log_assert("IF_OK", #x, __FILE__, __LINE__),0 ) )
#define		IF_OK_(x)	if( (x) ? 1 : ( log_assert("IF_OK_", #x, __FILE__, __LINE__),0 ) )
#define		PURE_VIRTUAL_FUNCTION_0		{ ASSERT(!"PURE_VIRTUAL_FUNCTION_0"); return 0; }
#define		PURE_VIRTUAL_FUNCTION		{ ASSERT(!"PURE_VIRTUAL_FUNCTION"); }
#define		PURE_VIRTUAL_FUNCTION_(x)	{ ASSERT(!"PURE_VIRTUAL_FUNCTION"); return x(); }
#define		PURE_VIRTUAL_FUNCTION_X		= 0;
#endif // LOCAL_DEBUG
#define		MYASSERT	ASSERT


//#define		DEBUG_TRY		try{
//#define		DEBUG_CATCH(s)			}catch(...){ char szFuncDump[1024];LogSave("CATCH: *** %s %d {" s "} crash! %s***", __FILE__, __LINE__, DumpFuncAddress(6, szFuncDump)); }
//#define		DEBUG_CATCH2(s,x)		}catch(...){ char szFuncDump[1024];LogSave("CATCH: *** %s %d {" s "} crash! %s***", __FILE__, __LINE__,(x), DumpFuncAddress(6, szFuncDump)); }
//#define		DEBUG_CATCH3(s,x,y)		}catch(...){ char szFuncDump[1024];LogSave("CATCH: *** %s %d {" s "} crash! %s***", __FILE__, __LINE__,(x),(y), DumpFuncAddress(6, szFuncDump)); }
//#define		DEBUG_CATCH4(s,x,y,z)	}catch(...){ char szFuncDump[1024];LogSave("CATCH: *** %s %d {" s "} crash! %s***", __FILE__, __LINE__,(x),(y),(z), DumpFuncAddress(6, szFuncDump)); }
//#define		DEBUG_CATCH_RUN(s,x)	}catch(...){ char szFuncDump[1024];LogSave("CATCH: *** %s %d {" s "} crash. %s***", __FILE__, __LINE__, DumpFuncAddress(6, szFuncDump)); (x); }
#ifndef _DEBUG
#define		DEBUG_TRY		try{
#else
#define		DEBUG_TRY {		
#endif

#ifndef _DEBUG
#define		DEBUG_CATCH(s)			}catch(EXCEPTION_POINTERS e){LogSaveE(&e,"CATCH: *** %s %d {" s "} crash! ***", __FILE__, __LINE__); }catch(...){ char szFuncDump[1024];LogSave("CATCH(...): *** %s %d {" s "} crash! %s***", __FILE__, __LINE__, DumpFuncAddress(6, szFuncDump)); }
#else
#define		DEBUG_CATCH(s)  }
#endif

#ifndef _DEBUG
#define		DEBUG_CATCH2(s,x)		}catch(EXCEPTION_POINTERS e){LogSaveE(&e,"CATCH: *** %s %d {" s "} crash! ***", __FILE__, __LINE__,(x)); }catch(...){ char szFuncDump[1024];LogSave("CATCH(...): *** %s %d {" s "} crash! %s***", __FILE__, __LINE__,(x), DumpFuncAddress(6, szFuncDump)); }
#else
#define		DEBUG_CATCH2(s,x) }
#endif

#ifndef _DEBUG
#define		DEBUG_CATCH3(s,x,y)		}catch(EXCEPTION_POINTERS e){LogSaveE(&e,"CATCH: *** %s %d {" s "} crash! ***", __FILE__, __LINE__,(x),(y)); }catch(...){ char szFuncDump[1024];LogSave("CATCH(...): *** %s %d {" s "} crash! %s***", __FILE__, __LINE__,(x),(y), DumpFuncAddress(6, szFuncDump)); }
#else
#define		DEBUG_CATCH3(s,x,y)	}
#endif

#ifndef _DEBUG
#define		DEBUG_CATCH4(s,x,y,z)	}catch(EXCEPTION_POINTERS e){LogSaveE(&e,"CATCH: *** %s %d {" s "} crash! ***", __FILE__, __LINE__,(x),(y),(z)); }catch(...){ char szFuncDump[1024];LogSave("CATCH(...): *** %s %d {" s "} crash! %s***", __FILE__, __LINE__,(x),(y),(z), DumpFuncAddress(6, szFuncDump)); }
#else
#define		DEBUG_CATCH4(s,x,y,z)	}
#endif

#ifndef _DEBUG
#define		DEBUG_CATCH_RUN(s,x)	}catch(EXCEPTION_POINTERS e){LogSaveE(&e,"CATCH: *** %s %d {" s "} crash. ***", __FILE__, __LINE__); (x); }catch(...){ char szFuncDump[1024];LogSave("CATCH(...): *** %s %d {" s "} crash. %s***", __FILE__, __LINE__, DumpFuncAddress(6, szFuncDump)); (x); }
#else
#define		DEBUG_CATCH_RUN(s,x)	}
#endif

#define		LOCKTHREAD		CSingleLock xLock(&m_xCtrl, true)

// int xCount = 0;
// while(...) { ...; DEAD_LOOP_BREAK(xCount, 10000) }
#define		DEAD_LOOP_BREAK(x,n)	{ if(++(x) > (n)){ ASSERT(!"DEAD_LOCK_BREAK"); break; } }


#define TRUE_TRY DEBUG_TRY
#define TRUE_CATCH(s) DEBUG_CATCH(s)

//#define TRUE_TRY __try{
//#define TRUE_CATCH(s) }__except(CModDump::DumpExcetionInfo(GetExceptionInformation())){ LogSave("EXCEPTION: *** %s %d {" s "} crash! ***", __FILE__, __LINE__); }


#endif // COMMON_H
