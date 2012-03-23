#include "DbgFunc.h"
#include "stdio.h"
#include <tlhelp32.h>
#include <eh.h> 

#pragma warning(disable: 4200)	//nonstandard extension used : zero-sized array in struct/union

#ifdef _SUPPORT_DLL
typedef	HANDLE (WINAPI * CREATE_TOOL_HELP32_SNAPSHOT)(DWORD dwFlags, DWORD th32ProcessID);
typedef	BOOL (WINAPI * MODULE32_FIRST)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
typedef	BOOL (WINAPI * MODULE32_NEST)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);

CREATE_TOOL_HELP32_SNAPSHOT	CreateToolhelp32Snapshot_=NULL;
MODULE32_FIRST	Module32First_=NULL;
MODULE32_NEST	Module32Next_=NULL;

BOOL WINAPI Get_ModuleByAddr(PBYTE Ret_Addr, PCHAR Module_Name, PBYTE & Module_Addr)
{
	MODULEENTRY32	M = {sizeof(M)};
	HANDLE	hSnapshot;

	Module_Name[0] = 0;
	
	HMODULE hKernel32 = GetModuleHandle("KERNEL32");
	if(!CreateToolhelp32Snapshot_)
		CreateToolhelp32Snapshot_ = (CREATE_TOOL_HELP32_SNAPSHOT)GetProcAddress(hKernel32, "CreateToolhelp32Snapshot");
	if(!Module32First_ )
		Module32First_ = (MODULE32_FIRST)GetProcAddress(hKernel32, "Module32First");
	if(!Module32Next_ )
		Module32Next_ = (MODULE32_NEST)GetProcAddress(hKernel32, "Module32Next");

	if (CreateToolhelp32Snapshot_)
	{
		hSnapshot = CreateToolhelp32Snapshot_(TH32CS_SNAPMODULE, 0);
		
		if ((hSnapshot != INVALID_HANDLE_VALUE) &&
			Module32First_(hSnapshot, &M))
		{
			do
			{
				if (DWORD(Ret_Addr - M.modBaseAddr) < M.modBaseSize)
				{
					//lstrcpyn(Module_Name, M.szExePath, MAX_PATH);
					lstrcpyn(Module_Name, M.szModule, MAX_PATH);
					Module_Addr = M.modBaseAddr;
					break;
				}
			} while (Module32Next_(hSnapshot, &M));
		}

		CloseHandle(hSnapshot);
	}

	return !!Module_Name[0];
}
#endif

PBYTE DumpFuncAddress(int nLevel/*=0*/, char* pBuf/*=NULL*/, PEXCEPTION_POINTERS pException/*=NULL*/)
{	
	typedef struct STACK
	{
		STACK *	Ebp;
		PBYTE	Ret_Addr;
		DWORD	Param[0];
	} STACK, * PSTACK;
	
	STACK	Stack = {0, 0};
	PSTACK	Ebp;
	int nPos=0;
	
	if (pException)	//fake frame for exception address
	{
		Stack.Ebp = (PSTACK)pException->ContextRecord->Ebp;
		Stack.Ret_Addr = (PBYTE)pException->ExceptionRecord->ExceptionAddress;
		Ebp = &Stack;
	}
	else
		Ebp = (PSTACK)&nLevel - 1;	//frame addr of DumpFuncAddress()
	if(pBuf)
		*pBuf = 0;
	
#ifdef _SUPPORT_DLL
	CHAR	Module_Name[MAX_PATH];
	PBYTE	Module_Addr = 0;
	PBYTE	Module_Addr1;
#endif
	bool bData=false;
	// Break trace on wrong stack frame.
	for (int Ret_Addr_I = 0;Ret_Addr_I <= nLevel ;)
	{
		if(!IsBadReadPtr(Ebp, sizeof(PSTACK)) && !IsBadCodePtr(FARPROC(Ebp->Ret_Addr)))
	{
		if(pBuf)
		{
#ifdef _SUPPORT_DLL
			int nPrintLen = 0;
			if (Get_ModuleByAddr(Ebp->Ret_Addr, Module_Name, Module_Addr1))
			{
				if(Module_Addr1 != Module_Addr)
				{
					Module_Addr = Module_Addr1;
					nPrintLen = sprintf(pBuf+nPos, "(%s %p:%i) ", Module_Name, Ebp->Ret_Addr-Module_Addr,Ret_Addr_I);
				}
				else
					nPrintLen = sprintf(pBuf+nPos, "(%p:%i) ", Ebp->Ret_Addr-Module_Addr,Ret_Addr_I);
			}
			else
				nPrintLen = sprintf(pBuf+nPos, "(%s %p:%i) ", "unknown", Ebp->Ret_Addr-Module_Addr,Ret_Addr_I);							
			nPos += nPrintLen;
#else
			sprintf(pBuf+nPos, "%p:%i ", Ebp->Ret_Addr,Ret_Addr_I);
			//nPos += 9;
			nPos += 11;
#endif
			
			bData=true;
		}
		if(Ret_Addr_I == nLevel)
			return pBuf ? (PBYTE)pBuf : Ebp->Ret_Addr;
			
			Ret_Addr_I++;
			Ebp = Ebp->Ebp;
	}
		else 
			break;
	
	}
	if(bData)
		return pBuf ? (PBYTE)pBuf : Ebp->Ret_Addr;
	return 0;
}

void CSEHException::trans_func( unsigned int u, EXCEPTION_POINTERS* pExp ) 
{ 
	throw *pExp;
}

void CSEHException::initialize_seh_trans_to_ce() 
{ 
	OldFanc=_set_se_translator( trans_func ); 
} 
 