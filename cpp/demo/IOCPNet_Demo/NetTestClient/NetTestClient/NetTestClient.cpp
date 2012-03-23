#include "stdafx.h"
#include "workframe.h"
#include "WorkframeReturnCode.h"
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include "TCPFunc.h"
#include "IniFile.h"

#define SENDING_INTERVAL									50

#define READ_TIMEOUT_TEST									(1000 * 30)
#define WRITE_TIMEOUT_TEST									(1000 * 30)

DWORD g_dRunning = 0;

DWORD WINAPI NetworkingThread(void *pParam);
int Connect(SOCKET *pSock, char *IP, unsigned short Port);

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

int _tmain(int argc, _TCHAR* argv[])
{
	SetCurrentDirectory(GetExeDir());

	int Loop;
	int Key;
	WSADATA WSAData;

	WSAStartup(MAKEWORD(2,2), &WSAData);

	srand((unsigned)time(0));

	g_dRunning = 1;

	printf("Client: Press any key to stop...\n");

	CIniFile ini("client.ini", "svr");
	int THREAD_NUMBER = ini.GetInt("thread_num");
	if(THREAD_NUMBER <= 0)
		THREAD_NUMBER = 1;

	for (Loop = 0; Loop < THREAD_NUMBER; Loop++)
	{
		if (0 == CreateThread(0, 0, NetworkingThread, (LPVOID)Loop, 0, 0))
		{
			printf("(Loop: %d) CreateThread is failed. Error code = %d\n", Loop, GetLastError());
			break;
		}
	}

	Key = getch();

	InterlockedExchange((long *)&g_dRunning, 0);

	Sleep(5000); // just sleep...

	WSACleanup();

	return 0;
} // _tmain()

DWORD WINAPI NetworkingThread(void *pParam)
{
	CIniFile ini("client.ini", "svr");
	const int TEST_PACKET_LEN = ini.GetInt("send_size");
	if(TEST_PACKET_LEN <= 0)
	{
		printf("read [svr].send_size fail");
		goto ErrHand;
	}
	
	int RunID;
	SOCKET *Sockets;
	BYTE* TestPacket = new BYTE[TEST_PACKET_LEN+1];
	BYTE* RecvData = new BYTE[TEST_PACKET_LEN+1];
	int PacketSize;
	char Temp[2];
	DWORD ErrorCode;
	int iRes;
	int ConnNum;
	int Loop;
	int Index;

	const int USER_NUMBER = ini.GetInt("conn_num");
	if(USER_NUMBER <= 0)
	{
		printf("read [svr].conn_num fail");
		goto ErrHand;
	}

	int THREAD_NUMBER = ini.GetInt("thread_num");
	if(THREAD_NUMBER <= 0)
		THREAD_NUMBER = 1;

	ConnNum = USER_NUMBER / THREAD_NUMBER;

	Sockets = (SOCKET *)malloc(sizeof (SOCKET) * ConnNum);

	RunID = (int)pParam;
	
	ZeroMemory(Sockets, sizeof (SOCKET) * ConnNum);
	
	itoa(RunID % 10, Temp, 10);
	PacketSize = TEST_PACKET_LEN;
	ZeroMemory(TestPacket, sizeof (TestPacket));
	memset(TestPacket, Temp[0], PacketSize);
	TestPacket[0] = 'S';
	TestPacket[TEST_PACKET_LEN - 1] = 'E';

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


	for (Loop = 0; Loop < ConnNum; Loop++)
	{
		if (0 == Connect(&Sockets[Loop], szIP, nPort))
		{
			printf("Connect is failed. Run ID = %d, Loop = %d\n", RunID, Loop);
			goto ErrHand;
		}
		printf("Connect OK. Run ID = %d\n", RunID);
	}

	while (1 == InterlockedExchange((long *)&g_dRunning, g_dRunning))
	{
		Index = rand() % ConnNum;

		iRes = TCPWrite(Sockets[Index], (char *)TestPacket, PacketSize, WRITE_TIMEOUT_TEST, &ErrorCode);
		if (RET_TIMEOUT == iRes)
		{
			printf("TCPWrite is failed. Error code = %d\n", ErrorCode);
			goto ErrHand;
		}

		iRes = TCPRead(Sockets[Index], 0, 0, READ_TIMEOUT_TEST, &ErrorCode);
		if (0 > iRes)
		{
			printf("TCPRead is failed. Error code = %d\n", ErrorCode);
			goto ErrHand;
		}

		ZeroMemory(RecvData, sizeof (RecvData));
		iRes = TCPRead(Sockets[Index], (char *)RecvData, iRes, READ_TIMEOUT_TEST, &ErrorCode);
		if (0 > iRes)
		{
			printf("TCPRead is failed. Error code = %d\n", ErrorCode);
			goto ErrHand;
		}

		//printf("RunID = %d\n", RunID);
		//printf("RecvData = %s\n", RecvData);

		Sleep(SENDING_INTERVAL);
	}

ErrHand:
	for (Loop = 0; Loop < ConnNum; Loop++)
	{
		if (0 != Sockets[Loop])
		{
			closesocket(Sockets[Loop]);
			Sockets[Loop] = 0;
		}
	}

	free(Sockets);

	return 0;
} // NetworkingThread()

int Connect(SOCKET *pSock, char *IP, unsigned short Port)
{
	sockaddr_in SockAddr;

	*pSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == *pSock)
	{
		return 0;
	}

	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = inet_addr(IP);
	SockAddr.sin_port = htons(Port);

	if (SOCKET_ERROR == connect(*pSock, (SOCKADDR*) &SockAddr, sizeof (SockAddr)))
	{
		DWORD ErrorCode = WSAGetLastError();
		printf("ErrorCode = %d\n", ErrorCode);
		return 0;
	}
	
	return 1;
} // Connect()