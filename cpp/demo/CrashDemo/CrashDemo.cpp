// CrashDemo.cpp : Defines the entry point for the console application.
//
//http://code.google.com/p/crashrpt/

#include "stdafx.h"
#include <stdlib.h>
#include <exception>
#include <assert.h>
#include <Windows.h>


//////////////////////////////////////////////////////////////////////////
//pure call
void _cdecl HandlePureCall()
{
	printf("handle it!!!\r\n");
	throw std::exception("HandlePureCall");	//抛出异常，避免程序退出
}

void TestPureCall()
{
	_set_purecall_handler(HandlePureCall);
	
	class CPure
	{
	public:
		CPure(){};
		virtual ~CPure(){ test2(); };
		virtual void test() = 0;
		void test2()	{ test(); }
	};

	class CPureB : public CPure
	{
	public:
		CPureB(){};
		virtual ~CPureB(){};
		virtual void test() { printf("b....\r\n"); }
	};

	try
	{
		CPure* p = new CPureB;

		delete p;
	}
	catch (const std::exception& ex)
	{
		printf("catch(exception):%s\r\n", ex.what());
	}
	catch (...)
	{
		printf("catch(...)\r\n");
	}

}

//////////////////////////////////////////////////////////////////////////
//invalid parameter
void myInvalidParameterHandler(const wchar_t* expression,
							   const wchar_t* function, 
							   const wchar_t* file, 
							   unsigned int line, 
							   uintptr_t pReserved)
{
	wprintf(L"Invalid parameter detected in function %s."
		L" File: %s Line: %d\n", function, file, line);
	wprintf(L"Expression: %s\n", expression);
}

void TestInvalidParameter()
{
	_set_invalid_parameter_handler(myInvalidParameterHandler);

	printf(NULL);
}

//////////////////////////////////////////////////////////////////////////
//异常回调
#include <Windows.h>
#include <eh.h>
void trans_func(unsigned int u, EXCEPTION_POINTERS* pExp)
{
	printf("trans_func\n");
}

//////////////////////////////////////////////////////////////////////////
void TestStackOverFlow()
{
	_alloca(2000);
	printf("TestStackOverFlow()\n");
	TestStackOverFlow();
}

//////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	printf("main start\r\n");
	_set_se_translator(trans_func);
	try
	{
		TestStackOverFlow();
	}
	catch(...)
	{
		printf("catch ...\n");
	}
	_alloca(2000);

	printf("main finish\r\n");
	return 0;
}

