// EventPack.cpp: implementation of the CEventPack class.
// paled: 2004.3.11
// 内部消息打包工具。
//
// 打包：
// CEventPack(DWORD idEvent, ULONG idKey, DWORD idAction=0);	// 用于消息打包
//																// 目前 idEvent 和 idAction 只支持USHORT，idKey 支持DWORD。
// CEventPack& operator <<(...)									// 用于向消息包添加附加参数
//
// 解析：
// CEventPack(const char* pPackBuf, int nPackSize);				// 用于消息解析
// CEventPack& operator >>(...);								// 用于从消息中解析出附加参数
//////////////////////////////////////////////////////////////////////

#include "EventPack.h"
#include "limits.h"
#include "common.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEventPack::CEventPack(DWORD idEvent, ULONG idKey, DWORD idAction/*=ID_NONE*/)
{
	ASSERT(idAction < USHRT_MAX);

	m_idObjType		= idEvent;
	m_idKey			= idKey;
	m_idAction		= (USHORT)idAction;
#ifdef	EVENTPACK_MORE_CHECK
	m_bitParam		= 0;
#endif

	m_bSplit		= false;
	m_ptr			= m_setParam;
	m_size			= 0;
#ifdef	EVENTPACK_MORE_CHECK
	m_nCount		= 0;
#endif
}

//////////////////////////////////////////////////////////////////////
CEventPack::CEventPack(const char* pPackBuf, int nPackSize)
{
	memset(m_buf, 0, EVENTPACK_SIZE);
	IF_OK(nPackSize >= EVENTPACK_HEAD && nPackSize <= EVENTPACK_SIZE)
		memcpy(m_buf, pPackBuf, nPackSize);

	m_bSplit		= true;
	m_ptr			= m_setParam;
	m_size			= nPackSize;

	// 防止串溢出
	ASSERT(sizeof(m_buf) == EVENTPACK_SIZE +1);
	m_buf[EVENTPACK_SIZE]	= 0;
#ifdef	EVENTPACK_MORE_CHECK
	m_nCount		= 0;
#endif
}

//////////////////////////////////////////////////////////////////////
CEventPack::~CEventPack()
{
#ifdef	EVENTPACK_MORE_CHECK
	if(m_bSplit)
	{
		ASSERT(m_ptr <= m_buf + m_size);	/* STRICT == use over */
	}
#endif
}

//////////////////////////////////////////////////////////////////////
// appilcation
//////////////////////////////////////////////////////////////////////
CEventPack& CEventPack::operator <<(const int nData)
{
	IF_NOT(IsUnite())
		return *this;
	IF_NOT(CheckPush(sizeof(long)))
		return *this;

	*((long*)m_ptr)	= nData;
	m_ptr	+= sizeof(long);

#ifdef	EVENTPACK_MORE_CHECK
	if(m_nCount < 32)
	{
		m_nCount++;
	}
#endif
	return *this;
}

//////////////////////////////////////////////////////////////////////
CEventPack& CEventPack::operator <<(const String& str)
{
	IF_NOT(IsUnite())
		return *this;
	IF_NOT(CheckPush( strlen((const char*)str)+1 ))
		return *this;

	SafeCopy(m_ptr, (const char*)str, 0);
	m_ptr	+= strlen((const char*)str)+1;

#ifdef	EVENTPACK_MORE_CHECK
	if(m_nCount < 32)
	{
		m_nCount++;
		m_bitParam |= (1 << (32-m_nCount));
	}
#endif
	return *this;
}
CEventPack& CEventPack::operator <<(const PackBuffer& buf)
{
	IF_NOT(buf.GetSize() >= 0 && buf.GetSize() <= 255)
		return *this;
	IF_NOT(IsUnite())
		return *this;
	IF_NOT(CheckPush( 1+buf.GetSize() ))
		return *this;

	*(m_ptr++)	= buf.GetSize();
	if(buf.GetSize())
	{
		memcpy(m_ptr, buf.GetBuf(), buf.GetSize());
		m_ptr	+= buf.GetSize();
	}

#ifdef	EVENTPACK_MORE_CHECK	//???????????????????????????  与串类型共用，未能实现更强的类型检测。
	if(m_nCount < 32)
	{
		m_nCount++;
		m_bitParam |= (1 << (32-m_nCount));
	}
#endif
	return *this;	
}
//////////////////////////////////////////////////////////////////////
CEventPack& CEventPack::operator <<(const CEventPack::Buffer buf)
{
	IF_NOT(buf.ptr && buf.size >= 0 && buf.size <= 255 && buf.psize == 0)
		return *this;
	IF_NOT(IsUnite())
		return *this;
	IF_NOT(CheckPush( 1+buf.size ))
		return *this;

	*(m_ptr++)	= buf.size;
	if(buf.size)
	{
		memcpy(m_ptr, buf.ptr, buf.size);
		m_ptr	+= buf.size;
	}

#ifdef	EVENTPACK_MORE_CHECK	//???????????????????????????  与串类型共用，未能实现更强的类型检测。
	if(m_nCount < 32)
	{
		m_nCount++;
		m_bitParam |= (1 << (32-m_nCount));
	}
#endif
	return *this;
}

