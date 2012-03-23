////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		TCPFunc.h
//	Author:
//		Chun-Hyok, Chong.
//	Description:
//		It provides the basic network functions handle reading/writing/packaging rough(?) data.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TCPFUNC_H_
#define __TCPFUNC_H_

#include "Workframe.h"

#define TCP_CHECK_ALIVE_MSG						"checkalive"
#define TCP_CHECK_ALIVE_LEN						10

int TCPRead(SOCKET Socket, char *Buffer, DWORD BufferSize, DWORD TimeoutMilli, DWORD *ErrorCode);
int TCPWrite(SOCKET Socket, char *Buffer, DWORD BufferSize, DWORD TimeoutMilli, DWORD *ErrorCode);

#endif // __TCPFUNC_H_