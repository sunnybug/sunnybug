// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__04DA717E_9195_4F55_B069_1C26987E01AF__INCLUDED_)
#define AFX_STDAFX_H__04DA717E_9195_4F55_B069_1C26987E01AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
//type
struct OBJPOS
{
	int x;
	int y;
	int z;
	OBJPOS(int _x, int _y, int _z):x(_x),y(_y),z(_z){}
	OBJPOS():x(0),y(0),z(0){}
	OBJPOS operator+(const OBJPOS& b) const
	{
		return OBJPOS(x+b.x, y+b.y, z+b.z);
	}
	bool operator == (const OBJPOS& a) const { return x==a.x && y==a.y; }
	OBJPOS& operator = (const OBJPOS& a) { x=a.x; y=a.y; return *this; }
};
inline bool operator < (const OBJPOS& a, const OBJPOS& b) { return (a.x<b.x) || ((a.x>b.y) && (a.y<b.y)); }

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__04DA717E_9195_4F55_B069_1C26987E01AF__INCLUDED_)
