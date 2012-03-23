// stdafx.cpp : source file that includes just the standard includes
// BugLint.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <string>

using namespace std;
// TODO: reference any additional headers you need in STDAFX.H
// and not in this file


//错误示例

struct SNotUsed
{
	string str;
	char ac[100];
};

struct SChar
{
	char ac[100];
};

enum ETYPE1
{
	ETYPE1_A,
};

enum ETYPE2
{
	ETYPE2_A,
	ETYPE2_B,
};

void LogSave(const char* fmt, ...)
{
	if(!fmt)
		return;
	char buf[1024] = {0};
	_vsnprintf(buf, sizeof(buf), fmt, (char*) ((&fmt)+1));
	buf[sizeof(buf)-1] = 0;
}
#define LOGSAVE LogSave

void NotNULL(char* p)
{

}

void CheckFormat(char* pMayNull)
{
// 	//告警：函数传参可能为空指针
// 	SChar schar1 = {0};
// 	memcpy(pMayNull, schar1.ac, sizeof(schar1));
// 
// 	if(!pMayNull)
// 		return;
// 
// 	SChar schar;
// 	//告警：变量未赋值/初始化就被使用，这里这里sizeof(schar.ac)==sizeof(schar)所以pclint不告警
// 	memcpy(pMayNull, schar.ac, sizeof(schar));
// 
// 	//告警：缓冲区溢出
// 	memset(&schar, 0x00, sizeof(SNotUsed));
// 
// 	//告警：库函数的格式类型不匹配
// 	sprintf(schar.ac, "%s", 1);
//
//	//告警：自定义函数的格式类型不匹配
//	LOGSAVE("%s", 1);
//
// 	
// 	char* p = new char;//告警：内存泄露
 	char *p2 = NULL;
 	memset(p2, 0x00, 3);	//告警：空指针	

// 	ETYPE1 eType1 = ETYPE2_A;
// 	switch (eType1)
// 	{
// 	case ETYPE2_A:	//告警：switch和case的类型不匹配
// 		
// 		break;
// 	}

}