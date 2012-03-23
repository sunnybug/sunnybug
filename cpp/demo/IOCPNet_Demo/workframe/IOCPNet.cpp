////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		IOCPNet.cpp
//	Author:
//		Chun-Hyok, Chong.
//	Description:
//		It was desined for high performance and large size data flow.
////////////////////////////////////////////////////////////////////////////////////////////////////

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

// variables for test.
int g_TestID = -1;
int g_CloseSocketCallNum = 0;
int g_CloseByWrite = 0;
int g_CloseByRead = 0;

void OIOCPNetReadSlot::Constructor(OErrLog *pEL, OIOCPNet *pIOCPNet)
{
	m_pIOCPNet = pIOCPNet;
	m_pEL = pEL;

	m_pSlot = 0;
	m_SlotIndexToPut = 0;

	InitializeCriticalSection(&m_SlotLock);
} // Constructor()

void OIOCPNetReadSlot::Destructor()
{
	m_PAReadQ.FreePreAllocatedMemory();

	if (0 != m_pSlot)
	{
		m_DMemSlot.Free();
		m_pSlot = 0;
	}

	DeleteCriticalSection(&m_SlotLock);
} // Destructor()

int OIOCPNetReadSlot::SetSlots(DWORD SlotNumber)
{
	OSafeLocker SLock(&m_SlotLock);

	SLock.Lock();
	if (0 != m_pSlot)
	{
		m_DMemSlot.Free();
		m_pSlot = 0;
	}
	
    m_SlotNum = SlotNumber;
	m_pSlot = (OReadSlotElement *)m_DMemSlot.Allocate(m_SlotNum * sizeof (OReadSlotElement));
	if (0 == m_pSlot)
	{
		m_pEL->ErrLog(ERRLOC, "Allocate is failed.");
		return RET_FAIL;
	}
	m_SlotIndexToPut = 0;
	
	m_PAReadQ.PreAllocate(BUFFER_UNIT_SIZE, SlotNumber * BUFFER_TO_SLOT_FACTOR);
	m_SMMReadQ.SetPreAllocator(&m_PAReadQ);
	SLock.Unlock();

	return RET_SUCCESS;
} // SetSlots()

int OIOCPNetReadSlot::Put(int EventType, OBufferedSocket *pBuffSock, BYTE *pDataPartial, DWORD IOSize)
{
	BOOL bError;
	BOOL bFound;
	OSafeLocker SLock(&m_SlotLock);
	BYTE *pDataStart;
	DWORD IOSizeRevised;
	DWORD SizeToRead;
	DWORD SizeRemain;
	BYTE *pDataIndex;
	int NetSizeRevised;
	BOOL bDataChain;

	bError = 0;
	bFound = 0;
	
	if (SOCKET_EVENT_TYPE_CLOSE == EventType)
	{
		// Put an event to the event Q of IOCPNet.
		// The real ordering of Q is done by IOCPNet, not IOCPNetReadSlost.
		m_pIOCPNet->PutSocketEvent(SOCKET_EVENT_TYPE_CLOSE, pBuffSock, 0, 0);
		bError = 0;
		goto ErrHand;
	}

	SLock.Lock();
	pDataIndex = pDataPartial;
	do
	{
		if (0 == pBuffSock->pFillingSlot) // Don't forget '&'
		{
			if (RET_FAIL == SetNextIndexToPut())
			{
				m_pEL->ErrLog(ERRLOC, "SetNextIndexToPut is failed.");
				bError = 1;
				goto ErrHand;
			}
			pBuffSock->pFillingSlot = (m_pSlot + m_SlotIndexToPut);
			pBuffSock->pFillingSlot->bUsed = 1;
			pBuffSock->pFillingSlot->pBuffSock = pBuffSock;

			if (0 < pBuffSock->NetSizeInfoTempLen)
			{
				NetSizeRevised = NET_SIZE_INFO_LEN - pBuffSock->NetSizeInfoTempLen;
				memcpy(&pBuffSock->pFillingSlot->TotalSize, pBuffSock->NetSizeInfoTemp, pBuffSock->NetSizeInfoTempLen);
				memcpy(&pBuffSock->pFillingSlot->TotalSize + pBuffSock->NetSizeInfoTempLen, pDataIndex, NetSizeRevised);
				pBuffSock->NetSizeInfoTempLen = 0; // initialize it.
			}
			else
			{
				NetSizeRevised = NET_SIZE_INFO_LEN;
				memcpy(&pBuffSock->pFillingSlot->TotalSize, pDataIndex, NetSizeRevised);
			}
		
			pBuffSock->pFillingSlot->ReadSize = 0;
			if (0 != pBuffSock->pFillingSlot->pData)
			{
				m_SMMReadQ.Free(pBuffSock->pFillingSlot->pData);
				pBuffSock->pFillingSlot->pData = 0;
			}
			pBuffSock->pFillingSlot->pData = (BYTE *)m_SMMReadQ.Allocate(pBuffSock->pFillingSlot->TotalSize);
			if (0 == pBuffSock->pFillingSlot->pData)
			{
				m_pEL->ErrLog(ERRLOC, "Allocate is failed.");
				bError = 1;
				goto ErrHand;
			}

			pDataStart = pDataIndex + NetSizeRevised;
			IOSizeRevised = IOSize - NetSizeRevised;
			bDataChain = 0;
		}
		else
		{
			pDataStart = pDataIndex;
			IOSizeRevised = IOSize;
			bDataChain = 1;
		}

		if ((pBuffSock->pFillingSlot->TotalSize - pBuffSock->pFillingSlot->ReadSize) >= IOSizeRevised)
		{
			SizeToRead = IOSizeRevised;
			SizeRemain = 0;
		}
		else
		{
			SizeToRead = pBuffSock->pFillingSlot->TotalSize - pBuffSock->pFillingSlot->ReadSize;
			SizeRemain = IOSizeRevised - SizeToRead;
		}
		memcpy(pBuffSock->pFillingSlot->pData + pBuffSock->pFillingSlot->ReadSize, pDataStart, SizeToRead);
		pBuffSock->pFillingSlot->ReadSize += SizeToRead;

		if (pBuffSock->pFillingSlot->TotalSize == pBuffSock->pFillingSlot->ReadSize)
		{
			// Put an event to the event Q of IOCPNet.
			// The real ordering of Q is done by IOCPNet, not IOCPNetReadSlost.
			m_pIOCPNet->PutSocketEvent(SOCKET_EVENT_TYPE_READ, pBuffSock, pBuffSock->pFillingSlot, pBuffSock->pFillingSlot->pData);
			
			// Initialize variables.
			pBuffSock->pFillingSlot = 0;
		}

		if (0 == SizeRemain)
		{
			break;
		}
		else if (NET_SIZE_INFO_LEN < SizeRemain)
		{
			IOSize = SizeRemain;
			if (0 == bDataChain)
			{
				pDataIndex += (NET_SIZE_INFO_LEN + SizeToRead);
			}
			else
			{
				pDataIndex += SizeToRead;
			}
		}
		else if (0 < SizeRemain && NET_SIZE_INFO_LEN >= SizeRemain)
		{
			memcpy(pBuffSock->NetSizeInfoTemp, pDataStart + SizeToRead, SizeRemain);
			pBuffSock->NetSizeInfoTempLen = SizeRemain;
			break;
		}
		else
		{
			m_pEL->ErrLog(ERRLOC, "Odd case.");
			break;
		}
	} while (0 < SizeRemain);
	SLock.Unlock();

ErrHand:
	if (1 == bError)
	{
		return RET_FAIL;
	}

	return RET_SUCCESS;
} // Put()

