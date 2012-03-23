#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include "xTickCount.h"

#pragma comment(lib, "winmm.lib")

// Low-resolution ticks value of the application
static DWORD ticks_start;
// Store if a high-resolution performance counter exists on the system
static bool hi_res_timer_available =  Sys_TickInit(true);
// The first high-resolution ticks value of the application 
static LARGE_INTEGER hi_res_start_ticks;
// The number of ticks per second of the high-resolution performance counter
static LARGE_INTEGER hi_res_ticks_per_second;

bool Sys_TickInit(bool bUseHighResolute/* = true*/)
{
	// Detect the hardware is support high-resolution perfermance
	if (bUseHighResolute && QueryPerformanceFrequency(&hi_res_ticks_per_second) == TRUE)
	{
		hi_res_timer_available = true;
		QueryPerformanceCounter(&hi_res_start_ticks);
// 		FILE* fp = fopen("syslog\\system.txt", "w");
// 		if(fp)
// 		{
// 			fprintf(fp, "CPU frequency:%u", hi_res_ticks_per_second.QuadPart);
// 			fclose(fp);
// 		}
	}
	else
	{
		hi_res_timer_available = false;
		ticks_start = timeGetTime();
		return false;
	}
	return true;
}

DWORD Sys_GetTicks()
{
	DWORD ticks = 0;
	if(hi_res_timer_available) {
		LARGE_INTEGER hi_res_now;
		QueryPerformanceCounter(&hi_res_now);
		hi_res_now.QuadPart -= hi_res_start_ticks.QuadPart;
		ticks = (DWORD)(hi_res_now.QuadPart);
	}
	else {
		DWORD now = timeGetTime();
		ticks = now - ticks_start;
	}

	return ticks;
}

DWORD Sys_TicksToMS(DWORD ticks)
{
	// Hi-resolution 
	if(hi_res_timer_available) {
		LARGE_INTEGER largeTicks;
		largeTicks.QuadPart = ticks;
		largeTicks.QuadPart *= 1000;
		largeTicks.QuadPart /= hi_res_ticks_per_second.QuadPart;
		return (DWORD)largeTicks.QuadPart;
	}

	// Low resolution is ms already.
	return ticks;
}

DWORD Sys_Milliseconds()
{
	if(hi_res_timer_available)
	{
		return Sys_TicksToMS(Sys_GetTicks());
	}

	return Sys_GetTicks();
}

xTickCount::xTickCount()
{
	Start();
}

void xTickCount::Start()
{ 
	tick_start = Sys_GetTicks(); 
}

void xTickCount::Finish()
{
	tick_end = Sys_GetTicks();
}

int xTickCount::GetUsedTicks()
{
	return (tick_end - tick_start);
}

int	xTickCount::GetUsedMS()
{
	return Sys_TicksToMS(GetUsedTicks()); 
}
