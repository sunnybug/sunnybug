#include "Console.h"
#include "BaseFunc.h"
#include <stdio.h>
#include <conio.h>
#include "MyCriticalSection.h"
#include "Debug.h"

CMyCriticalSection& GetConsoleLock()
{
	static CMyCriticalSection ls_csConsoleLock;
	return ls_csConsoleLock;
}

CConsole::CConsole()
{
#ifdef _DEBUG
	AllocConsole();
#endif
}

CConsole::~CConsole()
{
#ifdef _DEBUG
	Close();
#endif
}


CConsole& CConsole::GetInstance()
{
	static CConsole ls_Console;
	return ls_Console;
}

void CConsole::Close()
{
	FreeConsole();
}

void CConsole::WriteToConsole(const char *format,...)
{
	//这里的缓冲区没必要太大
	const int MAX_CHAR = 1024*2;
	char result[MAX_CHAR]={0,};
	
	try{		
		_vsnprintf(result,_countof_array(result)-1,format, (char*)(&format+1));
        
		GetConsoleLock().Lock();
		
		_cprintf(result);
        
		GetConsoleLock().Unlock();
	}
	catch(...){ LogSave("CATCH: *** ConsoleTrace() crash! ***\n"); }
}