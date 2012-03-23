// TimerDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <xTickCount.h>
using namespace std;

int test_performance(int argc, char* argv[])
{
	LARGE_INTEGER tFrequence;
	::QueryPerformanceFrequency(&tFrequence);
	
	LARGE_INTEGER tStart;
	::QueryPerformanceCounter(&tStart);

	Sleep(1);
	
	LARGE_INTEGER tEnd;
	::QueryPerformanceCounter(&tEnd);

	printf("cost %I64d us\r\n", ((tEnd.QuadPart-tStart.QuadPart)*1000000/tFrequence.QuadPart));

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//get cpu usage

#include <windows.h>
#include <conio.h>
#include <stdio.h>
#define SystemBasicInformation 0
#define SystemPerformanceInformation 2
#define SystemTimeInformation 3
#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))
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
typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
PROCNTQSI NtQuerySystemInformation;
void get_cpu_usage()
{
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
	SYSTEM_TIME_INFORMATION SysTimeInfo;
	SYSTEM_BASIC_INFORMATION SysBaseInfo;
	double dbIdleTime;
	double dbSystemTime;
	LONG status;
	LARGE_INTEGER liOldIdleTime = {0,0};
	LARGE_INTEGER liOldSystemTime = {0,0};
	NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle("ntdll"),"NtQuerySystemInformation");
	if (!NtQuerySystemInformation) return;
	// get number of processors in the system
	status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL);
	if (status != NO_ERROR) return;
	int amount=0;
	//printf("\n您的CPU使用率为： ");
	while(!_kbhit())
	{
		//设置新的系统时间：
		amount++;
		status = NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0);
		if (status!=NO_ERROR) return;
		status = NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL);
		if (status != NO_ERROR) return;
		if (liOldIdleTime.QuadPart != 0)
		{
			dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime);
			dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);
			dbIdleTime = dbIdleTime / dbSystemTime;
			dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors;
			int c=(int)dbIdleTime;
			printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\2b\b\b\b\b\b\b\b%d---%d%%",amount,c);
		}
		//改变新的CPU使用率和系统时间
		liOldIdleTime = SysPerfInfo.liIdleTime;
		liOldSystemTime = SysTimeInfo.liKeSystemTime;
		Sleep(1000);
	}
	printf("\n");
}

//////////////////////////////////////////////////////////////////////////
//返回引用和返回结构体的区别：OBJPOS
OBJPOS g_pos;
OBJPOS ReturnStruct()
{
	return g_pos;
}
OBJPOS& ReturnStructRef()
{
	return g_pos;
}

void TestReturnStruct(const int nCount)
{
	xTickCount tStruct;
	tStruct.Start();
	for (int i=0; i<nCount; ++i)
	{
		OBJPOS pos = ReturnStruct();
		pos.x += 3;
	}
	tStruct.Finish();

	xTickCount tRef;
	tRef.Start();
	for (int i=0; i<nCount; ++i)
	{
		OBJPOS pos = ReturnStructRef();
		pos.x += 3;
	}
	tRef.Finish();

	xTickCount tRef2;
	tRef2.Start();
	for (int i=0; i<nCount; ++i)
	{
		OBJPOS& pos = ReturnStructRef();
		pos.x += 3;
	}
	tRef2.Finish();

	cout << "COUNT:" << nCount
		<< " ReturnStruct:" << tStruct.GetUsedMS() << "ms"
		<< " ReturnRef:" << tRef.GetUsedMS() << "ms"
		<< " ReturnRefEx:" << tRef2.GetUsedMS() << "ms"
		<< endl;

}
void DemoReturnStruct()
{
	TestReturnStruct(10000000);
}

int main(int argc, char* argv[])
{
	GetTickCount();
	DemoReturnStruct();
	return 0;
}
