// GameAutoPtr.h: declare for the CAutoPtr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOITER_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
#define AFX_AUTOITER_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_

#include "common.h"
#include "basetype.h"
#include "I_Iterator.h"
using namespace ak47;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// 说明：叠代器指针封装，不支持COPY和=。
// 可以重复给CAutoIter赋值。
// 该类的指针不可以为NULL。

template<TYPENAME T>
class CAutoIter
{
	typedef	IIterator<T>	IIter;
public:
//	CAutoIter() : m_ptr(NULL)					{}
	CAutoIter(IIter& ref) : m_ptr(&ref)			{ ASSERT(m_ptr); }
	~CAutoIter()								{ ReleasePtr(); }
	CAutoIter& operator=(IIter& ref)			{ ReleasePtr(); m_ptr=&ref; ASSERT(m_ptr); return *this; }
private: // can't use these
	CAutoIter& operator=(const CAutoIter& ptr);
	CAutoIter(const CAutoIter&);
protected:
	virtual	void	ReleasePtr()				{ if(m_ptr) m_ptr->Release(); m_ptr=NULL; }

public:
	virtual	bool	Next()						{ return m_ptr->Next(); }
	virtual	void	Reset()						{ m_ptr->Reset(); }
	virtual	operator T*()						{ ASSERT(m_ptr); return *m_ptr; }
	virtual	T*		operator->()				{ ASSERT(m_ptr); return *m_ptr; }

	virtual operator IIter&()					{ return *m_ptr; }

protected:
	IIter*		m_ptr;
};





#endif // !defined(AFX_AUTOITER_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
