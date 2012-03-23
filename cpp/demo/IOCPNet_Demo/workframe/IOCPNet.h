////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		IOCPNet.h
//	Author:
//		Chun-Hyok, Chong.
//	Description:
//		It was desined for high performance and large size data flow.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __IOCPNET_H__
#define __IOCPNET_H__

#include "workframe.h"
#include "WorkframeReturnCode.h"
#include "errlog.h"
#include "preallocator.h"
#include "SafeLocker.h"
#include "SafeDynMem.h"
#include "SafeStaticMemMulti.h"
#include "LinkedListStatic.h"
#include <winsock2.h>
#include <mswsock.h>

#define THREAD_NUM_FACTOR_PER_CPU							2 // experimental value, revise it on considering the specification of running system.

#define IP_STR_LEN											(15 + 1)
#define BUFFER_UNIT_SIZE									512
#define BUFFER_TO_SLOT_FACTOR								4
#define IO_READ_BUFFER_SIZE									BUFFER_UNIT_SIZE // for IO Read.
#define NET_SIZE_INFO_LEN									4

#ifdef CLIENT_SIDE
#define MAX_ACCEPTABLE_SOCKET_NUM							20
#else
//#define MAX_ACCEPTABLE_SOCKET_NUM							65000 // Win2003/2K Server.
#define MAX_ACCEPTABLE_SOCKET_NUM							4000 // WinXP.
#endif

#define DEFAULT_READQ_SLOT_NUM								50000
#define DEFAULT_WRITEQ_BLOCK_NUM							50000
#define DEFAULT_SOCKET_EVENT_NUM							50000
#define BACK_LOG_SIZE										50000

#define SOCKET_EVENT_TYPE_CLOSE								1
#define SOCKET_EVENT_TYPE_READ								2

#define STATUS_WRITE_FROM_Q									1 // It is not the result of IO, just internal command. 0x00000001 is in exceptional memory range.
#define STATUS_BEFORE_ACCEPT								2
#define STATUS_ACCEPT										3
#define STATUS_WAITING_FOR_EVENT							4
#define IO_TYPE_READ										5
#define IO_TYPE_WRITE										6
#define IO_TYPE_WRITE_LAST									7

#define BUFFER_SIZE_TO_ACCEPT								((sizeof (sockaddr_in) + 16) * 2)

#define PREPARE_SOCKET_INIT									1
#define PREPARE_SOCKET_REASSIGN								2

// The ways of operation of read Q and write Q are different.
// So I implemented the two different Q class.

class OIOCPNet;
struct OBufferedSocket;
struct OTemporaryWriteData;
struct OVERLAPPEDExt;

struct OReadSlotElement
{
	DWORD bUsed;
	OBufferedSocket *pBuffSock;
	DWORD TotalSize;
	DWORD ReadSize;
	BYTE *pData;
}; // OReadQSlotElement

class OIOCPNetReadSlot
{
private:
	OErrLog *m_pEL;
	OSafeDynMem m_DMemSlot; // Used(Allocate -> Free) just one time, when this class is created.
	OReadSlotElement *m_pSlot;
	DWORD m_SlotIndexToPut;
	DWORD m_SlotNum;
	OPreAllocator m_PAReadQ;
	OSafeStaticMemMulti m_SMMReadQ;
	CRITICAL_SECTION m_SlotLock;
	OIOCPNet *m_pIOCPNet;

private:
	void Constructor(OErrLog *pEL, OIOCPNet *pIOCPNet);
	void Destructor();
	int SetNextIndexToPut();
public:
	OIOCPNetReadSlot(OErrLog *pEL, OIOCPNet *pIOCPNet)
	{Constructor(pEL, pIOCPNet);}
	~OIOCPNetReadSlot()
	{Destructor();}
	int SetSlots(DWORD SlotNumber);
	int Put(int EventType, OBufferedSocket *pBuffSock, BYTE *pDataPartial, DWORD IOSize);
	void ReleaseSlot(OReadSlotElement *pSlot);
}; // OIOCPNetReadSlot