//////////////////////////////////////////////////////////////////////
CEventPack& CEventPack::operator >>(int& nData) 
{
	IF_NOT(IsSplit())
		return *this;
	IF_NOT(CheckPop(sizeof(long)))
		return *this;

	nData	= *((long*)m_ptr);
	m_ptr	+= sizeof(long);

#ifdef	EVENTPACK_MORE_CHECK
	if(m_nCount < 32)
	{
		ASSERT((m_bitParam & 0x80000000) == 0);
		m_bitParam <<= 1;
		m_nCount++;
	}
#endif
	return *this;
}

//////////////////////////////////////////////////////////////////////
CEventPack& CEventPack::operator >>(String& str) 
{
	IF_NOT(IsSplit())
		return *this;
	IF_NOT(CheckPop( strlen(m_ptr)+1 ))
		return *this;

	str		= m_ptr;
	m_ptr	+= strlen((const char*)str)+1;

#ifdef	EVENTPACK_MORE_CHECK
	if(m_nCount < 32)
	{
		ASSERT((m_bitParam & 0x80000000) != 0);
		m_bitParam <<= 1;
		m_nCount++;
	}
#endif
	return *this;
}



CEventPack& CEventPack::operator >>(CEventPack::PackBuffer& buf) 
{
	buf.Clear();
	IF_NOT(IsSplit())
		return *this;
	IF_NOT(CheckPop( 1 ))
		return *this;
	buf.m_nSize = *(m_ptr);
	char* pBuf = m_ptr + 1;
	IF_NOT(CheckPop( 1+buf.m_nSize ))
		return *this;
	m_ptr++;		// size
	if(buf.m_nSize)
	{
		memcpy(buf.m_Buffer, pBuf, buf.m_nSize);
		m_ptr	+= buf.m_nSize;
	}

#ifdef	EVENTPACK_MORE_CHECK
	if(m_nCount < 32)
	{
		ASSERT((m_bitParam & 0x80000000) != 0);
		m_bitParam <<= 1;
		m_nCount++;
	}
#endif

	return *this;
}
//////////////////////////////////////////////////////////////////////
CEventPack& CEventPack::operator >>(CEventPack::Buffer buf) 
{
	IF_NOT(buf.ptr && buf.size >= 0 && buf.size <= 255)
		return *this;
	IF_NOT(IsSplit())
		return *this;

	IF_NOT(CheckPop( 1 ))
		return *this;
	int nSize = *(m_ptr);
	char* pBuf = m_ptr + 1;

	IF_NOT(CheckPop( 1+nSize ))
		return *this;

	if(buf.psize)
	{
		*buf.psize = nSize;
	}
	else
	{
		IF_NOT(nSize == buf.size)		// 校验
			return *this;
	}

	m_ptr++;		// size
	if(nSize)
	{
		memcpy(buf.ptr, pBuf, nSize);
		m_ptr	+= nSize;
	}

#ifdef	EVENTPACK_MORE_CHECK
	if(m_nCount < 32)
	{
		ASSERT((m_bitParam & 0x80000000) != 0);
		m_bitParam <<= 1;
		m_nCount++;
	}
#endif

	return *this;
}