int OIOCPNetReadSlot::SetNextIndexToPut()
{
	BOOL bError;
	BOOL bFound;
	DWORD StartPoint;

	bError = 0;

	// Locked by the external caller.
	// Find the next empty slot.
	StartPoint = m_SlotIndexToPut;
	bFound = 0;
	do // the same slot may be used several times. (put->get->put->get->...)
	{
		if (0 == (m_pSlot + m_SlotIndexToPut)->bUsed)
		{
			bFound = 1;
			break;
		}

		m_SlotIndexToPut++;
		if (m_SlotNum == m_SlotIndexToPut)
		{
			m_SlotIndexToPut = 0;
		}
	} while (m_SlotIndexToPut != StartPoint);

	if (0 == bFound)
	{
		m_pEL->ErrLog(ERRLOC, "Read-Q is full. Set a bigger Read-Q.");
		bError = 1;
		goto ErrHand;
	}

ErrHand:
    if (1 == bError)
	{
		return RET_FAIL;
	}

	return RET_SUCCESS;
} // FindNextIndexToPut()

void OIOCPNetReadSlot::ReleaseSlot(OReadSlotElement *pSlot)
{
	OSafeLocker SLock(&m_SlotLock);
	
	SLock.Lock();
	pSlot->pBuffSock = 0;
	if (0 != pSlot->pData)
	{
		m_SMMReadQ.Free(pSlot->pData);
		pSlot->pData = 0;
	}
	pSlot->TotalSize = 0;
	pSlot->ReadSize = 0;
	pSlot->bUsed = 0;
	SLock.Unlock();
} // ReleaseSlot()

void OIOCPNetWriteBlock::Constructor(OErrLog *pEL, OIOCPNet *pIOCPNet)
{
	m_pEL = pEL;
	m_pIOCPNet = pIOCPNet;

	m_BlockNum = 0;
	m_pBlock = 0;

	InitializeCriticalSection(&m_BlockLock);
} // Constructor()

void OIOCPNetWriteBlock::Destructor()
{
	if (0 != m_pBlock)
	{
		m_DMemBlock.Free();
		m_pBlock = 0;
	}

	DeleteCriticalSection(&m_BlockLock);
} // Destructor()

int OIOCPNetWriteBlock::SetBlock(DWORD BlockNumber)
{
	m_BlockNum = BlockNumber;
	m_pBlock = (OWriteBlockElement *)m_DMemBlock.Allocate(m_BlockNum * sizeof (OWriteBlockElement));
	if (0 == m_pBlock)
	{
		m_pEL->ErrLog(ERRLOC, "Allocate is failed.");
		return RET_FAIL;
	}
	m_BlockIndexToPut = 0;
	m_BlockIndexToGet = 0;

	return RET_SUCCESS;
} // SetBlock()

int OIOCPNetWriteBlock::Put(OBufferedSocket *pBuffSock, BYTE *pDataTotal, DWORD TotalSize)
{
	BOOL bError;
	DWORD RealSize;
	DWORD PuttingCount;
	OSafeLocker SLock(&m_BlockLock);
	DWORD Loop;
	int SizeToCopy;
	int UnitSizeToCopy;
	BYTE *pDataIndex;

	bError = 0;

	RealSize = TotalSize + NET_SIZE_INFO_LEN;

	PuttingCount = (RealSize / BUFFER_UNIT_SIZE) + ((0 == (RealSize % BUFFER_UNIT_SIZE)) ? 0 : 1);
	
	if (0 == PuttingCount)
	{
		bError = 1;
		goto ErrHand;
	}

	SLock.Lock();
	SizeToCopy = RealSize;
	pDataIndex = pDataTotal;
	for (Loop = 0; Loop < PuttingCount; Loop++)
	{
		if (1 == (m_pBlock + m_BlockIndexToPut)->bFilled)
		{
			m_pEL->ErrLog(ERRLOC, "No empty write block.");
			bError = 1;
			goto ErrHand;
		}

		(m_pBlock + m_BlockIndexToPut)->PacketNumTotal = PuttingCount;
		(m_pBlock + m_BlockIndexToPut)->PacketNumSerial = Loop;
        (m_pBlock + m_BlockIndexToPut)->pBuffSock = pBuffSock;
		
		if (BUFFER_UNIT_SIZE > SizeToCopy)
		{
			UnitSizeToCopy = SizeToCopy;
		}
		else
		{
			UnitSizeToCopy = BUFFER_UNIT_SIZE;
		}

		if (0 < Loop)
		{
			(m_pBlock + m_BlockIndexToPut)->BlockDataSize = UnitSizeToCopy;
			memcpy((m_pBlock + m_BlockIndexToPut)->BlockData, pDataIndex, UnitSizeToCopy);
			pDataIndex += UnitSizeToCopy;
		}
		else // if (0 == Loop)
		{
			(m_pBlock + m_BlockIndexToPut)->BlockDataSize = UnitSizeToCopy;
			memcpy((m_pBlock + m_BlockIndexToPut)->BlockData, &TotalSize, sizeof (TotalSize));
			memcpy((m_pBlock + m_BlockIndexToPut)->BlockData + sizeof (RealSize), pDataIndex, UnitSizeToCopy - sizeof (RealSize));
			pDataIndex += (UnitSizeToCopy - sizeof (RealSize));
		}
		(m_pBlock + m_BlockIndexToPut)->bFilled = 1;
		SizeToCopy -= UnitSizeToCopy;

		m_BlockIndexToPut++;
		if (m_BlockNum == m_BlockIndexToPut)
		{
			m_BlockIndexToPut = 0;
		}
	}
	SLock.Unlock();

ErrHand:
	if (1 == bError)
	{
		return RET_FAIL;
	}

	return RET_SUCCESS;
} // Put()

