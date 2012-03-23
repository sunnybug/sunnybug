#ifndef _BASEFUNC_H
#define _BASEFUNC_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma	warning(disable:4786)
#include "common.h"
#include "BaseType.h"
#include <vector>
using namespace std;


String	DumpBuffer(const char* buf, int nSize);
void	DateTime(char * buf20, time_t tInput = 0);		// 0 : curr time
bool	FormatDateTime(char* buf, const char* szFormat, time_t tInput = 0);		// szFormat: "%04d-%02d-%02d %02d:%02d:%02d"
void	DateTimeStamp(char * buf15, time_t tInput = 0);		// 0 : curr time
int		DateStamp(time_t tTime=0);
int		DateStampPass(int nDate, int nDays);	// return -1: error
bool	IsActiveTime(time_t tCurr, unsigned long nFormat);
bool	IsTimeRange(unsigned long nBegin, unsigned long nEnd, time_t tCurr=0);
inline long	GetCurrTime()				{ return time(NULL); }
char*	StrStrCh(const char* string, const char* strCharSet);
enum TIME_TYPE{ TIME_MILLISECOND=0, TIME_SECOND, TIME_MINUTE, TIME_DAY, TIME_DAYTIME, TIME_STAMP };
DWORD TimeGet(TIME_TYPE type=TIME_MILLISECOND, int nOffsetSecs=0);
//DWORD	TimeGet(void);
DWORD	SysTimeGet(void);
DWORD	SysTimeGetEx(void);
DWORD	SysTimeGetMin(void);
int		AddHour(int dwCurYYMMDDhh, int nHour);
int		AddMin(int dwCurYYMMDDhhmm, int nMin);
int		Time_DaysDiff( int nYYMMDD1, int nYYMMDD2);
int		Time_HourDiff( int YYMMDDhh1, int YYMMDDhh2);

DWORD	GetYYMMDDHHMM(time_t tCur=0);

inline long	GetClock()		{ return clock(); }
int		RandGet		(int nMax, BOOL bReset=false);
double  RandomRateGetSinSqrtTwo(double dRange);
double  RandomRateGetSinSqrtOne(double dRange);

inline bool	Probability(int nProb, int nBase=100)			{ return (::RandGet(nBase) < nProb); }
void	DDALineEx(int x0, int y0, int x1, int y1, vector<POINT>& vctPoint);
void	DDALineExtend(int x0, int y0, int x1, int y1, int nRange, vector<POINT>& vctPoint);
int	CombinePercent(int nAdj1, int nAdj2, int nBase=100);


void	LogSave		(const char* fmt, ...);
void	MyLogSave	(const char* pszName, const char* fmt, ...);

void	DebugSave	(const char* fmt, ...);
void	GmLogSave	(const char* fmt, ...);
void	ActionLogSave	(char * szFileName, const char* fmt, ...);//1022action修改 tcp 2007-4-6 modify
void	MsgLogSave	(const char* pszFileName, const char* fmt, ...);
void	QaLogSave	(const char* pszFileName, const char* fmt, ...);

BOOL	StringCheck	(char* pszString, BOOL bNewLineEnable=false);
BOOL	TalkStrCheck(char* pszString, BOOL bNewLineEnable=false);
BOOL	DisableWordCheck(char* pszString, BOOL bNewLineEnable=false);
BOOL	NameStrCheck(char* pszString);

double	RandomRateGet	(double dRange);
int		Double2Int		(double dValue);

inline int	AddToTop(int nData, int nAdd, int nTop) { int res=nData+nAdd; if(nAdd>0 && res<nData || res>nTop) return nTop; else return res; }
inline int	MulDivUp(int nVal, int nMul, int nDiv) { CHECKF(nDiv!=0); return ((nVal-1) * nMul / nDiv) + 1; }

enum DIFF_TIME_TYPE{DIFF_TIME_SECOND, DIFF_TIME_MINUTE, DIFF_TIME_HOUR, DIFF_TIME_DAY};
DWORD GetDiffDay(DIFF_TIME_TYPE diffTimeType, UINT uStartTime, UINT uEndDayTime);//返回时间差(精确到分),uStartTime、uEndDayTime均为YYMMDDHHMM的日期格式


