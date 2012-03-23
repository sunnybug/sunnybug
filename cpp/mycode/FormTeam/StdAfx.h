// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9094D1B1_7B79_4D26_84DE_434D9A337815__INCLUDED_)
#define AFX_STDAFX_H__9094D1B1_7B79_4D26_84DE_434D9A337815__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <assert.h>

//std
#include <list>
#include <vector>

#pragma warning(disable:4786)

//////////////////////////////////////////////////////////////////////////
//type
class OBJPOS : public CPoint
{
public:
	OBJPOS():CPoint(0, 0){}
	OBJPOS(int x, int y):CPoint(x, y){}
	OBJPOS(const CPoint& pt):CPoint(pt){}

	OBJPOS operator+(const OBJPOS& b) const
	{
		return OBJPOS(x+b.x, y+b.y);
	}
	bool operator == (const OBJPOS& a) const { return x==a.x && y==a.y; }
	OBJPOS& operator = (const OBJPOS& a) { x=a.x; y=a.y; return *this; }
};
inline bool operator < (const OBJPOS& a, const OBJPOS& b) { return (a.x<b.x) || ((a.x>b.y) && (a.y<b.y)); }

class OBJSIZE : public CSize
{
public:
	OBJSIZE():CSize(0, 0){}
};
typedef DWORD OBJID;
typedef std::vector<OBJPOS> PathList;

struct  Pix : public CPoint
{
	Pix():CPoint(0, 0){}
	Pix(int x, int y):CPoint(x, y){}
};

struct MatrixIndex
{
	int x;
	int y;
	MatrixIndex():x(0),y(0){}
	MatrixIndex(int usX, int usY):x(usX),y(usY){}

	bool operator==(const MatrixIndex& b) const
	{
		return x==b.x && y==b.y;
	}
	MatrixIndex operator-(const MatrixIndex& b) const
	{
		return MatrixIndex(x-b.x, y-b.y);
	}
};

typedef std::list<int> DirList;

//////////////////////////////////////////////////////////////////////////
//const
const int MATRIX_WIDTH		= 20;
const int MATRIX_HEIGHT	= 20;

const CPoint POINT_NONE(-1, -1);
const OBJPOS OBJPOS_NONE(0, 0);
const OBJID ID_NONE = 0;

//从12点方向开始，逆时针的8个方向
const int _DELTA_X[9] = {0, -1, -1, -1, 0, 1, 1, 1, 0};	
const int _DELTA_Y[9] = {1, 1, 0, -1, -1, -1, 0, 1, 0};
const int MAX_DIRSIZE = 8;
inline int GetDirByPos(const OBJPOS& ptStar, const OBJPOS& ptEnd)
{
	if (ptStar.x < ptEnd.x)
	{
		if (ptStar.y < ptEnd.y)
			return 7;
		else if (ptStar.y > ptEnd.y)
			return 5;
		else
			return 6;
	}
	else if (ptStar.x > ptEnd.x)
	{
		if (ptStar.y < ptEnd.y)
			return 1;
		else if (ptStar.y > ptEnd.y)
			return 3;
		else
			return 2;
	}
	else
	{
		if (ptStar.y < ptEnd.y)
			return 0;
		else if (ptStar.y > ptEnd.y)
			return 4;
	}
	return 8;
}


inline int Distance(const OBJPOS& pos1, const OBJPOS& pos2)
{
	const int nOffsetX = abs(pos1.x - pos2.x);
	const int nOffsetY = abs(pos1.y - pos2.y);
	return max(nOffsetX, nOffsetY);
}

#include "GameWord.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9094D1B1_7B79_4D26_84DE_434D9A337815__INCLUDED_)