// Externally locked by m_BlockLock, because of sequential calling to this function.
// the size of pData does not over BUFFER_UNIT_SIZE.
int OIOCPNetWriteBlock::GetBlockNeedsExternalLock(OBufferedSocket **ppBuffSock, BYTE *pData, DWORD *pReadSize, BOOL *pDoesItHaveMoreSequence)
{
	BOOL bError;

	bError = 0;

	// The external lock needed.
	
	if (0 == (m_pBlock + m_BlockIndexToGet)->bFilled)
	{
		m_pEL->ErrLog(ERRLOC, "No empty write block.");
		bError = 1;
		goto ErrHand;
	}

	// BUFFER_UNIT_SIZE >= (m_pBlock + m_BlockIndexToGet)->BlockDataSize
	*ppBuffSock = (m_pBlock + m_BlockIndexToGet)->pBuffSock;
	memcpy(pData, (m_pBlock + m_BlockIndexToGet)->BlockData, (m_pBlock + m_BlockIndexToGet)->BlockDataSize);
	*pReadSize = (m_pBlock + m_BlockIndexToGet)->BlockDataSize;
	if ((m_pBlock + m_BlockIndexToGet)->PacketNumTotal == (m_pBlock + m_BlockIndexToGet)->PacketNumSerial + 1) // + 1 because PacketNumSerial starts from 0.
	{
		*pDoesItHaveMoreSequence = 0;
	}
	else
	{
		*pDoesItHaveMoreSequence = 1;
	}
	(m_pBlock + m_BlockIndexToGet)->bFilled = 0;

	m_BlockIndexToGet++;
	if (m_BlockNum == m_BlockIndexToGet)
	{
		m_BlockIndexToGet = 0;
	}


ErrHand:
	if (1 == bError)
	{
		return RET_FAIL;
	}

	return RET_SUCCESS;
} // GetBlockNeedsExternalLock()

CRITICAL_SECTION *OIOCPNetWriteBlock::GetBlockLock()
{
	return &m_BlockLock;
} // GetBlockLock()

void OIOCPNet::Constructor(OErrLog *pEL)
{
	m_pEL = pEL;
	m_pReadSlot = new OIOCPNetReadSlot(pEL, this);
	m_pWriteBlock = new OIOCPNetWriteBlock(pEL, this);

	m_pReadSlot->SetSlots(DEFAULT_READQ_SLOT_NUM);
	m_pWriteBlock->SetBlock(DEFAULT_WRITEQ_BLOCK_NUM);

	m_PABuffSock.PreAllocate(sizeof (OBufferedSocket), MAX_ACCEPTABLE_SOCKET_NUM);
	m_SMMBuffSock.SetPreAllocator(&m_PABuffSock);

	m_PASocketEvent.PreAllocate(sizeof (OBufferedSocket), DEFAULT_SOCKET_EVENT_NUM);
	m_SocketEventList.SetPreAllocator(&m_PASocketEvent);

	m_PATempWriteData.PreAllocate(sizeof (OTemporaryWriteData), DEFAULT_WRITEQ_BLOCK_NUM);
	m_SMMTempWriteData.SetPreAllocator(&m_PATempWriteData);

	m_ActiveConnectionNum = 0;	
	m_LastActiveConnectionNum = 0;
	m_tLastPrint = 0;
	m_SocketUniqueGen = 0;

	m_hSocketEvent = CreateEvent(0, 0, 0, 0);

	m_lpfnAcceptEx = 0;

	InitializeCriticalSection(&m_SocketEventLock);
	InitializeCriticalSection(&m_BuffSockLock);
} // Constructor()

void OIOCPNet::Destructor()
{
	CloseHandle(m_hSocketEvent);

	DeleteCriticalSection(&m_BuffSockLock);
	DeleteCriticalSection(&m_SocketEventLock);

	delete m_pReadSlot;
	delete m_pWriteBlock;

	// Don't call PreAllocator's FreePreAllocatedMemory function.
	// Use PreAllocator's destructor.
	// Because PreAllocator's Free function is called to release SafeStaticMemMulti's resource in SafeStaticMemMulti's destructor.
	// function call order
	// OTCPDetectorIOCP's Deinit (in destructor) -> PreAllocator's Free in SafeStaticMemMulti's destructor 
	// -> PreAllocator's destructor.
} // Destructor()

int OIOCPNet::Start(char *AcceptIP, unsigned short AcceptPort)
{
	BOOL bError;
	SYSTEM_INFO SysInfo;
	DWORD Loop;
	HANDLE hThread;

	bError = 0;

	if (0 == AcceptIP || 0 == strlen(AcceptIP) || 0 == AcceptPort)
	{
		m_pEL->ErrLog(ERRLOC, "Invalid accept socket information.");
		return RET_FAIL;
	}

	// Set IOCP & run IOCP worker threads.
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (0 == m_hCompletionPort)
	{
		m_pEL->ErrLog(ERRLOC, "CreateIoCompletionPort is failed. Error code = %d", GetLastError());
		bError = 1;
		goto ErrHand;
	}

	InterlockedExchange((long *)&m_dRunning, 1);

	GetSystemInfo(&SysInfo);
	m_WorkerThreadNum  = SysInfo.dwNumberOfProcessors * THREAD_NUM_FACTOR_PER_CPU;

	m_ThreadParamNum = m_WorkerThreadNum;

	m_pThreadParam = (OThreadParam *)m_DMemForHandle.Allocate(sizeof (OThreadParam) * m_ThreadParamNum);
	if (0 == m_pThreadParam)
	{
		m_pEL->ErrLog(ERRLOC, "Allocate is failed.");
		bError = 1;
		goto ErrHand;
	}
	
	for (Loop = 0; Loop < m_ThreadParamNum; Loop++)
	{
		(m_pThreadParam + Loop)->pIOCPNet = this;
		(m_pThreadParam + Loop)->hEvent = CreateEvent(0, 0, 0, 0);
		if (0 == (m_pThreadParam + Loop)->hEvent)
		{
			m_pEL->ErrLog(ERRLOC, "CreateEvent is failed. Error code = %d", GetLastError());
			bError = 1;
			goto ErrHand;
		}
	}	

	if (RET_FAIL == PrepareAcceptableSockets(AcceptIP, AcceptPort))
	{
		m_pEL->ErrLog(ERRLOC, "PrepareAcceptableSockets is failed.");
		bError = 1;
		goto ErrHand;
	}

	for (Loop = 0; Loop < m_WorkerThreadNum; Loop++)
	{
		hThread = CreateThread(0, 0, WorkerThreadDummy, (m_pThreadParam + Loop), 0, 0);
		if (0 == hThread)
		{
			m_pEL->ErrLog(ERRLOC, "CreateThread is failed. Error code = %d", GetLastError());
			bError = 1;
			goto ErrHand;
		}
		CloseHandle(hThread);
	}
	
ErrHand:
	if (1 == bError)
	{
		return RET_FAIL;
	}

	if (2 == bError)
	{
		return RET_NET_NOT_INIT;
	}

	return RET_SUCCESS;
} // Start()

