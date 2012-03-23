// XProfile.cpp : 定义 DLL 应用程序的导出函数。
//

#include "XProfile.h"
#include <Windows.h>
#include <stddef.h>
#include <stdio.h>

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#ifdef _CAP_PROFILING
#error Don't build this file with /fastcap
#endif

extern XPROFILE_API void _cyg_profile_func_enter( void* pfn_this, void* pCaller )
{
	printf("enter");
}

extern XPROFILE_API void _cyg_profile_func_exit( void* pfn_this )
{
	printf("exit");
}

#ifdef _MSC_VER

// void __stdcall _CAP_Enter_Function(void* func)
// {
// 
// 	__asm
// 	{
// 		pushad        //堆栈中按顺序压入寄存器: EAX,ECX,EDX,EBX,ESP,EBP,ESI和EDI
// 		pushfd
// 	}
// 	//这里添加代码
// 	__asm 
// 	{
// 		popfd
// 		popad
// 	}
// 
// }
// void __stdcall _CAP_Exit_Function(void* func)
// {
// 	__asm 
// 	{
// 		pushad
// 		pushfd
// 	}
// 	//这里添加代码
// 	__asm 
// 	{
// 		popfd
// 		popad
// 	}
// }

extern "C"
_declspec(naked) void _stdcall _CAP_Start_Profiling( void* pCaller, void* pCallee )
{
	__asm
	{
		push ebp
		mov ebp, esp
		push ebx
		push esi
		push edi
		pushad
		pushfd
		push eax
		push ecx
		push edx
	}
	_cyg_profile_func_enter(pCallee, pCaller);
	__asm
	{
		pop edx
		pop ecx
		pop eax
		popfd
		popad
		pop edi
		pop esi
		pop ebx
		mov esp, ebp
		pop ebp
		ret 8
	}
}

extern "C"
_declspec(naked) void _stdcall _CAP_End_Profiling( void* pCallee )
{
	__asm
	{
		push ebp
		mov ebp, esp
		push ebx
		push esi
		push edi
		pushad
		pushfd
		push eax
	}
	_cyg_profile_func_exit(pCallee);
	__asm
	{
		pop eax
		popfd
		popad
		pop edi
		pop esi
		pop ebx
		mov esp, ebp
		pop ebp
		ret 4
	}
}

#endif	//_MSC_VER