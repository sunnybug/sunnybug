// 诊断程序
// 仙剑修, 2001.10.10

//#include <afxmt.h>
#include "common.h"
#include "basefunc.h"
#include "logfile.h"
#include "AutoPtrC.h"

#define TICK_ANALYST(x)

#define		BUFSIZE		1024
class CCriticalSec
{
  public:
	  CCriticalSec()
	  {
		  InitializeCriticalSection(&cs);
	  }
	  ~CCriticalSec()
	  {
		  DeleteCriticalSection(&cs);
	  }
	  void Lock()
	  {
		  EnterCriticalSection(&cs);
	  }
	  void UnLock()
	  {
		  LeaveCriticalSection(&cs);
	  }

  private:
    CRITICAL_SECTION cs;                //!<临界区变量
};
class CMyLock
{
// Constructors
public:
	CMyLock(CCriticalSec* pCs):m_pCs(pCs) {if(m_pCs) m_pCs->Lock();}	
	~CMyLock(){if(m_pCs) m_pCs->UnLock();}
private:
	CCriticalSec* m_pCs;
};

//CString	g_sLogFile;
#define	MUTEX_LOGFILE
#ifdef	MUTEX_LOGFILE
//CAutoPtrC<CCriticalSection>	x_pLogCrit = new CCriticalSection;
CCriticalSec	x_pLogCrit;
int g_bLogCrit=1;
#else
int g_bLogCrit=0;
#endif
char		log_szTitle[BUFSIZE]	= "未命名程序 V1.0";			//? 可能空间不足
char		log_szFilename[BUFSIZE]	= "syslog\\Logfile";
time_t		log_tStartServerTime	= 0;

////////////////////////////////////////////////////////////////////////////
// 初始化LOG文件
void InitLog(const char * pTitle, const char * pFilename, time_t tStart)
{
	if(pTitle)
	{
		int nLen = strlen(pTitle);
		if(nLen >= BUFSIZE-1)
			nLen = BUFSIZE-1;
		strncpy(log_szTitle, pTitle, nLen+1);
		log_szTitle[BUFSIZE-1]	= 0;
	}

	if(pFilename)
	{
		int nLen = strlen(pFilename);
		if(nLen >= BUFSIZE-1)
			nLen = BUFSIZE-1;
		strncpy(log_szFilename, pFilename, nLen+1);
		log_szFilename[BUFSIZE-1]	= 0;
	}

	log_tStartServerTime = tStart;
}

void LOSEDATA(const char * sFormat, ...)
{
	va_list argptr;
	va_start( argptr, sFormat);     /* Initialize variable arguments. */

	char	buf[4096] = "";					/* ?????空间可能不足 */
	int		ret = vsprintf(buf, sFormat, argptr);
	strcat(buf, "\n");
//	if(!(ret < 4096-2)) log_Save(x, "ASSERT(ret < 4096-2)");	/* ?????空间可能不足 */

	va_end( argptr );              /* Reset variable arguments.      */

	log_Save("LOSE_DATA", buf);
}

void log_Save2(const char * pFilename, const char * pBuf);
void log_Save(const char * pFilename, const char * pBuf)
{
#ifndef CUSTOM_LOGSERVICE
	TICK_ANALYST(ANALYST_LOGSAVE);
#endif
#ifdef	MUTEX_LOGFILE
	//if(x_pLogCrit)
	if(g_bLogCrit)
	{
		CMyLock lockObj(&x_pLogCrit);
		log_Save2(pFilename, pBuf);
	}
	else
#endif
	{
		log_Save2(pFilename, pBuf);
	}
}

void log_Save2(const char * pFilename, const char * pBuf)
{

	// 写文件
	time_t tCurrTime;
	time( &tCurrTime );
	tm *	pTmFile = localtime(&tCurrTime);
	char	szLogFile[1024] = "";				//? 可能空间不足
	sprintf(szLogFile, "%s %04d-%02d-%02d.LOG", pFilename, pTmFile->tm_year+1900, pTmFile->tm_mon+1, pTmFile->tm_mday);

#ifndef CUSTOM_LOGSERVICE
	FILE	* pFile;
	pFile = fopen(szLogFile, "a");
	if(pFile)
	{
		fseek(pFile, 0, SEEK_END);
		{
			if(ftell(pFile) == 0)
			{
				char buf[2048];			//? 
				char bufStart[20];
				char bufCurr[20];
			
				DateTime(bufStart, log_tStartServerTime);
				DateTime(bufCurr, tCurrTime);
				int	nSec = (int)difftime(tCurrTime, log_tStartServerTime);
				sprintf(buf,	"================================================================================\n"
								"=== %s\n"
								"=== Start server time is %s\n"
								"================================================================================\n"
								"=== Current time is %s\n"
								"=== Server takes %d days  %02d:%02d:%02d\n"
								"================================================================================\n\n\n"
								, log_szTitle
								, bufStart
								, bufCurr
								, nSec / 86400, (nSec/3600)%24, (nSec/60)%60, nSec%60 );
				fputs(buf, pFile);
			}
		}

		// 输出消息
		if(ftell(pFile) < 500*1000*1000)
			fputs(pBuf, pFile);
		fclose(pFile);
	}
#else
	CacheFile_Write2(szLogFile, pBuf);
#endif
}

void FirstWrite(const char* filename)
{
	FILE	* pFile;
	pFile = fopen(filename, "a");
	if(pFile)
	{
		char buf[2048];			//? 
		char bufStart[20];
		char bufCurr[20];
		time_t tCurrTime;
		time( &tCurrTime );
		
		DateTime(bufStart, log_tStartServerTime);
		DateTime(bufCurr, tCurrTime);
		int	nSec = (int)difftime(tCurrTime, log_tStartServerTime);
		sprintf(buf,	"================================================================================\n"
			"=== %s\n"
			"=== Start server time is %s\n"
			"================================================================================\n"
			"=== Current time is %s\n"
			"=== Server takes %d days  %02d:%02d:%02d\n"
			"================================================================================\n\n\n"
			, log_szTitle
			, bufStart
			, bufCurr
			, nSec / 86400, (nSec/3600)%24, (nSec/60)%60, nSec%60 );
		fputs(buf, pFile);
		
		fclose(pFile);
	}
}

//////////////////////////////////////////////////////////////////////
CLogFile::CLogFile(LPCTSTR szFile, LPCTSTR szTitle/*=NULL*/, int nMaxSize/*=500*1000*1000*/)
{
}

//////////////////////////////////////////////////////////////////////
void CLogFile::SaveLog(LPCTSTR szHead, LPCTSTR szFormat, ...)
{
}

//////////////////////////////////////////////////////////////////////
void CLogFile::SaveRaw(LPCTSTR szLog)
{
}