DWORD WINAPI WorkerThreadDummy(LPVOID pParam)
{
	((OThreadParam *)pParam)->pIOCPNet->WorkerThread(((OThreadParam *)pParam)->hEvent);
	return 0;
} // WorkerThreadDummy()

void OIOCPNet::WorkerThread(HANDLE hEvent)
{
	BOOL bError;
	BOOL bIORet;
	DWORD IOSize;
	OBufferedSocket *pBuffSock;
	DWORD Flag;
	int ResRecv;
	int ResSend;
	DWORD ErrorCode;
	OSafeLocker SLock(m_pWriteBlock->GetBlockLock());
	OBufferedSocket *pBuffSockToWrite;
	DWORD ReadSizeToWrite;
	BOOL DoesItHaveMoreSequence;
	DWORD WrittenSizeUseless;
	OTemporaryWriteData *pTempWriteData;
	OVERLAPPEDExt OVL;
	LPOVERLAPPED pOVL = (LPOVERLAPPED)&OVL;
	int TestID;

	bError = 0;

	TestID = InterlockedIncrement((long *)&g_TestID);

	while (1 == InterlockedExchange((long *)&m_dRunning, m_dRunning))
	{
		// Get a completed IO request.
		pBuffSock = 0;
		bIORet = GetQueuedCompletionStatus(m_hCompletionPort, &IOSize, (LPDWORD)&pBuffSock, &pOVL, INFINITE);
		if (0 == InterlockedExchange((long *)&m_dRunning, m_dRunning))
		{
			bError = 0;
			goto ErrHand;
		}
	
		if (0 == bIORet)
		{
			ErrorCode = GetLastError();
			if (ERROR_NETNAME_DELETED == ErrorCode) // server side close. close socket because of the result of writing, timeout, etc... // ERROR_NETNAME_DELETED The specified network name is no longer available.
			{
				// the close of this server itself. (close socket because of the result of writing)
				// or closesocket by client, suddenly. 'Write -> (no Read) closesocket' on client.
				InterlockedIncrement((long *)&g_CloseByRead);
				m_pReadSlot->Put(SOCKET_EVENT_TYPE_CLOSE, pBuffSock, 0, 0); // pBuffSock's SocketUnique is needed when close.
			}
			else
			{
				m_pEL->ErrLog(ERRLOC, "GetQueuedCompletionStatus is failed. Error Code = %d", ErrorCode);
			}

			continue;
		}
		
		if (0 != pBuffSock)
		{
			if (0 == IOSize && 0 != pOVL && STATUS_BEFORE_ACCEPT != ((OVERLAPPEDExt *)pOVL)->IOType
				&& STATUS_ACCEPT != ((OVERLAPPEDExt *)pOVL)->IOType)
			{
				InterlockedIncrement((long *)&g_CloseByRead);
				m_pReadSlot->Put(SOCKET_EVENT_TYPE_CLOSE, pBuffSock, 0, 0); // pBuffSock's SocketUnique is needed when close.
				continue; // Don't run into the read mode.
			}

			if (0 != pOVL)
			{
				if ((IO_TYPE_WRITE_LAST == ((OVERLAPPEDExt *)pOVL)->IOType || IO_TYPE_WRITE == ((OVERLAPPEDExt *)pOVL)->IOType))
				{
					if (0 != ((OVERLAPPEDExt *)pOVL)->pTempWriteData)
					{
						m_SMMTempWriteData.Free(((OVERLAPPEDExt *)pOVL)->pTempWriteData);
					}	
					continue;
				}
				else if (IO_TYPE_READ == ((OVERLAPPEDExt *)pOVL)->IOType)
				{
					if (RET_FAIL == m_pReadSlot->Put(SOCKET_EVENT_TYPE_READ, pBuffSock, (BYTE *)pBuffSock->BufferReadIO.buf, IOSize))
					{
						m_pEL->ErrLog(ERRLOC, "Put is failed.");
						// Illegal packet, release the content of pBuffSock.
						InterlockedIncrement((long *)&g_CloseByRead);
						m_pReadSlot->Put(SOCKET_EVENT_TYPE_CLOSE, pBuffSock, 0, 0);
					}
				}
			}
			else
			{
				m_pEL->ErrLog(ERRLOC, "Odd operation.");
				continue;
			}
		}
		else
		{
			if (STATUS_WRITE_FROM_Q == (DWORD)pOVL)
			{
				SLock.Lock();
				do
				{
					// pTempWriteData will be freed when send IO ends.
					pTempWriteData = (OTemporaryWriteData *)m_SMMTempWriteData.Allocate(sizeof (OTemporaryWriteData));
					if (0 == pTempWriteData)
					{
						m_pEL->ErrLog(ERRLOC, "Allocate is failed.");
						bError = 1;
						goto ErrHand;
					}
					
					// the size of pData (the second parameter of GetBlockNeedsExternalLock) does not be over BUFFER_UNIT_SIZE.
					m_pWriteBlock->GetBlockNeedsExternalLock(&pBuffSockToWrite, pTempWriteData->Data, &ReadSizeToWrite, &DoesItHaveMoreSequence);
					
					pTempWriteData->Socket = pBuffSockToWrite->Socket;
					pTempWriteData->DataBuf.buf = (char *)pTempWriteData->Data;
					pTempWriteData->DataBuf.len = ReadSizeToWrite;
					pTempWriteData->OLExt.pTempWriteData = pTempWriteData;
					if (1 == DoesItHaveMoreSequence)
					{
						pTempWriteData->OLExt.IOType = IO_TYPE_WRITE;
					}
					else
					{
						pTempWriteData->OLExt.IOType = IO_TYPE_WRITE_LAST;
					}					

					Flag = MSG_PARTIAL;
					try
					{
						ResSend = WSASend(pTempWriteData->Socket, &pTempWriteData->DataBuf, 1, &WrittenSizeUseless, Flag, (LPOVERLAPPED)&pTempWriteData->OLExt, 0);
					}
					catch (...)
					{
						// suddenly, the BuffSock was closed and released by PreAllocator. Ignore...
						SLock.Unlock();
						continue;
					}
					if (SOCKET_ERROR == ResSend)
					{
						ErrorCode = WSAGetLastError();
						if (WSA_IO_PENDING != ErrorCode)
						{
							if (WSAECONNRESET == ErrorCode || WSAECONNABORTED == ErrorCode 
								|| WSAESHUTDOWN == ErrorCode || WSAENETRESET == ErrorCode)
							{
								if (0 == DoesItHaveMoreSequence)
								{
									InterlockedIncrement((long *)&g_CloseByWrite);
									m_pReadSlot->Put(SOCKET_EVENT_TYPE_CLOSE, pBuffSockToWrite, 0, 0);
								}
							}
							else
							{
								m_pEL->ErrLog(ERRLOC, "WSASend is failed. Error Code = %d", ErrorCode);
								if (0 == DoesItHaveMoreSequence)
								{
									InterlockedIncrement((long *)&g_CloseByWrite);
									m_pReadSlot->Put(SOCKET_EVENT_TYPE_CLOSE, pBuffSockToWrite, 0, 0);
								}
							}
						}
					}

				} while (1 == DoesItHaveMoreSequence);
				SLock.Unlock();
				continue;
			}
			else if (STATUS_BEFORE_ACCEPT == ((OVERLAPPEDExt *)pOVL)->IOType)
			{
				pBuffSock = ((OVERLAPPEDExt *)pOVL)->pBuffSock;
				ZeroMemory(&pBuffSock->OLExt, sizeof (pBuffSock->OLExt));
				pBuffSock->OLExt.pBuffSock = pBuffSock;
				pBuffSock->OLExt.IOType = STATUS_ACCEPT;
				if (0 == m_lpfnAcceptEx(m_ListeningSocket, pBuffSock->Socket, 
					reinterpret_cast<void*>(const_cast<BYTE*>(pBuffSock->BufferForAccept)), 0,
					sizeof (sockaddr_in) + 16, sizeof (sockaddr_in) + 16, &pBuffSock->AcceptBytes, (LPOVERLAPPED)&(pBuffSock->OLExt)))
				{
					ErrorCode = WSAGetLastError();
					if (WSA_IO_PENDING != ErrorCode)
					{
						m_pEL->ErrLog(ERRLOC, "WSAAcceptEx is failed. Error Code = %d",ErrorCode);
						bError = 1;
						goto ErrHand;
					}
				}
				continue;
			}
			else if (STATUS_ACCEPT == ((OVERLAPPEDExt *)pOVL)->IOType)
			{
				pBuffSock = ((OVERLAPPEDExt *)pOVL)->pBuffSock; // So bad AcceptEx! pBuffSock was not assigned correctly at PostQueued... for AcceptEx.
				if (RET_FAIL == ChangeSocketModeAfterAccept(pBuffSock->Socket))
				{
					m_pEL->ErrLog(ERRLOC, "ChangeSocketModeAfterAccept is failed.");
					InterlockedIncrement((long *)&g_CloseByRead);
					m_pReadSlot->Put(SOCKET_EVENT_TYPE_CLOSE, pBuffSock, 0, 0);
				}
				InterlockedIncrement((long *)&m_ActiveConnectionNum);
				goto RecvPoint;
			}
			else
			{
				m_pEL->ErrLog(ERRLOC, "Odd operation.");
				continue; // bug!
			}
		}

RecvPoint:
		// GetQueuedCompletionStatus detects a socket event whether IO is set or not by WSARecv(WSASend).
		Flag = MSG_PARTIAL;
		try
		{
			pBuffSock->OLExt.IOType = IO_TYPE_READ;
			ResRecv = WSARecv(pBuffSock->Socket, &pBuffSock->BufferReadIO, 1, &IOSize, &Flag, (LPOVERLAPPED)&pBuffSock->OLExt, 0);
		}
		catch (...)
		{
			// suddenly, the BuffSock was closed and released by PreAllocator. Ignore...
			continue;
		}
		if (SOCKET_ERROR == ResRecv)
		{
			ErrorCode = WSAGetLastError();
			if (WSA_IO_PENDING != ErrorCode)
			{
				if (WSAECONNRESET == ErrorCode || WSAECONNABORTED == ErrorCode 
					|| WSAESHUTDOWN == ErrorCode || WSAENETRESET == ErrorCode)
				{
					InterlockedIncrement((long *)&g_CloseByRead);
					m_pReadSlot->Put(SOCKET_EVENT_TYPE_CLOSE, pBuffSock, 0, 0);
				}
				else
				{
					m_pEL->ErrLog(ERRLOC, "WSARecv is failed. Error Code = %d", ErrorCode);
					InterlockedIncrement((long *)&g_CloseByRead);
					m_pReadSlot->Put(SOCKET_EVENT_TYPE_CLOSE, pBuffSock, 0, 0);
				}
			}
		}
	}

ErrHand:
	SetEvent(hEvent);
	return;
} // WorkerThread()

