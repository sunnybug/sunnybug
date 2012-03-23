
//Debug.h

//xbq 2005.10.20

//pre include windows needed to complile this file

#ifndef _Debug_H_
#define _Debug_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _DEBUG
#define DEBUG_BREAK() DebugBreak();
#else
#define DEBUG_BREAK()
#endif

// determine number of elements in an array (not bytes)
#define _countof_array(array) (sizeof(array)/sizeof(array[0]))


#endif