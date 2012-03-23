#ifndef _XTICKCOUNT_H_
#define _XTICKCOUNT_H_

#pragma once

class xTickCount
{
public:
	xTickCount();
	void	Start();		
	void	Finish();		
	int		GetUsedTicks();	
	int		GetUsedMS();	
private:
	DWORD	tick_start;
	DWORD	tick_end;
};

bool	Sys_TickInit(bool bUseHighResolute = false);
DWORD	Sys_GetTicks();
DWORD	Sys_TicksToMS(DWORD ticks);
DWORD	Sys_Milliseconds();				// Get the milliseconds from init

#endif //file guarder