/////////////////////////////////////////////////////////////////////////////
class CTimeCostChk
{
public:
	CTimeCostChk(const char* pszStr, DWORD tmChk) : m_tmChk(tmChk) 
	{
		m_tmBegin	= ::TimeGet();
		m_pszStr	= pszStr;
	}
	
	~CTimeCostChk(void)
	{
		DWORD tmCost = ::TimeGet()-m_tmBegin;
		if (tmCost > m_tmChk)
		{
			if (m_pszStr)
				::LogSave("Overtime (%u) when chk %s", tmCost, m_pszStr); 
			else
				::LogSave("Overtime (%u)", tmCost); 
		}
	}

private:
	DWORD m_tmChk;
	const char* m_pszStr;

	DWORD m_tmBegin;
};

/////////////////////////////////////////////////////////////////////////////
template<TYPENAME T>
inline T	CutRange(T n, T min, T max) { return (n<min) ? min : ((n>max) ? max : n); }
template<TYPENAME T>
inline T	CutCeiling(T x, T y) { return (x <= y) ? x : y; }
template<TYPENAME T>
inline T	CutFloor(T x, T y) { return (x >= y) ? x : y; }

//另增加MAX和MIN，以代替_max和_min。(因为_max和_min是用宏实现的，不能用于参数是函数调用的情况。
template <TYPENAME T>
inline T MIN(T x, T y) { return (x <= y) ? x : y; }
template <TYPENAME T>
inline T MAX(T x, T y) { return (x >= y) ? x : y; }

//(已废弃) 命名风格不好，建议改用上面的两种
template<TYPENAME T>
inline T	CutOverflow(T x, T y) { return (x <= y) ? x : y; }
template<TYPENAME T>
inline T	CutTrail(T x, T y) { return (x >= y) ? x : y; }

