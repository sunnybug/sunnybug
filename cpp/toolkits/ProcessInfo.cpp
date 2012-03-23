#include "ProcessInfo.h"
#include <strsafe.h>
#include <tlhelp32.h> 

#pragma comment(lib, "psapi.lib")

#define SystemBasicInformation 0 
#define SystemPerformanceInformation 2 
#define SystemTimeInformation 3 

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart)) 
typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
typedef struct
{
	DWORD dwUnknown1;
	ULONG uKeMaximumIncrement;
	ULONG uPageSize;
	ULONG uMmNumberOfPhysicalPages;
	ULONG uMmLowestPhysicalPage;
	ULONG uMmHighestPhysicalPage;
	ULONG uAllocationGranularity;
	PVOID pLowestUserAddress;
	PVOID pMmHighestUserAddress;
	ULONG uKeActiveProcessors;
	BYTE bKeNumberProcessors;
	BYTE bUnknown2;
	WORD wUnknown3;
} SYSTEM_BASIC_INFORMATION;

static PROCNTQSI NtQuerySystemInformation = (PROCNTQSI)GetProcAddress( GetModuleHandle("ntdll") , "NtQuerySystemInformation");
static SYSTEM_BASIC_INFORMATION	SysBaseInfo;

ProcessInfo::ProcessInfo(void)
{
}

ProcessInfo::~ProcessInfo(void)
{
}

bool ProcessInfo::Init()
{
	if (!NtQuerySystemInformation)
		return false;

	// get number of processors in the system
	if (NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL) != NO_ERROR)
		return false;

	return true;
}

void ProcessInfo::OnTimer()
{
	DWORD dwPID = GetCurrentProcessId();
	CalcMemory(m_CurrProcess.m_stMem, dwPID);
	CalcCpuUsage(m_CurrProcess);
	CalcTxt(m_CurrProcess);
}

void ProcessInfo::GetProcessName( char* buf, int buf_size )
{
	char sz[MAX_PATH] = {0};
	::GetModuleFileName(0, sz, sizeof(sz)-1);
	char* pToken = strrchr(sz, '\\');
	if(pToken)
		strncpy(buf, pToken+1, buf_size-1);
}

DWORD
ProcessInfo::GetPID(const TCHAR *InputProcessName) 
{ 
	DWORD aProcesses[1024] = {0,};
	DWORD cbNeeded = 0;
	DWORD cProcesses = 0;
	HANDLE hProcess = NULL; 
	HMODULE hMod = NULL; 
	TCHAR szProcessName[MAX_PATH] = _T("UnknownProcess"); 

	// 计算目前有多少进程, aProcesses[]用来存放有效的进程PIDs 
	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
		return 0; 

	const DWORD dwProcesses = cbNeeded / sizeof(DWORD); 
	// 按有效的PID遍历所有的进程 
	for (unsigned int i = 0; i < dwProcesses; i++ ) 
	{ 
		hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]); 
		// 取得特定PID的进程名 
		if ( hProcess ) 
		{ 
			if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) ) 
			{ 
				GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName) ); 
				//将取得的进程名与输入的进程名比较，如相同则返回进程PID 
				if(!_tcsicmp(szProcessName, InputProcessName)) 
				{ 
					CloseHandle( hProcess ); 
					return aProcesses[i]; 
				} 
			} 
		}
	}

	CloseHandle( hProcess ); 
	return 0; 
}
// 
// void GetKernelMemory(LONG& Paged, LONG& NonePaged)
// {
// 	Paged = 0;
// 	NonePaged = 0;
// 
// 	static bool bOk = LoadAPI();
// 	if(!bOk)
// 		return;
// 
// 	SYSTEM_PERFROMANCE_INFOMATION sysPer;
// 	DWORD dwNumberBytes = 0;
// 	DWORD dwReturnLength = 0;
// 	dwNumberBytes = sizeof(sysPer);
// 	if(NtQuerySystemInfomation && STATUS_SUCCESS ==NtQuerySystemInfomation(SYSTEM_PERF_INFO, &sysPer, dwNumberBytes, dwReturnLength))
// 	{
// 		Paged = sysPer.PagedPoolUsage * MemPageSize;
// 		NonePaged = sysPer.NonePagedPoolUsage * MemPageSize;
// 	}
// }

bool 
ProcessInfo::CalcMemory(PROCESS_MEMORY_COUNTERS& stMemoryInfo, DWORD dwPID)
{
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ, 
		FALSE, dwPID);
	if(!hProcess)
		return false;
	bool bRet = false;

	stMemoryInfo.cb = sizeof(PROCESS_MEMORY_COUNTERS);
	if ( ::GetProcessMemoryInfo( hProcess, &stMemoryInfo, sizeof(stMemoryInfo)) )
	{
		bRet = true;
	}

	CloseHandle(hProcess);
	return bRet;
}

