// MsgBuf.h: interface for the CMsgBuf_ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGBUF_H__31E3D462_386A_4960_9A98_DD5C1846C03C__INCLUDED_)
#define AFX_MSGBUF_H__31E3D462_386A_4960_9A98_DD5C1846C03C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template< int SZ >
class CMsgBuf_  
{
public:
	virtual ~CMsgBuf_() {}
public: // send mode
	CMsgBuf_(int nReserveHead=16)	{ CHECK(nReserveHead>=0 && nReserveHead<SZ); memset(m_buf, 0, SZ); m_size=0; m_pHead=m_pData=m_buf+nReserveHead; m_bSendMode=true; }
	bool	PushLong(int nData)		{ CHECKF(m_bSendMode && m_pData+sizeof(long)<=m_buf+SZ); *((long*)m_pData)=nData; m_pData+=sizeof(long); return true; }
	bool	PushShort(int nData)	{ CHECKF(m_bSendMode && m_pData+sizeof(short)<=m_buf+SZ); *((short*)m_pData)=nData; m_pData+=sizeof(short); return true; }
	bool	PushChar(int nData)		{ CHECKF(m_bSendMode && m_pData+sizeof(char)<=m_buf+SZ); *((char*)m_pData)=nData; m_pData+=sizeof(char); return true; }
	bool	PushHeadLong(int nData)		{ CHECKF(m_bSendMode && m_pHead-sizeof(long)>=m_buf); m_pHead-=sizeof(long); *((long*)m_pHead)=nData; return true; }
	bool	PushHeadShort(int nData)	{ CHECKF(m_bSendMode && m_pHead-sizeof(short)>=m_buf); m_pHead-=sizeof(short); *((short*)m_pHead)=nData; return true; }
	bool	PushHeadChar(int nData)		{ CHECKF(m_bSendMode && m_pHead-sizeof(char)>=m_buf); m_pHead-=sizeof(char); *((char*)m_pHead)=nData; return true; }
public: // recv mode
	CMsgBuf_(char* buf, int nSize)	{ CHECK(nSize>0 && nSize<=SZ); memcpy(m_buf, buf, nSize); m_size=nSize; m_pHead=m_pData=m_buf; m_bSendMode=false; }
	int		PopLong()		{ CHECKF(!m_bSendMode && m_pData+sizeof(long)<=m_buf+m_size); int nData=*((long*)m_pData); m_pData+=sizeof(long); return nData; }
	int		PopShort()		{ CHECKF(!m_bSendMode && m_pData+sizeof(short)<=m_buf+m_size); int nData=*((short*)m_pData); m_pData+=sizeof(short); return nData; }
	int		PopChar()		{ CHECKF(!m_bSendMode && m_pData+sizeof(char)<=m_buf+m_size); int nData=*((char*)m_pData); m_pData+=sizeof(char); return nData; }
	int		PopUshort()		{ CHECKF(!m_bSendMode && m_pData+sizeof(short)<=m_buf+m_size); int nData=*((unsigned short*)m_pData); m_pData+=sizeof(short); return nData; }
	int		PopUchar()		{ CHECKF(!m_bSendMode && m_pData+sizeof(char)<=m_buf+m_size); int nData=*((unsigned char*)m_pData); m_pData+=sizeof(char); return nData; }
protected:
	char*		m_pHead;
	char*		m_pData;
	int			m_size;				// only for recv
	bool		m_bSendMode;		// true: 发送方，false: 接收方
	char		m_buf[SZ];
};
typedef	CMsgBuf_<1024> CMsgBuf;

#endif // !defined(AFX_MSGBUF_H__31E3D462_386A_4960_9A98_DD5C1846C03C__INCLUDED_)
