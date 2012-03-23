// MathDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <assert.h>
#include <windows.h>
#include <xTickCount.h>
#include <ProcessInfo.h>
#include <iostream>
using namespace std;

//////////////////////////////////////////////////////////////////////////
//线段经过格子算法
#include <vector>
#include <algorithm>
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

struct OBJPOS
{
	int x;
	int y;
	int z;
	OBJPOS():x(0),y(0){}
	OBJPOS(int _x, int _y):x(_x),y(_y){}
	bool operator == (const OBJPOS& a) const { return x==a.x && y==a.y; }

	OBJPOS& operator = (const OBJPOS& a) { x=a.x; y=a.y; return *this; }
};
inline bool operator < (const OBJPOS& a, const OBJPOS& b) { return (a.x<b.x) || ((a.x>b.y) && (a.y<b.y)); }

typedef std::vector<OBJPOS> OBJPOS_VEC;
bool Add2Vec(OBJPOS_VEC& vec,const OBJPOS& x)
{
 	if (std::find(vec.begin(), vec.end(), x) != vec.end())
 		return false;
	vec.push_back(x);
	return true;
}

int Distance(const OBJPOS& pos1, const OBJPOS& pos2)
{
	const int nOffsetX = abs(pos1.x - pos2.x);
	const int nOffsetY = abs(pos1.y - pos2.y);
	return max(nOffsetX, nOffsetY);
}

class CalcDistance
{
public:
	CalcDistance(const OBJPOS& posStart):m_posStart(posStart){}
	bool operator() (const OBJPOS& pos1, const OBJPOS& pos2)	{ return Distance(m_posStart,pos1) < Distance(m_posStart,pos2); }
protected:
	const OBJPOS& m_posStart;
};

struct OBJLINE
{
	OBJPOS posStart;
	OBJPOS posEnd;
};

//线段相交点
bool GetCrossPoint(const OBJLINE& line1, const OBJLINE& line2, OBJPOS& posCrossPoint)
{
//一条线段的两个端点为A(x1,y1),B(x2,y2),另一条线段的两个端点为C(x3,y3),D(x4,y4)
//	d   =   (y2-y1)(x4-x3)-(y4-y3)(x2-x1)
// 	x0   =   [(x2-x1)*(x4-x3)*(y3-y1)+(y2-y1)*(x4-x3)*x1-(y4-y3)*(x2-x1)*x3]/d
// 	y0   =   [(y2-y1)*(y4-y3)*(x3-x1)+(x2-x1)*(y4-y3)*y1-(x4-x3)*(y2-y1)*y3]/(-d)
	const int d = (line1.posEnd.y-line1.posStart.y)*(line2.posEnd.x-line2.posStart.x)
		-(line2.posEnd.y-line2.posStart.y)*(line1.posEnd.x-line1.posStart.x);
	if(d == 0)	//平行或重合
		return false;
	posCrossPoint.x = ((line1.posEnd.x-line1.posStart.x)*(line2.posEnd.x-line2.posStart.x)*(line2.posStart.y-line1.posStart.y)
		+(line1.posEnd.y-line1.posStart.y)*(line2.posEnd.x-line2.posStart.x)*line1.posStart.x
		-(line2.posEnd.y-line2.posStart.y)*(line1.posEnd.x-line1.posStart.x)*line2.posStart.x)/d;
	posCrossPoint.y = -1*((line1.posEnd.y-line1.posStart.y)*(line2.posEnd.y-line2.posStart.y)*(line2.posStart.x-line1.posStart.x)
		+(line1.posEnd.x-line1.posStart.x)*(line2.posEnd.y-line2.posStart.y)*line1.posStart.y
		-(line2.posEnd.x-line2.posStart.x)*(line1.posEnd.y-line1.posStart.y)*line2.posStart.y)/d;

	return true;
}

