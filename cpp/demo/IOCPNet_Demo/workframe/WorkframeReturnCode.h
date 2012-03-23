////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		WorkframeReturnCode.h
//	Author:
//		Chun-Hyok, Chong.
//	Description:
//		It provides the return codes used in the Online Game Server.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WORKFRAMERETURNCODE_H__
#define __WORKFRAMERETURNCODE_H__

// Common Return Code.
#define RET_SUCCESS										1
#define RET_FAIL										-1

#define RET_NET_NOT_INIT								-2
#define RET_NO_ERROR									1
#define RET_ACCEPT_SOCKET_ERROR							-1
#define RET_CLIENT_SOCKET_ERROR							-2
#define RET_NO_SOCKET									-3
#define RET_NO_CONNECTION								-4
#define RET_SOCKET_CLOSED								0
#define RET_TIMEOUT										-5
#define RET_SOCKET_SHUT_DOWN							-6
#define RET_SERVER_NOT_PREPARED							-7
#define RET_SOCKET_NO_ORDER								-8
#define RET_CONNECT										-9
#define RET_DISCONNECT									-10
#define RET_NO_AVAILABLE_PEER							-11
#define RET_NO_PEER										-12
#define RET_NO_PACKAGE									-13

#define READ_TIMEOUT									5000
#define WRITE_TIMEOUT									5000

#endif // __WORKFRAMERETURNCODE_H__