int ProcessInfo::CalcCpuUsage(ProcessData& stProcessData)
{
	SYSTEM_TIME_INFORMATION SysTimeInfo;
	if (NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0) != NO_ERROR)
		return 0;

	// get new CPU's idle time
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
	if (NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL) != NO_ERROR)
		return 0;

	int nCpuUsage = 0;

	// if it's a first call - skip it
	if (stProcessData.m_liOldIdleTime.QuadPart != 0)
	{
		double dbIdleTime = 0;
		double dbSystemTime = 0;

		// CurrentValue = NewValue - OldValue
		dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(stProcessData.m_liOldIdleTime);
		dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(stProcessData.m_liOldSystemTime);

		// CurrentCpuIdle = IdleTime / SystemTime
		dbIdleTime = dbIdleTime / dbSystemTime;

		// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
		dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;

		nCpuUsage = (UINT)dbIdleTime;
	}

	// store new CPU's idle and system time
	stProcessData.m_liOldIdleTime = SysPerfInfo.liIdleTime;
	stProcessData.m_liOldSystemTime = SysTimeInfo.liKeSystemTime;

	return nCpuUsage;
}

bool ProcessInfo::CalcTxt(ProcessData& stProcess)
{
	int nLen = _snprintf(stProcess.m_txt, sizeof(stProcess.m_txt)-1, "进程内存:%dM(%dM) 虚拟内存:%dM(%dM) 页面缓存:%dK(%dK) 非页面缓存:%dK(%dK)",
		stProcess.m_stMem.WorkingSetSize >> 20, stProcess.m_stMem.PeakWorkingSetSize >> 20,
		stProcess.m_stMem.PagefileUsage >> 20, stProcess.m_stMem.PeakPagefileUsage >> 20,
		stProcess.m_stMem.QuotaNonPagedPoolUsage >> 10, stProcess.m_stMem.QuotaPeakPagedPoolUsage >> 10,
		stProcess.m_stMem.QuotaNonPagedPoolUsage >> 10, stProcess.m_stMem.QuotaPeakNonPagedPoolUsage >> 10);

		nLen = _snprintf(stProcess.m_txt + nLen, sizeof(stProcess.m_txt) - nLen - 1, "\r\nCPU:%d%%", stProcess.m_nCpuUsage);

	return true;
}

bool ProcessInfo::CalcProcessTime( DWORD dwPID, FILETIME& stKernelTime, FILETIME& stUserTime )
{
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ, 
		FALSE, dwPID);
	if(!hProcess)
		return false;
	bool bRet = false;

	FILETIME stCreateTime = {0};
	FILETIME stExitTime = {0};
	if ( ::GetProcessTimes( hProcess, &stCreateTime, &stExitTime, &stKernelTime, &stUserTime) )
	{
		bRet = true;
	}

	CloseHandle(hProcess);

	return bRet;
}

bool ProcessInfo::CalcProcessThreadTime( DWORD dwPID, THREADTIME_LIST& lstThdTime )
{
	ID_LIST lstThd;
	if(!CalcProcessThreadList(dwPID, lstThd))
		return false;

	lstThdTime.reserve(lstThd.size());
	ThreadTime stThdTime;
	for (ID_LIST::const_iterator it=lstThd.begin(); it!=lstThd.end(); ++it)
	{
		CalcThreadTime(*it, stThdTime.stKernelTime, stThdTime.stUserTime );
		lstThdTime.push_back(stThdTime);
	}

	return true;
}

bool ProcessInfo::CalcThreadTime( DWORD dwTID, FILETIME& stKernelTime, FILETIME& stUserTime )
{
	HANDLE hThd = OpenThread( THREAD_QUERY_INFORMATION, FALSE, dwTID);
	if(!hThd)
		return false;
	bool bRet = false;

	FILETIME stCreateTime = {0};
	FILETIME stExitTime = {0};
	if ( ::GetThreadTimes( hThd, &stCreateTime, &stExitTime, &stKernelTime, &stUserTime) )
	{
		bRet = true;
	}

	return bRet;
}

bool ProcessInfo::CalcProcessThreadList(DWORD th32ProcessID, ID_LIST& lstThd)
{ 
	lstThd.clear();
	HANDLE hThreadSnap = NULL; 
	THREADENTRY32 th32; 

	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, th32ProcessID); 
	if (hThreadSnap == INVALID_HANDLE_VALUE) 
		return false; 

	th32.dwSize = sizeof(THREADENTRY32); 
	if (!Thread32First(hThreadSnap, &th32)) 
	{ 
		CloseHandle(hThreadSnap); 
		return false; 
	} 

	do 
	{ 
		if (th32.th32OwnerProcessID == th32ProcessID) 
		{ 
			//printf("ThreadID: %ld\n", th32.th32ThreadID); //显示找到的线程的ID 
			lstThd.push_back(th32.th32ThreadID);
		} 
	}while(Thread32Next(hThreadSnap, &th32)); 


	CloseHandle(hThreadSnap); 
	return true; 
}

const ProcessInfo::ProcessData& ProcessInfo::GetProcessData() const
{
	return m_CurrProcess;
}
