// QiPtr.h: declare for the CQiPtr class.
// paled, 2004.11.29
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QIPTR_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
#define AFX_QIPTR_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_

#include "common.h"
#include "basetype.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// 说明：COM风格指针。用于自动转换接口指针的类型。
//       指针所指向的接口，要有QueryInterface函数可供调用。

template<TYPENAME T>
class CQiPtr
{
public:
	CQiPtr() : m_ptr(NULL)						{}
	~CQiPtr()									{}

	template<TYPENAME X>
		CQiPtr(CQiPtr<X>& obj)					{ if(obj) obj->QueryInterface(GUID_OF(T), (void**)&m_ptr); else m_ptr = NULL; }
	CQiPtr(CQiPtr<T>& obj)						{ m_ptr = (T*)obj; }

	template<TYPENAME X>
		CQiPtr& operator=(CQiPtr<X>& obj)		{ if(obj) obj->QueryInterface(GUID_OF(T), (void**)&m_ptr); else m_ptr = NULL; return *this; }
	CQiPtr& operator=(CQiPtr<T>& obj)			{ m_ptr = obj; return *this; }

	template<TYPENAME X>
		CQiPtr(X* ptr)							{ if(ptr) ptr->QueryInterface(GUID_OF(T), (void**)&m_ptr); else m_ptr = NULL; }
	CQiPtr(T* ptr)								{ m_ptr = ptr; }

	template<TYPENAME X>
		CQiPtr& operator=(X* ptr)				{ if(ptr) ptr->QueryInterface(GUID_OF(T), (void**)&m_ptr); else m_ptr = NULL; return *this; }
	CQiPtr& operator=(T* ptr)					{ m_ptr = ptr; return *this; }

	CQiPtr(int ptr)								{ ASSERT(ptr==0); m_ptr = (T*)ptr; }		// for NULL

public: // create new
	T* New(CLS_ID id)							{ m_ptr=T::CreateNew(id); ASSERT(m_ptr); return m_ptr; }

public:
	operator T*()								{ return m_ptr; }
	T*	operator->()							{ ASSERT(m_ptr); return m_ptr; }
	operator const T*()	 	 const				{ return m_ptr; }
	const T*	operator->() const				{ ASSERT(m_ptr); return m_ptr; }

protected:
	T*			m_ptr;
};





#endif // !defined(AFX_QIPTR_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