int OIOCPNet::PutSocketEvent(int EventType, OBufferedSocket *pBuffSock, OReadSlotElement *pSlotEle, BYTE *pData)
{
	OSocketEvent SocketEvent;
	OSafeLocker SLock(&m_SocketEventLock);

	ZeroMemory(&SocketEvent, sizeof (SocketEvent));
	SocketEvent.SocketUnique = InterlockedExchange((long *)&pBuffSock->SocketUnique, pBuffSock->SocketUnique); // the memory content of the pBuffSock may be changed suddenly, when socket is closed.
	SocketEvent.EventType = (SOCKET_EVENT_TYPE_CLOSE == EventType) ? SOCKET_EVENT_TYPE_CLOSE : SOCKET_EVENT_TYPE_READ;
	SocketEvent.pBuffSock = pBuffSock;
	SocketEvent.pSlot = pSlotEle;
    SocketEvent.pData = pData;				

	SLock.Lock();
	m_SocketEventList.Add(SocketEvent);
	SetEvent(m_hSocketEvent);
	SLock.Unlock();

	return RET_SUCCESS;
} // PutSocketEvent()

int OIOCPNet::PutSocketEvent(DWORD SocketUnique, int EventType, OBufferedSocket *pBuffSock, OReadSlotElement *pSlotEle, BYTE *pData)
{
	OSocketEvent SocketEvent;
	OSafeLocker SLock(&m_SocketEventLock);

	ZeroMemory(&SocketEvent, sizeof (SocketEvent));
	SocketEvent.SocketUnique = SocketUnique; // the memory content of the pBuffSock may be changed suddenly, when socket is closed.
	SocketEvent.EventType = (SOCKET_EVENT_TYPE_CLOSE == EventType) ? SOCKET_EVENT_TYPE_CLOSE : SOCKET_EVENT_TYPE_READ;
	SocketEvent.pBuffSock = pBuffSock;
	SocketEvent.pSlot = pSlotEle;
	SocketEvent.pData = pData;				

	SLock.Lock();
	m_SocketEventList.Add(SocketEvent);
	SetEvent(m_hSocketEvent);
	SLock.Unlock();

	return RET_SUCCESS;
} // PutSocketEvent()