//线段和x=n y=n直线的焦点
bool GetCrossPoint(const OBJLINE& line, OBJPOS_VEC& vecPos)
{
	OBJLINE line2;
	OBJPOS posCross;
	
	const int nX1 = min(line.posStart.x, line.posEnd.x);
	const int nX2 = max(line.posStart.x, line.posEnd.x);
	const int nY1 = min(line.posStart.y, line.posEnd.y);
	const int nY2 = max(line.posStart.y, line.posEnd.y);

	vecPos.reserve(Distance(line.posStart, line.posEnd));

	//和x=n的焦点
	if (line.posStart.x != line.posEnd.x)
	{
		const int nVectorX = line.posStart.x>line.posEnd.x ? -1 : 1;	//轴向量
		int nX = line.posStart.x + nVectorX;	//过滤起点
		const int nOffsetX = abs(line.posStart.x - line.posEnd.x);
		for (int i=0; i<nOffsetX; ++i)
		{
			line2.posStart.x = line2.posEnd.x = nX;
			line2.posStart.y = nY1-1;
			line2.posEnd.y = nY2+1;
			if(GetCrossPoint(line, line2, posCross))
				Add2Vec(vecPos, posCross);
			nX += nVectorX;
		}
	}

	//和y=n的焦点
	if (line.posStart.y != line.posEnd.y)
	{
		const int nVectorY = line.posStart.y>line.posEnd.y ? -1 : 1;	//轴向量
		int nY = line.posStart.y + nVectorY;	//过滤起点
		const int nOffsetY = abs(line.posStart.y - line.posEnd.y);
		for (int i=0; i<nOffsetY; ++i)
		{
			line2.posStart.y = line2.posEnd.y = nY;
			line2.posStart.x = nX1-1;
			line2.posEnd.x = nX2+1;
			if(GetCrossPoint(line, line2, posCross))
				Add2Vec(vecPos, posCross);
			nY += nVectorY;
		}
	}

	std::sort(vecPos.begin(),vecPos.end(), CalcDistance(line.posStart));
// 	//unique()将进行排序，将重复的元素移动到队列后面
// 	OBJPOS_VEC::iterator new_end = std::unique(vecPos.begin(),vecPos.end());
// 
// 	//队列前面的元素都是唯一的，现在删掉new_end后面的元素
// 	vecPos.erase(new_end,vecPos.end());

	return true;
}

void PressCrossPointDemo()
{
	const int COUNT = 100000;
	xTickCount oCount;
	oCount.Start();
	for (int i=0; i<COUNT; ++i)
	{
		OBJLINE line;
		OBJPOS_VEC vecPos;
		line.posStart.x = 1;
		line.posStart.y = 1;
		line.posEnd.x = 2;
		line.posEnd.y = 2;
		GetCrossPoint(line, vecPos);
	}
	oCount.Finish();
	cout << COUNT << " use " << oCount.GetUsedMS() << "ms" << endl;
}

void CrossPointDemo()
{
	OBJLINE line;
	OBJPOS_VEC vecPos;

	//y = 1 [1,1]->[2,2]
	line.posStart.x = 1;
	line.posStart.y = 1;
	line.posEnd.x = 2;
	line.posEnd.y = 2;
	GetCrossPoint(line, vecPos);
	assert(std::find(vecPos.begin(), vecPos.end(), OBJPOS(2,2)) != vecPos.end());
	vecPos.clear();

	//y = 1 [3,1]->[5,1]
	line.posStart.x = 3;
	line.posStart.y = 1;
	line.posEnd.x = 5;
	line.posEnd.y = 1;
	GetCrossPoint(line, vecPos);
	assert(std::find(vecPos.begin(), vecPos.end(), OBJPOS(4,1)) != vecPos.end());
	vecPos.clear();
	
	//x = 1 [1,3]->[1,5]
	line.posStart.x = 1;
	line.posStart.y = 3;
	line.posEnd.x = 1;
	line.posEnd.y = 5;
	GetCrossPoint(line, vecPos);
	assert(std::find(vecPos.begin(), vecPos.end(), OBJPOS(1,4)) != vecPos.end());
	vecPos.clear();

	//y = 2x +1 [1,3]->[3,7]
	line.posStart.x = 1;
	line.posStart.y = 3;
	line.posEnd.x = 3;
	line.posEnd.y = 7;
	GetCrossPoint(line, vecPos);
	vecPos.clear();
	
	std::swap(line.posStart, line.posEnd);
	GetCrossPoint(line, vecPos);
	vecPos.clear();

	//y = -2x +1 [1,-1]->[3,-5]
	line.posStart.x = 1;
	line.posStart.y = -1;
	line.posEnd.x = 3;
	line.posEnd.y = -1;
	GetCrossPoint(line, vecPos);
}



int _tmain(int argc, _TCHAR* argv[])
{
	ProcessInfo process;
	PROCESS_MEMORY_COUNTERS stMem1;
	process.GetMemoryInfo(stMem1);
	typedef std::vector<OBJPOS_VEC> OBJPOS_VEC_VEC;
	OBJPOS_VEC_VEC vec;
	for (int i=0; i<3000;++i)
	{
		OBJPOS_VEC vecPos;
		vecPos.reserve(20);
		vec.push_back(vecPos);
	}
	PROCESS_MEMORY_COUNTERS stMem2;
	process.GetMemoryInfo(stMem2);
	char buf[1024] = {0};
	_snprintf(buf, sizeof(buf)-1, "Mem[%dKB] VMEM[%dKB]", 
		(stMem2.WorkingSetSize-stMem1.WorkingSetSize)/1024, (stMem2.PagefileUsage-stMem1.PagefileUsage)/1024);
	OutputDebugStr(buf);
	return 0;
}

