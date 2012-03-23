// GameAutoPtr.h: declare for the CAutoPtr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEAUTOPTR_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
#define AFX_GAMEAUTOPTR_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_

#include "common.h"
#include "basetype.h"
#include "mycom.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// 说明：简单智能指针，不支持COPY和=。该模板需要Release()函数支持
// 该类的指针可以为NULL。
// 注意：包含该对象的类必须是个不能COYP和=的类。

template<TYPENAME T, TYPENAME T2=T>
class CAutoPtr
{
public:
	CAutoPtr() : m_ptr(NULL)					{}
	CAutoPtr(T* ptr) : m_ptr(ptr)				{}		// may be null
	~CAutoPtr()									{ Release(); }
	CAutoPtr& operator=(T* ptr)					{ if(m_ptr && m_ptr!=ptr) Release(); m_ptr=ptr; return *this; }
	T*	pop()									{ T* p=m_ptr; m_ptr=NULL; return p; }
private: // can't use these
	CAutoPtr& operator=(const CAutoPtr& ptr);
	CAutoPtr(const CAutoPtr&);

public: // create new
	T* New(CLS_ID id)								{ Release(); m_ptr=T2::CreateNew(id); ASSERT(m_ptr); return m_ptr; }
	void Release()									{ if(m_ptr) m_ptr->Release(); m_ptr=NULL; }

public:
	operator T*()									{ return m_ptr; }
	POINT_NO_RELEASE<T>*	operator->()			{ ASSERT(m_ptr); return static_cast<POINT_NO_RELEASE<T>* >(m_ptr); }
	operator const T*()	 	 const					{ return m_ptr; }
	const POINT_NO_RELEASE<T>*	operator->() const	{ ASSERT(m_ptr); return static_cast<POINT_NO_RELEASE<T>* >(m_ptr); }

protected:
	T*			m_ptr;
};
template<TYPENAME T, TYPENAME T2>
inline void		safe_release(CAutoPtr<T,T2>& pT)		{ pT.Release(); }


//////////////////////////////////////////////////////////////////////
// 说明：简单类JAVA对象，不支持COPY和=。该模板需要CreateNew()和Release()函数支持
// 该类的指针不能为NULL，对象必须存在。
// 注意：包含该对象的类必须是个不能COYP和=的类。

template<TYPENAME T, TYPENAME T2=T>
class CJavaObj
{
public:
	CJavaObj() : m_ptr(NULL)						{}
	~CJavaObj()										{ Release(); }
private: // can't use these
	CJavaObj& operator=(const CJavaObj& ptr);
	CJavaObj(const CJavaObj&);

public: // create new
	T* New(CLS_ID id)								{ CHECKF(!m_ptr); m_ptr=T2::CreateNew(id); ASSERT(m_ptr); return m_ptr; }
	void Release()									{ if(m_ptr) m_ptr->Release(); m_ptr=NULL; }

public:
	operator T*()									{ if(!m_ptr) m_ptr=T2::CreateNew(GUID_OF(T2)); ASSERT(m_ptr); return m_ptr; }
	POINT_NO_RELEASE<T>*	operator->()			{ if(!m_ptr) m_ptr=T2::CreateNew(GUID_OF(T2)); ASSERT(m_ptr); return static_cast<POINT_NO_RELEASE<T>* >(m_ptr); }
	operator const T*()	 	 const					{ if(!m_ptr) m_ptr=T2::CreateNew(GUID_OF(T2)); ASSERT(m_ptr); return m_ptr; }
	const POINT_NO_RELEASE<T>*	operator->() const	{ if(!m_ptr) m_ptr=T2::CreateNew(GUID_OF(T2)); ASSERT(m_ptr); return static_cast<POINT_NO_RELEASE<T>* >(m_ptr); }

protected:
	T*			m_ptr;
};
template<TYPENAME T, TYPENAME T2>
inline void		safe_release(CJavaObj<T,T2>& pT)		{ pT.Release(); }


//////////////////////////////////////////////////////////////////////
// 说明：普通指针，不支持COPY和=。该模板“不需要”Release()函数支持
// 该类的指针可以为NULL。

template<TYPENAME T, TYPENAME T2=T>
class CAutoRef
{
public:
	CAutoRef() : m_ptr(NULL)					{}
	CAutoRef(T* ptr) : m_ptr(ptr)				{}		// may be null
	~CAutoRef()									{}
	CAutoRef& operator=(T* ptr)					{ m_ptr=ptr; return *this; }
	T*	pop()									{ T* p=m_ptr; m_ptr=NULL; return p; }
public: // enable
	CAutoRef& operator=(const CAutoRef& ptr)	{ m_ptr=ptr.m_ptr; }
	CAutoRef(const CAutoRef&)					{ m_ptr=ptr.m_ptr; }

public:
	operator T*()									{ return m_ptr; }
	POINT_NO_RELEASE<T>*	operator->()			{ ASSERT(m_ptr); return static_cast<POINT_NO_RELEASE<T>* >(m_ptr); }
	operator const T*()	 	 const					{ return m_ptr; }
	const POINT_NO_RELEASE<T>*	operator->() const	{ ASSERT(m_ptr); return static_cast<POINT_NO_RELEASE<T>* >(m_ptr); }

protected:
	T*			m_ptr;
};





#endif // !defined(AFX_GAMEAUTOPTR_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
