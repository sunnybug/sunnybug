// kxj 2005.
#ifndef __CONSOLE_H_
#define __CONSOLE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//xbq 2005.12.8 改为使用全局类访问控制台

class CConsole
{
public:
	void WriteToConsole(const char* format,...);
	
	virtual	~CConsole();
	
	static CConsole& GetInstance();
private:
	CConsole();

private:
	void Close();	
};

#ifdef _DEBUG
#define CON_Printf CConsole::GetInstance().WriteToConsole
#else
#define CON_Printf 
#endif


#endif //__CONSOLE_H_