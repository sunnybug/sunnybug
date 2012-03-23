#ifndef _PROCESSINFO_H_
#define _PROCESSINFO_H_

#pragma once

#include <windows.h>
#include <tchar.h>
#include <Psapi.h>
#include <vector>

class ProcessInfo
{
	//////////////////////////////////////////////////////////////////////////
public:
	typedef struct
	{
		LARGE_INTEGER liIdleTime;
		DWORD dwSpare[76];
	} SYSTEM_PERFORMANCE_INFORMATION;

	typedef struct
	{
		LARGE_INTEGER liKeBootTime;
		LARGE_INTEGER liKeSystemTime;
		LARGE_INTEGER liExpTimeZoneBias;
		ULONG uCurrentTimeZoneId;
		DWORD dwReserved;
	} SYSTEM_TIME_INFORMATION;

	struct ThreadTime
	{
		FILETIME stKernelTime;
		FILETIME stUserTime;
	};	
	struct ProcessTime
	{
		FILETIME stKernelTime;
		FILETIME stUserTime;
	};
	typedef std::vector<DWORD> ID_LIST;
	typedef std::vector<ThreadTime> THREADTIME_LIST;

	struct ProcessData
	{
		char	m_txt[4096];

		//mem
		PROCESS_MEMORY_COUNTERS		m_stMem;

		//cpu
		LARGE_INTEGER				m_liOldIdleTime;
		LARGE_INTEGER				m_liOldSystemTime;
		int	m_nCpuUsage;

		ProcessTime	m_stProcessTime;
		ThreadTime	m_stThreadTime;

		ProcessData()
		{
			ZeroMemory(this, sizeof(*this));
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//API
public:
	static void GetProcessName(char* buf, int buf_size);
	static DWORD GetPID(const TCHAR *InputProcessName);
	static bool	CalcProcessThreadList(DWORD th32ProcessID, ID_LIST& lstThd);	//获取进程的线程列表

	static bool	CalcMemory(PROCESS_MEMORY_COUNTERS& stMemoryInfo, DWORD dwPID);
	static int	CalcCpuUsage(ProcessData& stProcessData);

	static bool	CalcProcessTime(DWORD dwPID, FILETIME& stKernelTime, FILETIME& stUserTime);
	static bool	CalcProcessThreadTime(DWORD dwPID, THREADTIME_LIST& lstThdTime);
	static bool	CalcThreadTime(DWORD dwTID, FILETIME& stKernelTime, FILETIME& stUserTime);

public:
	ProcessInfo(void);
	~ProcessInfo(void);

	bool	Init();
	void	OnTimer();
	bool	CalcTxt(ProcessData& stProcess);
	const ProcessData& GetProcessData() const;

protected:

	ProcessData m_CurrProcess;
};

#endif
