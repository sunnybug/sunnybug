// ModDump.h: interface for the CModDump class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODDUMP_H__B3E9882B_145A_41DE_9B4B_F2B50432796E__INCLUDED_)
#define AFX_MODDUMP_H__B3E9882B_145A_41DE_9B4B_F2B50432796E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

//#include "BaseSupport.h"
#define BASE_SUPPORT_CLASS	_declspec(dllimport)
#define BASE_SUPPORT_API	_declspec(dllimport)

#include <string>

class BASE_SUPPORT_CLASS CModDump  
{
public:
	static void DumpModuleInfo(const HMODULE hMod);
	
	static int DumpExcetionInfo(LPEXCEPTION_POINTERS lpExceptInfo);
private:
	static void GetModuleInfo(
		const HMODULE hMod,
		std::string& strModInfo
		);
	
	CModDump();
	virtual ~CModDump();
	
};

#endif // !defined(AFX_MODDUMP_H__B3E9882B_145A_41DE_9B4B_F2B50432796E__INCLUDED_)