struct OWriteBlockElement
{
	BOOL bFilled;
	DWORD PacketNumTotal;
	DWORD PacketNumSerial;
	OBufferedSocket *pBuffSock;
	DWORD BlockDataSize;
	BYTE BlockData[BUFFER_UNIT_SIZE];
}; // OWriteBlockElement

class OIOCPNetWriteBlock
{
private:
	OErrLog *m_pEL;
	OIOCPNet *m_pIOCPNet;
	DWORD m_BlockNum;
	CRITICAL_SECTION m_BlockLock;
	OWriteBlockElement *m_pBlock;
	OSafeDynMem m_DMemBlock; // Used(Allocate -> Free) just one time, when this class is created.
	DWORD m_BlockIndexToPut;
	DWORD m_BlockIndexToGet;

private:
	void Constructor(OErrLog *pEL, OIOCPNet *pIOCPNet);
	void Destructor();
public:
	OIOCPNetWriteBlock(OErrLog *pEL, OIOCPNet *pIOCPNet)
	{Constructor(pEL, pIOCPNet);}
	~OIOCPNetWriteBlock()
	{Destructor();}
	int SetBlock(DWORD BlockNumber);
	int Put(OBufferedSocket *pBuffSock, BYTE *pDataTotal, DWORD TotalSize);
	// Externally locked by m_BlockLock, because of sequential calling to this function.
	// ppData will be freed by the external function.
	int GetBlockNeedsExternalLock(OBufferedSocket **ppBuffSock, BYTE *pData, DWORD *pReadSize, BOOL *pDoesItHaveMoreSequence);
	CRITICAL_SECTION *GetBlockLock();
}; // OIOCPNetWriteBlock

struct OVERLAPPEDExt
{
	OVERLAPPED OL;
	int IOType;
	OBufferedSocket *pBuffSock;
	OTemporaryWriteData *pTempWriteData;
}; // OVERLAPPEDExt

struct OBufferedSocket
{
	DWORD SocketUnique;
	SOCKET Socket;
	
	BYTE BufferForAccept[BUFFER_SIZE_TO_ACCEPT];
	DWORD AcceptBytes;

	OVERLAPPEDExt OLExt;

	WSABUF BufferReadIO;
	BYTE BufferReadIORealData[IO_READ_BUFFER_SIZE];

	OReadSlotElement *pFillingSlot; // used by IOCPReadQ, only.
	
	void *pCustomData; // This is the second advantage of using IOCP. After socket event -> no need for finding player data loop. The first thing is that no select loop needed.

	int NetSizeInfoTempLen;
	BYTE NetSizeInfoTemp[NET_SIZE_INFO_LEN];
}; // OBufferedSocket

struct OSocketEvent
{
	DWORD SocketUnique;
	int EventType;
	OBufferedSocket *pBuffSock;
	OReadSlotElement *pSlot;
	BYTE *pData;
}; // OSocketEvent

struct OThreadParam
{
	OIOCPNet *pIOCPNet;
	HANDLE hEvent;
}; // OThreadParam

struct OTemporaryWriteData
{
	SOCKET Socket;
	BYTE Data[BUFFER_UNIT_SIZE];
	WSABUF DataBuf;
	OVERLAPPEDExt OLExt;
}; // OTemporaryWriteData

class OIOCPNet
{
private:
	OErrLog *m_pEL;	
	
	OIOCPNetReadSlot *m_pReadSlot;
	OIOCPNetWriteBlock *m_pWriteBlock;
	
	DWORD m_SocketUniqueGen; // SOCKET or the memory content of pBuffSock are not reliable for checking the uniqueness.
	HANDLE m_hCompletionPort;
	DWORD m_WorkerThreadNum;
	DWORD m_dRunning;
	SOCKET m_ListeningSocket;
	OPreAllocator m_PABuffSock;
	OSafeStaticMemMulti m_SMMBuffSock;