// It might be called by a logic process thread.
// pSlotEle(and it's pData) must be released, when the event type is not close-socket.
// SocketUnique, pBuffSock and pSlot are needed when it releases data or closes the socket.
int OIOCPNet::GetSocketEventData(int WaitTimeMilli, int *pEventType, DWORD *pSocketUnique, BYTE **ppReadData, DWORD *pReadSize, OBufferedSocket **ppBuffSock, OReadSlotElement **ppSlot, void **ppCustData) // GetSocketEventData -> ReleaseSocketEvent!
{
	BOOL bError;
	DWORD dRes;
	OSafeLocker SLock(&m_SocketEventLock);
	OSocketEvent *pSEEle;
	DWORD TempSocketUnique;
	OBufferedSocket *pTempBuffSock;
	OReadSlotElement *pTempSlot;
	BOOL bClose;

	bError = 0;
	bClose = 0;

	dRes = WaitForSingleObject(m_hSocketEvent, 0 == WaitTimeMilli ? INFINITE : WaitTimeMilli);
	if (WAIT_TIMEOUT == dRes)
	{
		bError = 2;
		goto ErrHand;
	}
	else if (WAIT_FAILED == dRes)
	{
		m_pEL->ErrLog(ERRLOC, "WaitForSingleObject is failed. Error Code = %d", GetLastError());
		bError = 1;
		goto ErrHand;
	}

	SLock.Lock();
	pSEEle = m_SocketEventList.MoveFirst();
	*pEventType = pSEEle->EventType;
	if (SOCKET_EVENT_TYPE_CLOSE == *pEventType)
	{
		TempSocketUnique = pSEEle->SocketUnique;
		pTempBuffSock = pSEEle->pBuffSock;
		pTempSlot = pSEEle->pSlot;
		*ppCustData = (void *)InterlockedExchange((long *)&pTempBuffSock->pCustomData, (long)pTempBuffSock->pCustomData);
		bClose = 1;
	}
	else
	{
		*pSocketUnique = pSEEle->SocketUnique;
		*ppReadData = pSEEle->pSlot->pData;
		*pReadSize = pSEEle->pSlot->TotalSize;
		*ppBuffSock = pSEEle->pBuffSock;
		*ppSlot = pSEEle->pSlot;
		*ppCustData = (void *)InterlockedExchange((long *)&(*ppBuffSock)->pCustomData, (long)(*ppBuffSock)->pCustomData);
	}
	m_SocketEventList.SubtractFirst();
	if (0 < m_SocketEventList.GetCount())
	{
		SetEvent(m_hSocketEvent);
	}
	SLock.Unlock();

	if (1 == bClose)
	{
		// The only place to call CloseSocket().
		InterlockedIncrement((long *)&g_CloseSocketCallNum);
		CloseSocket(TempSocketUnique, pTempBuffSock, pTempSlot);
		bError = 3;
		goto ErrHand;
	}

ErrHand:
	if (1 == bError)
	{
		return RET_FAIL;
	}

	if (2 == bError)
	{
		return RET_TIMEOUT;
	}

	if (3 == bError)
	{
		return RET_SOCKET_CLOSED;
	}

	return RET_SUCCESS;	
} // GetSocketEvent()

void OIOCPNet::ReleaseSocketEvent(OReadSlotElement *pSlot) // GetSocketEventData -> ReleaseSocketEvent!
{
	m_pReadSlot->ReleaseSlot(pSlot);

	return;
} // ReleaseSocketEvent()

BOOL OIOCPNet::AssociateSocketWithCompletionPort(SOCKET Socket, HANDLE hCompletionPort, DWORD dwCompletionKey)
{
	HANDLE hIOCP;

	hIOCP = CreateIoCompletionPort((HANDLE)Socket, hCompletionPort, dwCompletionKey, 0);
	if (0 == hIOCP || hCompletionPort != hIOCP)
	{
		m_pEL->ErrLog(ERRLOC, "CreateIoCompletionPort is failed. Error code = %d", GetLastError());
		return 0;
	}

	return 1;
} // AssociateSocketWithCompletionPort()

int OIOCPNet::WriteData(DWORD SocketUnique, OBufferedSocket *pBuffSock, BYTE *pDataTotal, DWORD TotalSize)
{
	BOOL bError;
	OSafeLocker SLock(&m_BuffSockLock);

	bError = 0;

	SLock.Lock();
	if (SocketUnique != (DWORD)InterlockedExchange((long *)&pBuffSock->SocketUnique, pBuffSock->SocketUnique))
	{
		// The Socket has been closed.
		bError = 1;
		goto ErrHand;
	}

	if (RET_FAIL == m_pWriteBlock->Put(pBuffSock, pDataTotal, TotalSize))
	{
		m_pEL->ErrLog(ERRLOC, "Put is failed.");
		bError = 1;
		goto ErrHand;
	}
	if (0 == PostQueuedCompletionStatus(m_hCompletionPort, 0, 0, (LPOVERLAPPED)STATUS_WRITE_FROM_Q))
	{
		m_pEL->ErrLog(ERRLOC, "PostQueuedCompletionStatus is failed. Error code = %d", GetLastError());
		bError = 1;
		goto ErrHand;
	}
	SLock.Unlock();

ErrHand:
	if (1 == bError)
	{
		return RET_FAIL;
	}

	return RET_SUCCESS;
} // WriteData()

void OIOCPNet::Stop()
{
	DWORD Loop;

	InterlockedExchange((volatile long *)&m_dRunning, 0);
	for (Loop = 0; Loop < m_WorkerThreadNum; Loop++)
	{
		PostQueuedCompletionStatus(m_hCompletionPort, 0, (DWORD)0, 0);
	}
	for (Loop = 0; Loop < m_ThreadParamNum; Loop++)
	{
		WaitForSingleObject((m_pThreadParam + Loop)->hEvent, INFINITE);
	}

	closesocket(m_ListeningSocket);
	m_ListeningSocket = 0;

	for (Loop = 0; Loop < MAX_ACCEPTABLE_SOCKET_NUM; Loop++)
	{
		if (0 != (((OBufferedSocket *)(m_SMMBuffSock.GetPreAllocator())->GetAllocPoint()) + Loop)->Socket)
		{
			closesocket((((OBufferedSocket *)(m_SMMBuffSock.GetPreAllocator())->GetAllocPoint()) + Loop)->Socket);
		}
	}

	CloseHandle(m_hCompletionPort);
	m_hCompletionPort = 0;

	return;
} // Stop()

