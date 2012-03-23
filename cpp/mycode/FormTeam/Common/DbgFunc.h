#ifndef _DBGFUNC_H
#define _DBGFUNC_H
#include "windows.h"
#include <eh.h> 

#pragma optimize( "y", off )	// 保证CALL FRAME不会被优化掉

//返回函数调用地址，当nLevel＝0，返回调用DumpFuncAddress函数的地址，nLevel每加1，返回更上一级调用地址
//并把调用堆栈格式化到pBuf缓存中
PBYTE DumpFuncAddress(int nLevel=0, char* pBuf=NULL, PEXCEPTION_POINTERS pException=NULL);
class CSEHException
{
public:
	static  void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp ); 
	void initialize_seh_trans_to_ce();
	CSEHException(){OldFanc=NULL;}
	~CSEHException()
	{
		if(OldFanc)_set_se_translator(OldFanc); 
	}
	_se_translator_function OldFanc;
};
#endif