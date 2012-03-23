
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <string> 
#include <iostream> 
#include "ExcuteCmdLine.h"

using namespace std;


bool ExcuteCmd(const std::string& strCmdLine)
{
	STARTUPINFO si = {0};
	GetStartupInfo(&si); // 获取本进程的STARTUPINFO结构信息

	char acChar[MAX_PATH] = {0};
	strncpy(acChar, strCmdLine.c_str(), sizeof(acChar)-1);

	PROCESS_INFORMATION pi = {0};
	// 创建子进程
	if (!CreateProcess(NULL, acChar, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) // 输入信息
	{
		cout << "创建子进程失败,错误代码:" << GetLastError() << endl;
		return false;
	}

	return true;
}

bool ExcuteCmdWithOutput(const std::string& strCmdLine)
{
	//CreatePipe
	HANDLE hRead = NULL; // 管道读句柄
	HANDLE hWrite = NULL; // 管道写句柄
	BOOL bRet = CreatePipe(&hRead, &hWrite, NULL, 0); // 创建匿名管道
	if (bRet == TRUE)
		printf("成功创建匿名管道!\n");
	else
		printf("创建匿名管道失败,错误代码:%d\n", GetLastError());

	// 得到本进程的当前标准输出
	HANDLE hTemp = GetStdHandle(STD_OUTPUT_HANDLE);

	// 设置标准输出到匿名管道
	SetStdHandle(STD_OUTPUT_HANDLE, hWrite);
	ExcuteCmd(strCmdLine);
	SetStdHandle(STD_OUTPUT_HANDLE, hTemp); // 恢复本进程的标准输出

	char ReadBuf[100] = {0};
	DWORD ReadNum = 0;
	// 读管道直至管道关闭
	while (ReadFile(hRead, ReadBuf, 100, &ReadNum, NULL))
	{
		ReadBuf[ReadNum] = '\0';
		cout << "从管道" << ReadBuf << "读取" << ReadNum << "字节数据" << endl;
	}

	CloseHandle(hWrite); // 关闭写句柄
	CloseHandle(hRead); // 关闭写句柄
	return true;
}



#if 0
#define BUFSIZE 4096 

HANDLE hChildStdinRd, hChildStdinWr,  
hChildStdoutRd, hChildStdoutWr, 
hInputFile, hStdout;

BOOL CreateChildProcess(VOID); 
VOID WriteToPipe(VOID); 
VOID ReadFromPipe(VOID); 
VOID ErrorExit(LPSTR); 



bool ExcuteCmdLine(const std::string& strCmdLine)
{ 
	SECURITY_ATTRIBUTES saAttr; 
	BOOL fSuccess; 

	// Set the bInheritHandle flag so pipe handles are inherited. 

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 

	// Get the handle to the current STDOUT. 

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 

	// Create a pipe for the child process's STDOUT. 

	if (! CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) 
		ErrorExit("Stdout pipe creation failed\n"); 

	// Ensure the read handle to the pipe for STDOUT is not inherited.

	SetHandleInformation( hChildStdoutRd, HANDLE_FLAG_INHERIT, 0);

	// Create a pipe for the child process's STDIN. 

	if (! CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0)) 
		ErrorExit("Stdin pipe creation failed\n"); 

	// Ensure the write handle to the pipe for STDIN is not inherited. 

	SetHandleInformation( hChildStdinWr, HANDLE_FLAG_INHERIT, 0);

	// Now create the child process. 

	fSuccess = CreateChildProcess();
	if (! fSuccess) 
		ErrorExit("Create process failed with"); 

	// Get a handle to the parent's input file. 

	if (argc == 1) 
		ErrorExit("Please specify an input file"); 

	printf( "\nContents of %s:\n\n", argv[1]);

	hInputFile = CreateFile(argv[1], GENERIC_READ, 0, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL); 

	if (hInputFile == INVALID_HANDLE_VALUE) 
		ErrorExit("CreateFile failed"); 

	// Write to pipe that is the standard input for a child process. 

	WriteToPipe(); 

	// Read from pipe that is the standard output for child process. 

	ReadFromPipe(); 

	return 0; 
} 

BOOL CreateChildProcess() 
{ 
	TCHAR szCmdline[]=TEXT("child");
	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo;
	BOOL bFuncRetn = FALSE; 

	// Set up members of the PROCESS_INFORMATION structure. 

	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

	// Set up members of the STARTUPINFO structure. 

	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.hStdError = hChildStdoutWr;
	siStartInfo.hStdOutput = hChildStdoutWr;
	siStartInfo.hStdInput = hChildStdinRd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process. 

	bFuncRetn = CreateProcess(NULL, 
		szCmdline,     // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		0,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo);  // receives PROCESS_INFORMATION 

	if (bFuncRetn == 0) 
		ErrorExit("CreateProcess failed\n");
	else 
	{
		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
		return bFuncRetn;
	}
}

VOID WriteToPipe(VOID) 
{ 
	DWORD dwRead, dwWritten; 
	CHAR chBuf[BUFSIZE]; 

	// Read from a file and write its contents to a pipe. 

	for (;;) 
	{ 
		if (! ReadFile(hInputFile, chBuf, BUFSIZE, &dwRead, NULL) || 
			dwRead == 0) break; 
		if (! WriteFile(hChildStdinWr, chBuf, dwRead, 
			&dwWritten, NULL)) break; 
	} 

	// Close the pipe handle so the child process stops reading. 

	if (! CloseHandle(hChildStdinWr)) 
		ErrorExit("Close pipe failed\n"); 
} 

VOID ReadFromPipe(VOID) 
{ 
	DWORD dwRead, dwWritten; 
	CHAR chBuf[BUFSIZE]; 

	// Close the write end of the pipe before reading from the 
	// read end of the pipe. 

	if (!CloseHandle(hChildStdoutWr)) 
		ErrorExit("Closing handle failed"); 

	// Read output from the child process, and write to parent's STDOUT. 

	for (;;) 
	{ 
		if( !ReadFile( hChildStdoutRd, chBuf, BUFSIZE, &dwRead, 
			NULL) || dwRead == 0) break; 
		if (! WriteFile(hStdout, chBuf, dwRead, &dwWritten, NULL)) 
			break; 
	} 
} 

VOID ErrorExit (LPSTR lpszMessage) 
{ 
	fprintf(stderr, "%s\n", lpszMessage); 
	ExitProcess(0); 
}
#endif