void *OIOCPNet::AttachCustomDataWithBufferedSocket(void *pData, OBufferedSocket *pBufferedSocket)
{
	if (0 == pBufferedSocket || 0 == pData)
	{
		return 0;
	}

	InterlockedExchange((long *)&pBufferedSocket->pCustomData, (long)pData);

	return (void *)InterlockedExchange((long *)&pBufferedSocket->pCustomData, (long)pBufferedSocket->pCustomData);
} // AttachCustomDataWithBufferedSocket()

void *OIOCPNet::DetachCustomDataFromBufferedSocket(OBufferedSocket *pBufferedSocket)
{
	void *pBackup;

	if (0 == pBufferedSocket)
	{
		return 0;
	}

	pBackup = (void *)InterlockedExchange((long *)&pBufferedSocket->pCustomData, (long)pBufferedSocket->pCustomData);
	InterlockedExchange((long *)&pBufferedSocket->pCustomData, 0);

	return pBackup;
} // DetachCustomDataFromBufferedSocket()

void *OIOCPNet::GetCustomData(OBufferedSocket *pBufferedSocket)
{
	if (0 == pBufferedSocket)
	{
		return 0;
	}

	return (void *)InterlockedExchange((long *)&pBufferedSocket->pCustomData, (long)pBufferedSocket->pCustomData);
} // GetCustomData()

SOCKET OIOCPNet::GetNomalSocketFromBufferedSocket(OBufferedSocket *pBufferedSocket)
{
	return pBufferedSocket->Socket;
} // GetNomalSocketFromBufferedSocket()

void OIOCPNet::CloseSocketExternal(DWORD SocketUnique, OBufferedSocket *pBuffSock, OReadSlotElement *pSlot) // when you need to close a socket externally.
{
	// pSlot is valid, so release it.
	// Don't call private member CloseSocket() directly.

	if (0 == pBuffSock || 0 == pSlot)
	{
		// if pBuffSock or pSlot is null, CloseSocket might be called internally.
		return;
	}

	PutSocketEvent(SOCKET_EVENT_TYPE_CLOSE, pBuffSock, pSlot, 0);
	return;
} // CloseSocketExternal()

int OIOCPNet::PrepareAcceptableSockets(char *AcceptIP, unsigned short AcceptPort)
{
	BOOL bError;
	DWORD Loop;
	DWORD WSAErrorCode;
	sockaddr_in SockInfo;
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	DWORD dwBytes;

	bError = 0;

	// Set an accept socket.
	m_ListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_ListeningSocket)
	{
		WSAErrorCode = WSAGetLastError();
		if (WSANOTINITIALISED == WSAErrorCode)
		{
			m_pEL->ErrLog(ERRLOC, "WSANOTINITIALISED.");
			bError = 2;
			goto ErrHand;
		}
		else
		{
			m_pEL->ErrLog(ERRLOC, "socket is failed. Error code = %d", WSAErrorCode);
			bError = 1;
			goto ErrHand;
		}
	}

	SockInfo.sin_family = AF_INET;
	SockInfo.sin_addr.S_un.S_addr = 0;
	SockInfo.sin_port = htons(AcceptPort);

	if (SOCKET_ERROR == bind(m_ListeningSocket, (struct sockaddr *)&SockInfo, sizeof (SockInfo)))
	{
		m_pEL->ErrLog(ERRLOC, "bind is failed. Error code = %d", WSAGetLastError());
		bError = 1;
		goto ErrHand;
	}

	if (SOCKET_ERROR == listen(m_ListeningSocket, BACK_LOG_SIZE))
	{
		m_pEL->ErrLog(ERRLOC, "listen is failed. Error code = %d", WSAGetLastError());
		bError = 1;
		goto ErrHand;
	}

	if (0 == AssociateSocketWithCompletionPort(m_ListeningSocket, m_hCompletionPort, 0))
	{
		m_pEL->ErrLog(ERRLOC, "AssociateSocketWithCompletionPort is failed. Error Code = %d", GetLastError());
		bError = 1;
		goto ErrHand;
	}

	dwBytes = 0;
	if (SOCKET_ERROR == WSAIoctl(m_ListeningSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof (GuidAcceptEx),
		&m_lpfnAcceptEx, sizeof (m_lpfnAcceptEx), &dwBytes, 0, 0))
	{
		m_pEL->ErrLog(ERRLOC, "WSAIoctl is failed. Error code = %d", WSAGetLastError());
		bError = 1;
		goto ErrHand;
	}

	for (Loop = 0; Loop < MAX_ACCEPTABLE_SOCKET_NUM; Loop++)
	{
		if (RET_FAIL == PrepareSocket(PREPARE_SOCKET_INIT, 0))
		{
			bError = 1;
			goto ErrHand;
		}
	}

ErrHand:
	if (1 == bError)
	{
		return RET_FAIL;
	}

	return RET_SUCCESS;
} // PrepareAcceptableSockets()

int OIOCPNet::PrepareSocket(int Mode, OBufferedSocket *pBuffSockOld)
{
	BOOL bError;
	OBufferedSocket *pBuffSock;

	bError = 0;

	if (PREPARE_SOCKET_INIT == Mode)
	{
		// Create a new buffered socket.
		pBuffSock = CreateBufferedSocket(Mode, 0);
		if (0 == pBuffSock)
		{
			m_pEL->ErrLog(ERRLOC, "CreateBufferedSocket is failed.");
			bError = 1;
			goto ErrHand;
		}
	}
	else
	{
		pBuffSock = CreateBufferedSocket(Mode, pBuffSockOld);
		if (0 == pBuffSock)
		{
			m_pEL->ErrLog(ERRLOC, "CreateBufferedSocket is failed.");
			bError = 1;
			goto ErrHand;
		}
	}

	if (RET_FAIL == SetInitialAcceptMode(pBuffSock))
	{
		m_pEL->ErrLog(ERRLOC, "SetInitialAcceptMode is failed.");
		bError = 1;
		goto ErrHand;
	}

	if (0 == AssociateSocketWithCompletionPort(pBuffSock->Socket, m_hCompletionPort, (DWORD)pBuffSock))
	{
		m_pEL->ErrLog(ERRLOC, "AssociateSocketWithCompletionPort is failed. Error Code = %d", GetLastError());
		bError = 1;
		goto ErrHand;
	}

ErrHand:
	if (1 == bError)
	{
		return RET_FAIL;
	}

	return RET_SUCCESS;
} // PrepareSocket()

