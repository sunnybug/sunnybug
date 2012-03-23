////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		IOCPNetTest.cpp
//	Author:
//		Chun-Hyok, Chong.
//	Description:
//		IOCPNet test code.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "workframe.h"
#include "WorkframeReturnCode.h"
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <Mswsock.h>
#include "IOCPNet.h"
#include "IniFile.h"

#define CFG_PATH						"svr.ini"
#define TEST_LOG_PATH					"log.txt"

#define WAIT_TIMEOUT_TEST				100

DWORD WINAPI LogicThread(void *pParam);
void MainLogic(OIOCPNet *pIOCPNet, DWORD SocketUnique, OBufferedSocket *pBuffSock, BYTE *pReadData, DWORD ReadSize);

HMODULE GetCurrentModule()
{
	MEMORY_BASIC_INFORMATION mbi = {0};
	VirtualQuery(GetCurrentModule, &mbi, sizeof(mbi));
	return (HMODULE)mbi.AllocationBase;
}

const char* GetExeDir()
{
	static char dir[_MAX_PATH] = {0};
	if(dir[0]==0)
	{
		GetModuleFileName(GetCurrentModule(), dir, sizeof(dir)-1);
		char* p = strrchr(dir, '\\');
		if(p)
			*p = 0;
	}
	return dir;
}

DWORD g_dRunning;

int _tmain(int argc, _TCHAR* argv[])
{
	SetCurrentDirectory(GetExeDir());
	OErrLog EL;
	OIOCPNet *pIOCPNet = NULL;
	int key;
	WSADATA WSAData;
	HANDLE hThread;

	WSAStartup(MAKEWORD(2,2), &WSAData);

	char szLogPath[_MAX_PATH] = {0};
	_snprintf(szLogPath, sizeof(szLogPath)-1, "%s\\%s", GetExeDir(), TEST_LOG_PATH);
	EL.SetErrLogFile(szLogPath);

	CIniFile ini(CFG_PATH, "svr");
	char szIP[32] = {0};
	if(!ini.GetString(szIP, "ip", sizeof(szIP)-1))
	{
		printf("read [svr].ip fail");
		goto ErrHand;
	}
	const int nPort = ini.GetInt("port");
	if(nPort <= 0)
	{
		printf("read [svr].port fail");
		goto ErrHand;
	}

	pIOCPNet = new OIOCPNet(&EL);
	pIOCPNet->Start(szIP, nPort);
	
	InterlockedExchange((long *)&g_dRunning, 1);

	hThread = CreateThread(0, 0, LogicThread, pIOCPNet, 0, 0);
	if (0 == hThread)
	{
		printf("CreateThread is failed. Error code = %d\n", GetLastError());
		goto ErrHand;
	}

	printf("Server: Press any key to stop...\n");
	key = getch();

	InterlockedExchange((long *)&g_dRunning, 0);
	WaitForSingleObject(hThread, INFINITE);

ErrHand:
	if(pIOCPNet)
	{
		pIOCPNet->Stop();
		delete pIOCPNet;
	}

	WSACleanup();
	printf("closed\n");
	return 0;
} // _tmain()

DWORD WINAPI LogicThread(void *pParam)
{
	int iRes;
	OIOCPNet *pIOCPNet;
	int EventType;
	DWORD SocketUnique;
	BYTE *pReadData;
	DWORD ReadSize;
	OBufferedSocket *pBuffSock;
	OReadSlotElement *pSlot;
	void *pCustData; // for formatting. no custom data is used in this test.

	pIOCPNet = (OIOCPNet *)pParam;

	while (1 == InterlockedExchange((long *)&g_dRunning, g_dRunning))
	{
		iRes = pIOCPNet->GetSocketEventData(WAIT_TIMEOUT_TEST, &EventType, &SocketUnique, &pReadData, &ReadSize, &pBuffSock, &pSlot, &pCustData);
		pIOCPNet->Print();
		if (RET_TIMEOUT == iRes)
		{
			continue;
		}
		else if (RET_FAIL == iRes)
		{
			return 0;
		}
		else if (RET_SOCKET_CLOSED == iRes)
		{
			// release pCustData.
			continue;
		}

		// Process main logic.
		MainLogic(pIOCPNet, SocketUnique, pBuffSock, pReadData, ReadSize);
		
		pIOCPNet->ReleaseSocketEvent(pSlot);
	}

	return 0;
} // LogicThread()

void MainLogic(OIOCPNet *pIOCPNet, DWORD SocketUnique, OBufferedSocket *pBuffSock, BYTE *pReadData, DWORD ReadSize)
{
	//printf("pReadData = %s\n", pReadData);
	pIOCPNet->WriteData(SocketUnique, pBuffSock, pReadData, ReadSize); // echo.

	return;
} // MainLogic()