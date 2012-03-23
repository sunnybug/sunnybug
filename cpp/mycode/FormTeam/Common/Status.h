// CStatus.h: interface for the CStatus class.
// paled, 2004.9.1
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUS_H__D9E42F53_A4BD_4BB9_9745_D3FE8666BA41__INCLUDED_)
#define AFX_STATUS_H__D9E42F53_A4BD_4BB9_9745_D3FE8666BA41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
template <TYPENAME T, T I=T()>
class CStatus
{
public:
	CStatus()											{ m_status = I; }
	CStatus(const CStatus& obj)							{ m_status = obj.m_status; }
	CStatus<T,I>& operator=(const CStatus<T,I>& obj)	{ if(this==&obj) return *this; m_status = obj.m_status; return *this; }
	~CStatus()											{}

public: // application
	void	Reset()										{ m_status = (T)0; }
	T		Next()										{ m_status = (T)(m_status+1); return m_status; }
	void	Set(T status)								{ m_status = status; }

public: // operator
	CStatus(T status)									{ m_status = I; }
	CStatus<T,I>& operator=(T status)					{ m_status = status; return *this; }
	operator T()										{ return m_status; }

public: // const
	bool	IsStatus(T status) const					{ return status == m_status; }

protected:
	T		m_status;
};

#endif // !defined(AFX_STATUS_H__D9E42F53_A4BD_4BB9_9745_D3FE8666BA41__INCLUDED_)
