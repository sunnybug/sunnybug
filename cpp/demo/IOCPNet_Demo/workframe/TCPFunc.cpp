////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		TCPFunc.cpp
//	Author:
//		Chun-Hyok, Chong.
//	Description:
//		It provides the basic network functions handle reading/writing/packaging rough(?) data.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Workframe.h"
#include "TCPFunc.h"
#include "WorkframeReturnCode.h"
#include "SafeDynMem.h"

#ifdef BLOCK
#undef BLOCK
#endif // BLOCK

#define READ_UNIT_SIZE							512 // less than general MTU size.
#define WRITE_UNIT_SIZE							512 // less than general MTU size.
#define MSG_LEN_INFO_SIZE						4

#define MAX_NET_TRIAL							30

#define CHECK_ALIVE_VALUE						-987654321

#define MAX_RECV_AGAIN							3


int TCPRead(SOCKET Socket, char *Buffer, DWORD BufferSize, DWORD TimeoutMilli, DWORD *ErrorCode)
{
	BOOL bError;
	int ReturnCode;
	char LenInfo[MSG_LEN_INFO_SIZE];
	int RetLen;
	char ReadBuff[READ_UNIT_SIZE];
	DWORD CopyingSize;
	int ResRecv;
	int Timeout;
	char *pReadingPoint;
	int ToReadSize;
	int UnitReadSize;
	int TimeoutOld;
	int OutLen;
	int LoopCount;
	int RecvAgainCount;

	bError = 0;
	ReturnCode = RET_FAIL;
	TimeoutOld = -1;

	if (0 > Socket || 0 == ErrorCode)
	{
		bError = 1;
		ReturnCode = RET_FAIL;
		goto ErrHand;
	}

	*ErrorCode = 0;

	OutLen = sizeof (TimeoutOld);
	if (SOCKET_ERROR == getsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&TimeoutOld, &OutLen))
	{
		*ErrorCode = WSAGetLastError();
		bError = 1;
		ReturnCode = RET_FAIL;
		goto ErrHand;
	}

	Timeout = TimeoutMilli; // Assigning is due to input the address of the Timeout. 0 = infinite.
	if (SOCKET_ERROR == setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&Timeout, sizeof (Timeout)))
	{
		*ErrorCode = WSAGetLastError();
		bError = 1;
		ReturnCode = RET_FAIL;
		goto ErrHand;
	}

	if (0 == Buffer || 0 == BufferSize)
	{
RecvAgainNew:		
		RecvAgainCount = 0;
RecvAgain:
		ZeroMemory(LenInfo, sizeof (LenInfo));
		ResRecv = recv(Socket, LenInfo, sizeof (LenInfo), MSG_PEEK);
		if (SOCKET_ERROR == ResRecv)
		{ 
			*ErrorCode = WSAGetLastError();
			
			if (WSAECONNRESET == *ErrorCode || WSAECONNABORTED == *ErrorCode || WSAESHUTDOWN == *ErrorCode || WSAENETRESET == *ErrorCode)
			{
				bError = 1;
				ReturnCode = RET_SOCKET_SHUT_DOWN;
				goto ErrHand;
			}
			else if (WSAETIMEDOUT == *ErrorCode)
			{
				bError = 1;
				ReturnCode = RET_TIMEOUT;
				goto ErrHand;
			}
			
			bError = 1;
			ReturnCode = RET_FAIL;
			goto ErrHand;
		}
		else if (0 == ResRecv) // socket closed.
		{
			bError = 1;
			ReturnCode = RET_SOCKET_CLOSED;
			goto ErrHand;
		}
		else if (sizeof (LenInfo) > ResRecv)
		{
			if (MAX_RECV_AGAIN <= RecvAgainCount)
			{
				bError = 1;
				ReturnCode = RET_FAIL;
				goto ErrHand;
			}
			RecvAgainCount++;
			goto RecvAgain;
		}
		else if (sizeof (LenInfo) != ResRecv)
		{
			bError = 1;
			ReturnCode = RET_FAIL;
			goto ErrHand;
		}
		else
		{
			memcpy(&RetLen, LenInfo, sizeof (LenInfo));
			if (CHECK_ALIVE_VALUE == RetLen)
			{
				ResRecv = recv(Socket, LenInfo, sizeof (LenInfo), 0); // recv() may return arbitrary value. <- but it can handle the size of data as much as MTU.
				if (SOCKET_ERROR == ResRecv)
				{ 
					*ErrorCode = WSAGetLastError();
					
					if (WSAECONNRESET == *ErrorCode || WSAECONNABORTED == *ErrorCode || WSAESHUTDOWN == *ErrorCode || WSAENETRESET == *ErrorCode)
					{
						bError = 1;
						ReturnCode = RET_SOCKET_SHUT_DOWN;
						goto ErrHand;
					}
					else if (WSAETIMEDOUT == *ErrorCode)
					{
						bError = 1;
						ReturnCode = RET_TIMEOUT;
						goto ErrHand;
					}
					
					bError = 1;
					ReturnCode = RET_FAIL;
					goto ErrHand;
				}
				else if (0 == ResRecv) // socket closed.
				{
					bError = 1;
					ReturnCode = RET_SOCKET_CLOSED;
					goto ErrHand;
				}

				goto RecvAgainNew;
			}
			else if (0 >= RetLen)
			{
				bError = 1;
				ReturnCode = RET_SOCKET_NO_ORDER;
				goto ErrHand;
			}
			else
			{
				bError = 0;
				ReturnCode = RetLen;
				goto ErrHand;
			}
		}
	}
	else // read the data.
	{
		CopyingSize = 0;
		LoopCount = 0;
		ToReadSize = BufferSize + sizeof (LenInfo);
		while (1)
		{
			if (sizeof (ReadBuff) <= ToReadSize)
			{
				UnitReadSize = sizeof (ReadBuff);
			}
			else
			{
				UnitReadSize = ToReadSize;
			}
			ZeroMemory(ReadBuff, sizeof (ReadBuff));
			pReadingPoint = ReadBuff;
			ResRecv = recv(Socket, pReadingPoint, UnitReadSize, 0);
			if (SOCKET_ERROR == ResRecv)
			{ 
				*ErrorCode = WSAGetLastError();

				if (WSAECONNRESET == *ErrorCode || WSAECONNABORTED == *ErrorCode || WSAESHUTDOWN == *ErrorCode || WSAENETRESET == *ErrorCode)
				{
					bError = 1;
					ReturnCode = RET_SOCKET_SHUT_DOWN;
					goto ErrHand;
				}
				else if (WSAETIMEDOUT == *ErrorCode)
				{
					bError = 1;
					ReturnCode = RET_TIMEOUT;
					goto ErrHand;
				}

				bError = 1;
				ReturnCode = RET_FAIL;
				goto ErrHand;
			}
			else if (0 == ResRecv) // socket closed.
			{
				bError = 1;
				ReturnCode = RET_SOCKET_CLOSED;
				goto ErrHand;
			}
			else
			{
				if (0 == LoopCount)
				{
					memcpy(Buffer + CopyingSize, ReadBuff + sizeof (LenInfo), ResRecv - sizeof (LenInfo));
					CopyingSize += ResRecv - sizeof (LenInfo);
				}
				else
				{
					memcpy(Buffer + CopyingSize, ReadBuff, ResRecv);
					CopyingSize += ResRecv;
				}
				LoopCount++;
				ToReadSize -= ResRecv;

				if (0 >= ToReadSize)
				{
					break;
				}
			}
		}

		bError = 0;
		ReturnCode = CopyingSize;
		goto ErrHand;
	}