OBufferedSocket *OIOCPNet::CreateBufferedSocket(int Mode, OBufferedSocket *pBuffSockOld)
{
	BOOL bError;
	OBufferedSocket *pBuffSock;
	OSafeLocker SLock(&m_BuffSockLock);

	bError = 0;
	pBuffSock = 0;

	SLock.Lock();
	if (PREPARE_SOCKET_INIT == Mode)
	{
		pBuffSock = (OBufferedSocket *)m_SMMBuffSock.Allocate(sizeof (OBufferedSocket)); // This must be freed in CloseSocket function.
		if (0 == pBuffSock)
		{
			m_pEL->ErrLog(ERRLOC, "Allocate is failed. Error Code = %d", GetLastError());
			bError = 1;
			goto ErrHand;
		}
	}
	else
	{
		pBuffSock = pBuffSockOld;
	}

	InterlockedExchange((long *)&pBuffSock->SocketUnique, InterlockedIncrement((long *)&m_SocketUniqueGen));
	pBuffSock->Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == pBuffSock->Socket)
	{
		m_pEL->ErrLog(ERRLOC, "socket is failed. Error code = %d", WSAGetLastError());
		bError = 1;
		goto ErrHand;
	}

	pBuffSock->BufferReadIO.buf = (char *)pBuffSock->BufferReadIORealData;
	pBuffSock->BufferReadIO.len = sizeof (pBuffSock->BufferReadIORealData);
	pBuffSock->pCustomData = 0;
	SLock.Unlock();

ErrHand:
	if (1 == bError)
	{
		return 0;
	}

	return pBuffSock;		
} // CreateBufferedSocket()

int OIOCPNet::CloseSocket(DWORD SocketUnique, OBufferedSocket *pBuffSock, OReadSlotElement *pSlot)
{
	BOOL bError;
	OSafeLocker SLock(&m_BuffSockLock);

	bError = 0;

	if (0 == pBuffSock->Socket)
	{
		bError = 0;
		goto ErrHand;
	}

	SLock.Lock();
	// pBuffSock was allocated by PreAllocator, so it dose not guarantee uniqueness, and does not raise an access violation.
	if (SocketUnique != (DWORD)InterlockedExchange((long *)&pBuffSock->SocketUnique, pBuffSock->SocketUnique))
	{
		SLock.Unlock();
		bError = 0;
		goto ErrHand;
	}

	if (0 != pBuffSock->Socket)
	{
		closesocket(pBuffSock->Socket);
		pBuffSock->Socket = 0; // for debugging of dirty memory.
	}
	InterlockedExchange((long *)&pBuffSock->SocketUnique, 0); // for debugging of dirty memory.
	pBuffSock->pCustomData = 0; // for debugging of dirty memory.
	pBuffSock->pFillingSlot = 0; // for debugging of dirty memory.

	if (0 != pSlot)
	{
		m_pReadSlot->ReleaseSlot(pSlot);
	}

	m_SMMBuffSock.Free(pBuffSock);
	InterlockedDecrement((long *)&m_ActiveConnectionNum);
	if (0 == InterlockedExchange((long *)&m_ActiveConnectionNum, m_ActiveConnectionNum))
	{
		bError = 0; // for debugging, to see the variables.
	}
	SLock.Unlock();

	// Make a new socket to wait to be accepted.
	PrepareSocket(PREPARE_SOCKET_REASSIGN, pBuffSock);

ErrHand:
	if (1 == bError)
	{
		return RET_FAIL;
	}

	return RET_SUCCESS;
} // CloseSocket()

// Clients may connect to this server while server makes sockets accepted.
int OIOCPNet::SetInitialAcceptMode(OBufferedSocket *pBuffSock)
{
	BOOL bError;
	DWORD ErrorCode;

	bError = 0;

	ZeroMemory(&pBuffSock->OLExt, sizeof (pBuffSock->OLExt));
	pBuffSock->OLExt.pBuffSock = pBuffSock;
	pBuffSock->OLExt.IOType = STATUS_ACCEPT;
	if (0 == m_lpfnAcceptEx(m_ListeningSocket, pBuffSock->Socket, 
		reinterpret_cast<void*>(const_cast<BYTE*>(pBuffSock->BufferForAccept)), 0,
		sizeof (sockaddr_in) + 16, sizeof (sockaddr_in) + 16, &pBuffSock->AcceptBytes, (LPOVERLAPPED)&(pBuffSock->OLExt)))
	{
		ErrorCode = WSAGetLastError();
		if (WSA_IO_PENDING != ErrorCode)
		{
			m_pEL->ErrLog(ERRLOC, "WSAAcceptEx is failed. Error Code = %d",ErrorCode);
			bError = 1;
			goto ErrHand;
		}
	}

ErrHand:
	if (1 == bError)
	{
		return RET_FAIL;
	}

	return RET_SUCCESS;
} // SetInitialAcceptMode()

int OIOCPNet::SetAcceptMode(OBufferedSocket *pBuffSock)
{
	BOOL bError;
	OSafeLocker SLock(&m_BuffSockLock);

	bError = 0;

	SLock.Lock();
	pBuffSock->OLExt.pBuffSock = pBuffSock;
	pBuffSock->OLExt.IOType = STATUS_BEFORE_ACCEPT;
	// This SetAcceptMode is called in outer logic. so it dosen't call time consuming function like AcceptEx.
	if (0 == PostQueuedCompletionStatus(m_hCompletionPort, 0, 0, (LPOVERLAPPED)&pBuffSock->OLExt))
	{
		m_pEL->ErrLog(ERRLOC, "PostQueuedCompletionStatus is failed. Error code = %d", GetLastError());
		bError = 1;
		goto ErrHand;
	}
	SLock.Unlock();

ErrHand:
	if (1 == bError)
	{
		return RET_FAIL;
	}

	return RET_SUCCESS;
} // SetAcceptMode()

int OIOCPNet::ChangeSocketModeAfterAccept(SOCKET Socket)
{
	BOOL bError;

	bError = 0;

	if (SOCKET_ERROR == setsockopt(Socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *)&m_ListeningSocket, sizeof (m_ListeningSocket)))
	{
		m_pEL->ErrLog(ERRLOC, "setsockopt is failed. Error code = %d", WSAGetLastError());
		bError = 1;
		goto ErrHand;
	}

ErrHand:
	if (1 == bError)
	{
		return RET_FAIL;
	}

	return RET_SUCCESS;
} // ChangeSocketModeAfterAccept()

bool OIOCPNet::Print()
{
	if (time(NULL)-m_tLastPrint <= 1)
		return false;

	char szMsg[1024] = {0};
	if (m_LastActiveConnectionNum != m_ActiveConnectionNum)
	{
		_snprintf(szMsg, sizeof(szMsg)-1, "ActiveConn:%u", m_ActiveConnectionNum);
		InterlockedExchange(&m_LastActiveConnectionNum, m_ActiveConnectionNum);
	}

	if(szMsg[0] != 0)
		printf("%s\r\n", szMsg);

	m_tLastPrint = time(NULL);

	return true;
}