	CRITICAL_SECTION m_SocketEventLock;
	OPreAllocator m_PASocketEvent;
	OLinkedListStatic<OSocketEvent> m_SocketEventList;
	HANDLE m_hSocketEvent;

	CRITICAL_SECTION m_BuffSockLock; // Create, CloseSocket, WriteData may be in different threads.

	OSafeDynMem m_DMemForHandle;
	OThreadParam *m_pThreadParam;
	DWORD m_ThreadParamNum;

	OPreAllocator m_PATempWriteData;
	OSafeStaticMemMulti m_SMMTempWriteData;

	LONG m_ActiveConnectionNum;

	LPFN_ACCEPTEX m_lpfnAcceptEx;

private:
	void Constructor(OErrLog *pEL);
	void Destructor();
	OBufferedSocket *CreateBufferedSocket(int Mode, OBufferedSocket *pBuffSockOld);
	BOOL AssociateSocketWithCompletionPort(SOCKET Socket, HANDLE hCompletionPort, DWORD dwCompletionKey);
	int CloseSocket(DWORD SocketUnique, OBufferedSocket *pBuffSock, OReadSlotElement *pSlot);
	int PrepareAcceptableSockets(char *AcceptIP, unsigned short AcceptPort);
	int PrepareSocket(int Mode, OBufferedSocket *pBuffSock);
	int SetInitialAcceptMode(OBufferedSocket *pBuffSock);
	int SetAcceptMode(OBufferedSocket *pBuffSock);
	int ChangeSocketModeAfterAccept(SOCKET Socket);
public:
	OIOCPNet(OErrLog *pEL)
	{Constructor(pEL);}
	~OIOCPNet()
	{Destructor();}
	int PutSocketEvent(int EventType, OBufferedSocket *pBuffSock, OReadSlotElement *pSlotEle, BYTE *pData); // It is called by ReadQ.
	int PutSocketEvent(DWORD SocketUnique, int EventType, OBufferedSocket *pBuffSock, OReadSlotElement *pSlotEle, BYTE *pData); // by CloseSocketExternal.
	int Start(char *AcceptIP, unsigned short AcceptPort);
	void Stop();
	void WorkerThread(HANDLE hEvent);
	// It might be called by a logic process thread.
	// pSlotEle(and it's pData) must be released, when the event type is not close-socket.
	// SocketUnique, pBuffSock and pSlot are needed when it releases data or closes the socket.
	int GetSocketEventData(int WaitTimeMilli, int *pEventType, DWORD *pSocketUnique, BYTE **ppReadData, DWORD *pReadSize, OBufferedSocket **ppBuffSock, OReadSlotElement **ppSlot, void **ppCustData); // GetSocketEventData -> ReleaseSocketEvent!
	void ReleaseSocketEvent(OReadSlotElement *pSlot); // GetSocketEventData -> ReleaseSocketEvent!
	int WriteData(DWORD SocketUnique, OBufferedSocket *pBuffSock, BYTE *pDataTotal, DWORD TotalSize);
	void *AttachCustomDataWithBufferedSocket(void *pData, OBufferedSocket *pBufferedSocket);
	void *DetachCustomDataFromBufferedSocket(OBufferedSocket *pBufferedSocket);
	void *GetCustomData(OBufferedSocket *pBufferedSocket);
	SOCKET GetNomalSocketFromBufferedSocket(OBufferedSocket *pBufferedSocket);
	void CloseSocketExternal(DWORD SocketUnique, OBufferedSocket *pBuffSock, OReadSlotElement *pSlot); // when you need to close a socket externally.

	//////////////////////////////////////////////////////////////////////////
	//log
public:
	bool	Print();
protected:
	time_t	m_tLastPrint;
	LONG	m_LastActiveConnectionNum;
}; // OIOCPNet

DWORD WINAPI WorkerThreadDummy(LPVOID pParam);

#endif // __IOCPNET_H__