template <class T>
inline void RandGetIndexByLimitCount(vector<T>&set, const int nLimitCount)
{//指定数量，随机挑选的玩家2007-2-28 tcp add 经测试在开发机器上本算法耗时3000集合最大耗时1毫秒左右
	if(nLimitCount<1)
		return;
	
	const int nConstCount = set.size();	
	if(nLimitCount>=nConstCount)
		return;

	int nIndex =0;
	int nIndexSecond =0;

	T t;
	int nCount = nConstCount;
	if(1==nLimitCount)
	{
		nIndex= RandGet(nCount); 
		nCount--;		
		if(!(nIndex>=0 && nIndex < nConstCount))
		{
			return;
		}
		if(!(nCount>=0 && nCount < nConstCount))
		{
			return;
		}
		if(nCount==nIndex)
			return;
		t=set[nCount];
		set[nCount]= set[nIndex];
		set[nIndex] = t;
		return;
	}
	if(nLimitCount <= (nConstCount/2))
	{
		for(int i=0; i < nLimitCount; i++)
		{
			nIndex= RandGet(nCount); 
			nIndexSecond = RandGet(nCount); 			
			nCount--;		  		   		  
			if(!(nIndex>=0 && nIndex < nConstCount))
			{
				continue;
			}
			
			if(!(nCount>=0 && nCount < nConstCount))
			{
				continue;
			}
			
			if(nCount == nIndex)
				continue;
			
			if(!(nIndexSecond>=0 && nIndexSecond < nConstCount))
			{
				continue;
			}
			
			t=set[nCount];
			set[nCount]= set[nIndex];
			set[nIndex]= set[nIndexSecond];
			set[nIndexSecond] = t;
		}
	}
	else
	{
		int nNoBeSelectCount = nConstCount - nLimitCount;
		nCount = 0; 
		
		for(int i=0; i<nNoBeSelectCount; i++)
		{
			nIndex= RandGet(nConstCount-nCount); 
			if(!(nIndex>=0 && nIndex<nConstCount))
			{
				continue;
			}
			if(!(nCount>=0 && nCount<nConstCount))
			{
				continue;
			}
			if(nCount == nIndex)
			{
				nCount++;
				continue;
			}
			nIndexSecond = RandGet(nConstCount-nCount); 
			if(!(nIndexSecond>=0 && nIndexSecond < nConstCount))
			{
				continue;
			}
			nCount++;
			t=set[nCount];
			set[nCount]= set[nIndex];
			set[nIndex]= set[nIndexSecond];
			set[nIndexSecond] = t;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
inline unsigned int	ExchangeShortBits(unsigned long nData, int nBits)
{
	ASSERT(nBits >= 0 && nBits < 16);
	nData	&= 0xFFFF;
	return ((nData>>nBits) | (nData<<(16-nBits))) & 0xFFFF;
}

inline unsigned int	ExchangeLongBits(unsigned long nData, int nBits)
{
	ASSERT(nBits >= 0 && nBits < 32);
	return (nData>>nBits) | (nData<<(32-nBits));
}

//////////////////////////////////////////////////////////////////////
inline bool IsInValidDateTime(SYSTEMTIME st)
{
	//CHECKF(st.wYear>=1980);
	switch(st.wMonth)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			CHECKF(st.wDay>=1 && st.wDay<=31);				
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			CHECKF(st.wDay>=1 && st.wDay<=30);				
			break;
		case 2:
			if((st.wYear%4==0 && st.wYear%100!=0)  || st.wYear%400==0)
			{
				CHECKF(st.wDay>=1 && st.wDay<=29);								
			}
			else
			{
				CHECKF(st.wDay>=1 && st.wDay<=28);												
			}
			break;
		default:
			CHECKF(0);
	}	
	CHECKF(st.wHour>=0 && st.wHour<=23);			
	CHECKF(st.wMinute>=0 && st.wMinute<=59);			
	CHECKF(st.wSecond>=0 && st.wSecond<=59);	
	return true;
}

inline int SystemTime2Days(SYSTEMTIME st)
{//从公元元年开始	
	int nDay		=	st.wYear*365;
	int nTime4	=	st.wYear/4;
	int nTime100	=	st.wYear/100;
	int nTime400	=	st.wYear/400;
	int nTimes	=	nTime4-(nTime100-nTime400);
	nDay			+=	nTimes+1;
	bool bFlag	=	false;
	
	if((st.wYear%4==0 && st.wYear%100!=0) || st.wYear%400==0)
	{
		bFlag	=	true;	   
	}
	
	if(bFlag)
		nDay--;
	
	int nFebDays= bFlag ? 29:28;
	switch(st.wMonth)
	{
	   case 1:
		   nDay	+=	st.wDay;
		   break;
	   case 2:
		   nDay	+= 31+st.wDay;
		   break;		   
	   case 3:
		   nDay	+= 31+nFebDays+st.wDay;
		   break;		   
	   case 4:
		   nDay	+= 31+nFebDays+31+st.wDay;
		   break;		   
	   case 5:
		   nDay	+= 31+nFebDays+31+30+st.wDay;
		   break;		   
	   case 6:
		   nDay	+= 31+nFebDays+31+30+31+st.wDay;
		   break;		   
	   case 7:
		   nDay	+= 31+nFebDays+31+30+31+30+st.wDay;
		   break;		   
	   case 8:
		   nDay	+= 31+nFebDays+31+30+31+30+31+st.wDay;
		   break;		   
	   case 9:
		   nDay	+= 31+nFebDays+31+30+31+30+31+31+st.wDay;
		   break;		   
	   case 10:
		   nDay	+= 31+nFebDays+31+30+31+30+31+31+30+st.wDay;
		   break;		   		   
	   case 11:
		   nDay	+= 31+nFebDays+31+30+31+30+31+31+30+31+st.wDay;
		   break;		   		   
	   case 12:
		   nDay	+= 31+nFebDays+31+30+31+30+31+31+30+31+30+st.wDay;
		   break;		   		   		   
	}
	return nDay-1;
}

inline DWORD SystemTime2Hours(SYSTEMTIME st)
{
   int nDays = SystemTime2Days(st);
   DWORD dwHours = nDays*24;
   dwHours		+= st.wHour;
   return dwHours;
}


const int MAXYEARSCOUNT =4;
const SYSTEMTIME SYSTEMTIMEARRAY[MAXYEARSCOUNT]={
{0	 ,1,1,1,0,0,0,0},		
{1000,1,1,1,0,0,0,0},
{2000,1,1,1,0,0,0,0},
{3000,1,1,1,0,0,0,0}};
const DWORD DAYSARRAY[MAXYEARSCOUNT]={	
SystemTime2Days(SYSTEMTIMEARRAY[0]),
SystemTime2Days(SYSTEMTIMEARRAY[1]),
SystemTime2Days(SYSTEMTIMEARRAY[2]),
SystemTime2Days(SYSTEMTIMEARRAY[3])};
const DWORD ADDDAYSARRAY[MAXYEARSCOUNT]={
DAYSARRAY[0]-SYSTEMTIMEARRAY[0].wYear*365,
DAYSARRAY[1]-SYSTEMTIMEARRAY[1].wYear*365,
DAYSARRAY[2]-SYSTEMTIMEARRAY[2].wYear*365,
DAYSARRAY[3]-SYSTEMTIMEARRAY[3].wYear*365
};

const int  MAXMONTHCOUNT =12;
const WORD MONTHDAYSARRAY[MAXMONTHCOUNT][2]={
	{31,31},//JAN
	{28,29},//FEB
	{31,31},//MAR
	{30,30},//APR
	{31,31},//MAY
	{30,30},//JUN
	{31,31},//JUL
	{31,31},//AUG
	{30,30},//SEP
	{31,31},//AUG
	{30,30},//NOV
	{31,31},//DEC
};
const WORD MONTHTATOLDAYSARRAY[MAXMONTHCOUNT+1][2]={
	{						  0						 ,                         0					  },//		
	{MONTHTATOLDAYSARRAY[ 0][0]+MONTHDAYSARRAY[ 0][0],MONTHTATOLDAYSARRAY[ 0][1]+MONTHDAYSARRAY[ 0][1]},//JAN
	{MONTHTATOLDAYSARRAY[ 1][0]+MONTHDAYSARRAY[ 1][0],MONTHTATOLDAYSARRAY[ 1][1]+MONTHDAYSARRAY[ 1][1]},//FEB
	{MONTHTATOLDAYSARRAY[ 2][0]+MONTHDAYSARRAY[ 2][0],MONTHTATOLDAYSARRAY[ 2][1]+MONTHDAYSARRAY[ 2][1]},//MAR
	{MONTHTATOLDAYSARRAY[ 3][0]+MONTHDAYSARRAY[ 3][0],MONTHTATOLDAYSARRAY[ 3][1]+MONTHDAYSARRAY[ 3][1]},//APR
	{MONTHTATOLDAYSARRAY[ 4][0]+MONTHDAYSARRAY[ 4][0],MONTHTATOLDAYSARRAY[ 4][1]+MONTHDAYSARRAY[ 4][1]},//MAY
	{MONTHTATOLDAYSARRAY[ 5][0]+MONTHDAYSARRAY[ 5][0],MONTHTATOLDAYSARRAY[ 5][1]+MONTHDAYSARRAY[ 5][1]},//JUN
	{MONTHTATOLDAYSARRAY[ 6][0]+MONTHDAYSARRAY[ 6][0],MONTHTATOLDAYSARRAY[ 6][1]+MONTHDAYSARRAY[ 6][1]},//JUL
	{MONTHTATOLDAYSARRAY[ 7][0]+MONTHDAYSARRAY[ 7][0],MONTHTATOLDAYSARRAY[ 7][1]+MONTHDAYSARRAY[ 7][1]},//AUG
	{MONTHTATOLDAYSARRAY[ 8][0]+MONTHDAYSARRAY[ 8][0],MONTHTATOLDAYSARRAY[ 8][1]+MONTHDAYSARRAY[ 8][1]},//SEP
	{MONTHTATOLDAYSARRAY[ 9][0]+MONTHDAYSARRAY[ 9][0],MONTHTATOLDAYSARRAY[ 9][1]+MONTHDAYSARRAY[ 9][1]},//AUG
	{MONTHTATOLDAYSARRAY[10][0]+MONTHDAYSARRAY[10][0],MONTHTATOLDAYSARRAY[10][1]+MONTHDAYSARRAY[10][1]},//NOV
	{MONTHTATOLDAYSARRAY[11][0]+MONTHDAYSARRAY[11][0],MONTHTATOLDAYSARRAY[11][1]+MONTHDAYSARRAY[11][1]},//DEC	
};


inline SYSTEMTIME Days2MonthDay(WORD wYear,WORD wDays)
{//从公元元年开始
	SYSTEMTIME st={1980,1,1,1,0,0,0,0};	
	bool bFlag	=	false;	
	if((wYear%4==0 && wYear%100!=0) || wYear%400==0)
	{
		bFlag	=	true;	   
	}
	
	int i=0;
	if(bFlag)
	{
		IF_NOT(wDays>=0 && wDays<=366)
		{
			return st;
		}
		i=1;
	}
	else
	{
		IF_NOT(wDays>=0 && wDays<=365)
		{
			return st;
		}	
		i=0;
	}
	
	int nCount    = 0;
	int nMaxIndex = MAXMONTHCOUNT+1;
	int nMinIndex = 0;	
	int nIndex    = 0;
	while(1)
	{
	   IF_NOT(nIndex>=0 && nIndex<MAXMONTHCOUNT)
	   {
		   return st;
	   }
	   nIndex = (nMinIndex+nMaxIndex)/2;
       if(MONTHTATOLDAYSARRAY[nIndex][i]>wDays)
	   {
		    nMaxIndex = nIndex;
	   }
	   else
	   {
		    nMinIndex = nIndex;
	   }
	   if((nMaxIndex-nMinIndex)==1)
		   break;
	   DEAD_LOOP_BREAK(nCount, MAXMONTHCOUNT);
	}
	st.wYear  = wYear;	
	st.wMonth = nMinIndex+1;
	wDays -= MONTHTATOLDAYSARRAY[nMinIndex][i];
	st.wDay   = wDays+1;
	return st;
}



inline SYSTEMTIME Days2SystemTime(DWORD dwDays, WORD wHours)
{
	SYSTEMTIME st={1980,1,2,1,wHours,0,0,0};	
	if(dwDays>=DAYSARRAY[MAXYEARSCOUNT-1] )
	{
		return st;
	}
	WORD wDayOfWeek = (dwDays+6)%7;
    int nIndex=0;
	for(int i=MAXYEARSCOUNT-1;i>=0;i--)
	{
		if(dwDays>=DAYSARRAY[i])
		{
			nIndex=i;
			break;
		}		
	}			
	
	dwDays -= DAYSARRAY[nIndex];

	int nMaxYear	=	dwDays / 365;
	int nRestDays = dwDays % 365;			
	nMaxYear+=SYSTEMTIMEARRAY[nIndex].wYear;	
	
	int nTime4		=	nMaxYear/4;
	int nTime100	=	nMaxYear/100;
	int nTime400	=	nMaxYear/400;
	int nTimes	    =	nTime4-(nTime100-nTime400);
	int nDay		=	nTimes+1;
	
	bool bFlag	=	false;
	if((nMaxYear%4==0 && nMaxYear%100!=0) || nMaxYear%400==0)
	{
		bFlag	=	true;	   
	}	
	if(bFlag)
		nDay--;	
	
	nDay -= ADDDAYSARRAY[nIndex];
	
	if(nRestDays<nDay)
	{
		nMaxYear--;
		nRestDays += 365;
		nRestDays -= nDay;
		if(!bFlag)
		{
			if((nMaxYear%4==0 && nMaxYear%100!=0) || nMaxYear%400==0)
			{
				nRestDays++;
			}
		}
	}
	else
	{
		nRestDays -= nDay;		
	}
	st=Days2MonthDay(nMaxYear,nRestDays);
	st.wDayOfWeek = wDayOfWeek;
	st.wHour = wHours;
	return st;
}

inline SYSTEMTIME Hours2SystemTime(DWORD dwHours)
{
	DWORD dwDays = dwHours/24;
	dwHours		 = dwHours%24;
	SYSTEMTIME st= Days2SystemTime(dwDays,dwHours);
	return st;
}

inline time_t SystemtimeToSeconds(SYSTEMTIME st)
{
	tm GameBasetm;	
	memset(&GameBasetm,0,sizeof(GameBasetm));	 
	GameBasetm.tm_year = st.wYear-1900;
	GameBasetm.tm_mon  = st.wMonth-1;
	GameBasetm.tm_mday = st.wDay;
	GameBasetm.tm_hour = st.wHour;
	GameBasetm.tm_min  = st.wMinute;	 
	return mktime(&GameBasetm);
}

inline SYSTEMTIME SecondsToSystemtime(DWORD dwGameDateTimeSeconds)
{
	SYSTEMTIME st;
	::GetLocalTime(&st);
	time_t GameBaseTime=dwGameDateTimeSeconds;
    tm * GameCurtm=localtime(&GameBaseTime);
	if(GameCurtm)
	{
		st.wYear=GameCurtm->tm_year+1900;
		st.wMonth=GameCurtm->tm_mon+1;
		st.wDay=GameCurtm->tm_mday;
		st.wHour=GameCurtm->tm_hour;
		st.wMinute=GameCurtm->tm_min;
		st.wSecond=GameCurtm->tm_sec;
		st.wDayOfWeek = GameCurtm->tm_wday;
		st.wMilliseconds=0;
	}
	return st;
}

inline POINT NearestPointInRect(int nCx, int nCy, POINT ptTarget, POINT ptRect)
{
	POINT posNearby;
	int nBeginX = __min(ptRect.x,ptRect.x+nCx);
	int nEndX = __max(ptRect.x,ptRect.x+nCx);
	int nBeginY = __min(ptRect.y,ptRect.y+nCy);
	int nEndY = __max(ptRect.y,ptRect.y+nCy);


	if ((ptTarget.x > nBeginX) && (ptTarget.x < nEndX))
	{
		posNearby.x = ptTarget.x;
	}
	else
	{
		if (abs(ptTarget.x - ptRect.x) < abs(ptTarget.x - (ptRect.x + nCx)))
		{
			posNearby.x = ptRect.x;
		}
		else
		{
			posNearby.x = ptRect.x + nCx;
		}
	}


	if ((ptTarget.y > nBeginY) && (ptTarget.y < nEndY))
	{
		posNearby.y = ptTarget.y;
	}
	else
	{
		if (abs(ptTarget.y - ptRect.y) < abs(ptTarget.y - (ptRect.y + nCy)))
		{
			posNearby.y = ptRect.y;
		}
		else
		{
			posNearby.y = ptRect.y + nCy;
		}
	}

	return posNearby;
}

inline long SysTimeCompare(const SYSTEMTIME& st1, const SYSTEMTIME& st2)
{
  if (st1.wYear < st2.wYear)
	  return -1;
  if (st1.wYear > st2.wYear)
	  return 1;

  if (st1.wMonth < st2.wMonth)
	  return -1;
  if (st1.wMonth > st2.wMonth)
	  return 1;

  if (st1.wDay < st2.wDay)
	  return -1;
  if (st1.wDay > st2.wDay)
	  return 1;

  if (st1.wHour < st2.wHour)
	  return -1;
  if (st1.wHour > st2.wHour)
	  return 1;
  
  if (st1.wMinute < st2.wMinute)
	  return -1;
  if (st1.wMinute > st2.wMinute)
	  return 1;

  if (st1.wSecond < st2.wSecond)
	  return -1;
  if (st1.wSecond > st2.wSecond)
	  return 1;

  if (st1.wMilliseconds < st2.wMilliseconds)
	  return -1;
  if (st1.wMilliseconds > st2.wMilliseconds)
	  return 1;

  return 0;
  
}

//////////////////////////////////////////////////////////////////////////
inline int pos2index(int x, int y, int cx, int cy) { return (x + y*cx); }
inline int index2x(int idx, int cx, int cy) { return (idx % cy); }
inline int index2y(int idx, int cx, int cy) { return (idx / cy); }
inline void pos2lt(POINT* pTarget, const POINT& pos, int nRange) { pTarget->x = pos.x - nRange; pTarget->y = pos.y - nRange; }
inline void posadd(POINT* pTarget, int nOffsetX, int nOffsetY) { pTarget->x += nOffsetX; pTarget->y += nOffsetY; }
inline void possub(POINT* pTarget, int nOffsetX, int nOffsetY) { pTarget->x -= nOffsetX; pTarget->y -= nOffsetY; }

#endif

