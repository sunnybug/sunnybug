// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <assert.h>

#define	CHECKF(x)	do{ if(!(x)) {assert(!("#x")); return 0; } }while(0)
#define	IF_NOT(x)	if( !(x) ?	  (assert(!"IF_NOT("#x")"),1) : 0 )
#define	IF_OK(x)	if( (x) ? 1 : (assert( !"IF_OK("#x")"),0) )


// TODO: reference additional headers your program requires here