ErrHand:
	if (-1 != TimeoutOld)
	{
		Timeout = TimeoutOld;
		if (SOCKET_ERROR == setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&Timeout, sizeof (Timeout)))
		{
			*ErrorCode = WSAGetLastError();
			bError = 1;
			ReturnCode = RET_FAIL;
			goto ErrHand;
		}
	}

	return ReturnCode;
} // TCPRead()

int TCPWrite(SOCKET Socket, char *Buffer, DWORD BufferSize, DWORD TimeoutMilli, DWORD *ErrorCode)
{
	BOOL bError;
	int ReturnCode;
	DWORD WritingSize;
	int ResSend;
	int Timeout;
	int TimeoutOld;
	int OutLen;
	int ToSendSize;
	int ToSendSizeUnit;
	int RemainedUnit;
	int Loop;
	int SendingNumber;
	char *pIndex;
	char WriteBuf[WRITE_UNIT_SIZE];
	char *pIndexUnit;

	bError = 0;
	ReturnCode = RET_FAIL;
	TimeoutOld = -1;

	if (0 > Socket || 0 == Buffer || 0 == BufferSize || 0 == ErrorCode)
	{
		bError = 1;
		ReturnCode = RET_FAIL;
		goto ErrHand;
	}

	*ErrorCode = 0;

	OutLen = sizeof (TimeoutOld);
	if (SOCKET_ERROR == getsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&TimeoutOld, &OutLen))
	{
		*ErrorCode = WSAGetLastError();
		bError = 1;
		ReturnCode = RET_FAIL;
		goto ErrHand;
	}

	Timeout = TimeoutMilli; // Assigning is due to input the address of the Timeout. 0 = infinite.
	if (SOCKET_ERROR == setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&Timeout, sizeof (Timeout)))
	{
		*ErrorCode = WSAGetLastError();
		bError = 1;
		ReturnCode = RET_FAIL;
		goto ErrHand;
	}

	SendingNumber = ((BufferSize + sizeof (BufferSize)) / WRITE_UNIT_SIZE) + ((0 != ((BufferSize + sizeof (BufferSize)) % WRITE_UNIT_SIZE)) ? 1 : 0);
	WritingSize = 0;
	pIndex = Buffer;
	ToSendSize = BufferSize;
	for (Loop = 0; Loop < SendingNumber; Loop++)
	{
		if (0 == Loop)
		{
			ZeroMemory(WriteBuf, sizeof (WriteBuf));
			ToSendSizeUnit = ((WRITE_UNIT_SIZE - sizeof (BufferSize)) > ToSendSize) ? ToSendSize : WRITE_UNIT_SIZE - sizeof (BufferSize);
			memcpy(WriteBuf, &BufferSize, sizeof (BufferSize));
			memcpy(WriteBuf + sizeof (BufferSize), pIndex, ToSendSizeUnit);
			pIndex += ToSendSizeUnit;
			ToSendSize -= ToSendSizeUnit;
			ToSendSizeUnit += sizeof (BufferSize); // compensation.
		}
		else
		{
			ZeroMemory(WriteBuf, sizeof (WriteBuf));
			ToSendSizeUnit = (WRITE_UNIT_SIZE > ToSendSize) ? ToSendSize : WRITE_UNIT_SIZE;
			memcpy(WriteBuf, pIndex, ToSendSizeUnit);
			pIndex += ToSendSizeUnit;
			ToSendSize -= ToSendSizeUnit;
		}

		RemainedUnit = ToSendSizeUnit;
		pIndexUnit = WriteBuf;
		while (0 < RemainedUnit)
		{
			ResSend = send(Socket, pIndexUnit, RemainedUnit, 0); // send() operates as all or nothing.
			if (SOCKET_ERROR == ResSend)
			{ 
				*ErrorCode = WSAGetLastError();
				
				if (WSAECONNRESET == *ErrorCode || WSAECONNABORTED == *ErrorCode || WSAESHUTDOWN == *ErrorCode || WSAENETRESET == *ErrorCode)
				{
					bError = 1;
					ReturnCode = RET_SOCKET_SHUT_DOWN;
					goto ErrHand;
				}
				else if (WSAETIMEDOUT == *ErrorCode)
				{
					bError = 1;
					ReturnCode = RET_TIMEOUT;
					goto ErrHand;
				}
				
				bError = 1;
				ReturnCode = RET_FAIL;
				goto ErrHand;
			}
			else if (0 == ResSend) // socket closed.
			{
				bError = 1;
				ReturnCode = RET_SOCKET_CLOSED;
				goto ErrHand;
			}
			else if (ResSend == RemainedUnit) // the normal case.
			{
				WritingSize += ResSend;
				RemainedUnit -= ResSend;
			}
			else if (ResSend > RemainedUnit)
			{
				bError = 1;
				ReturnCode = RET_FAIL;
				goto ErrHand;
			}
			else if (ResSend < RemainedUnit)
			{
				WritingSize += ResSend;
				RemainedUnit -= ResSend;
				pIndexUnit += ResSend;
			}
			else // what? error.
			{
				bError = 1;
				ReturnCode = RET_FAIL;
				goto ErrHand;
			}
		}
	}

ErrHand:
	if (-1 != TimeoutOld)
	{
		Timeout = TimeoutOld;
		if (SOCKET_ERROR == setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&Timeout, sizeof (Timeout)))
		{
			*ErrorCode = WSAGetLastError();
			bError = 1;
			ReturnCode = RET_FAIL;
		}
	}

	if (1 == bError)
	{
		return ReturnCode;
	}

	ReturnCode = WritingSize - sizeof (BufferSize);
	return ReturnCode;
} // TCPWrite()