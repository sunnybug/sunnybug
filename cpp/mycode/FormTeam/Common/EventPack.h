// EventPack.h: interface for the CEventPack class.
// paled: 2004.3.11
// 内部消息打包工具。
//
// 打包：
// CEventPack(DWORD idEvent, ULONG idKey, DWORD idAction=0);	// 用于消息打包
//																// 目前 idEvent 和 idAction 只支持USHORT, idKey 支持DWORD。
// CEventPack& operator <<(...)									// 用于向消息包添加附加参数
//
// 解析：
// CEventPack(const char* pPackBuf, int nPackSize);				// 用于消息解析
// CEventPack& operator >>(...);								// 用于从消息中解析出附加参数
//
// EVENTPACK_MORE_CHECK : 可以检查出前32个参数的类型错误。
//////////////////////////////////////////////////////////////////////
// 常用方法：
// idEvent 通常用于CLS_ID，也就是传到哪个类
// idAction 通常用于消息的类型，所以WORD就够了
// idKey 通常用于对象ID，通过GetID()能取到
// 这样就能从一个对象向另一个对象发送内部消息了


#if !defined(AFX_EVENTPACK_H__7565C398_0FA3_4E78_9344_860F1D15C840__INCLUDED_)
#define AFX_EVENTPACK_H__7565C398_0FA3_4E78_9344_860F1D15C840__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "basefunc.h"

#define	EVENTPACK_MORE_CHECK

#ifdef	EVENTPACK_MORE_CHECK
const int EVENTPACK_HEAD		= sizeof(USHORT) + sizeof(ULONG) + sizeof(DWORD) + sizeof(DWORD);
#else
const int EVENTPACK_HEAD		= sizeof(USHORT) + sizeof(ULONG) + sizeof(DWORD);
#endif
const int EVENTPACK_SIZE		= 1024 + EVENTPACK_HEAD;

class CEventPack  
{
public:
	CEventPack(DWORD idEvent, ULONG idKey, DWORD idAction=0);
	CEventPack(const char* pPackBuf, int nPackSize);
	virtual ~CEventPack();

public: // appilcation
	class Buffer;
	class PackBuffer;
	CEventPack& operator <<(const int nData);
	CEventPack& operator <<(const String& str);
	CEventPack& operator <<(const Buffer buf);			// size >=0 && <=255
	CEventPack& operator <<(const PackBuffer& buf);		// size >=0 && <=255
	CEventPack& operator >>(int& nData);
	CEventPack& operator >>(String& str);
	CEventPack& operator >>(Buffer buf);				// size >=0 && <=255, psize 返回长度
	CEventPack& operator >>(PackBuffer& buf);			// size >=0 && <=255

	CEventPack& operator >>(unsigned int& nData)	{ return operator >>( (int&) nData ); }
	CEventPack& operator >>(long& nData)			{ return operator >>( (int&) nData ); }
	CEventPack& operator >>(unsigned long& nData)	{ return operator >>( (int&) nData ); }

public: // const
	int		GetObjType()	const		{ return m_idObjType; }
	ULONG	GetObjID()		const		{ return m_idKey; }
	int		GetAction()		const		{ return m_idAction; }

	const char*	GetBuf()	const		{ return m_buf; }
	int		GetSize()		const		{ return m_bSplit ? m_size : m_ptr-m_buf; }

protected:
	union{
		char	m_buf[EVENTPACK_SIZE +1];		// +1: 冗余，多出一个，用于防止串溢出
		struct{
			DWORD	m_idObjType;				// 这块的尺寸必须和EVENTPACK_HEAD相同
			USHORT	m_idAction;
			ULONG	m_idKey;
#ifdef	EVENTPACK_MORE_CHECK
			DWORD	m_bitParam;					// 参数类型，按BIT从高到低(0：INT，1：STR)
#endif
			char	m_setParam[1];
		};
	};

protected: // ctrl
	bool	IsUnite()				{ return !m_bSplit; }
	bool	IsSplit()				{ return m_bSplit; }
	bool	CheckPush(int nSize)	{ return m_ptr+nSize <= m_buf + EVENTPACK_SIZE; }
	bool	CheckPop(int nSize)		{ return m_ptr+nSize <= m_buf + m_size; }
protected:
	bool	m_bSplit;		// false: 发送方，true: 接收方
	char*	m_ptr;			// 当前数据指针。发送方下一次写入的位置，或者接收方下一次可读入的位置
	int		m_size;			// 接收方的消息SIZE，不用于发送方
#ifdef	EVENTPACK_MORE_CHECK
	UCHAR	m_nCount;		// 参数计数，不传过去。从0开始，最多32。
#endif

public:
	///////////////////////////////////
	// 内含类
	///////////////////////////////////
	// 例子
	// pack << 3 << 4 << CEventPack::Buffer(pBuf, 3) << "56" << 56 << "78";
	// pack2 >> a >> b >> CEventPack::Buffer(buf, 3) >> sb >> c >> sc;			// 校验长度为3
	// pack2 >> a >> b >> CEventPack::Buffer(buf, &nSize) >> sb >> c >> sc;		// 取出长度
	class Buffer
	{
	public:
		Buffer(char* pBuf, int nSize) : ptr(pBuf), size(nSize), psize(0) {
		}			// 仅校验长度
		Buffer(char* pBuf, int* pSize) : ptr(pBuf), size(0), psize(pSize) {
		}		// 返回长度
	private:
		char*	ptr;
		int		size;
		int*	psize;
		friend CEventPack;
	};
	class PackBuffer
	{
	public:
		PackBuffer()  { Clear(); }
		PackBuffer(const char* pBuf, int nSize) { Clear(); m_nSize=nSize; memcpy(m_Buffer, pBuf, nSize);}
		char*	GetBuf()	const	{ return (char*)m_Buffer; }
		int		GetSize()	const	{ return m_nSize; }
		void	Clear()		{ memset(m_Buffer, 0, sizeof(m_Buffer)); m_nSize=0; }

	private:
		char m_Buffer[255];
		int  m_nSize;
		friend CEventPack;
	};
};

#endif // !defined(AFX_EVENTPACK_H__7565C398_0FA3_4E78_9344_860F1D15C840__INCLUDED_)

