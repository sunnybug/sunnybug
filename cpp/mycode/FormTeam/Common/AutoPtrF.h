// GameAutoPtr.h: declare for the CAutoPtrF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEAUTOPTRF_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
#define AFX_GAMEAUTOPTRF_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_

#include "common.h"
#include "basetype.h"
#include "mycom.h"
#include "MyClassFactory.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// 说明：简单智能指针，不支持COPY和=。该模板需要CMyClassFactory类厂支持
// 该类的指针可以为NULL。
// 注意：包含该对象的类必须是个不能COYP和=的类。

template<TYPENAME T, TYPENAME T2=T>
class CAutoPtrF
{
public:
	CAutoPtrF() : m_ptr(NULL)					{}
	CAutoPtrF(T* ptr) : m_ptr(ptr)				{}		// may be null
	~CAutoPtrF()									{ Release(); }
	CAutoPtrF& operator=(T* ptr)					{ if(m_ptr && m_ptr!=ptr) Release(); m_ptr=ptr; return *this; }
	T*	pop()										{ T* p=m_ptr; m_ptr=NULL; return p; }
private: // can't use these
	CAutoPtrF& operator=(const CAutoPtrF& ptr);
	CAutoPtrF(const CAutoPtrF&);

public: // create new
	T* New(CLS_ID id)								{ Release(); CreateNewObj(GUID_OF(T2), PPV_OF(m_ptr)); MYASSERT(m_ptr); return m_ptr; }
	void Release()									{ if(m_ptr) ReleaseObj(GUID_OF(T2), PPV_OF(m_ptr)); m_ptr=NULL; }

public:
	operator T*()									{ return m_ptr; }
	T*	operator->()			{ MYASSERT(m_ptr); return static_cast<T* >(m_ptr); }
	operator const T*()	 	 const					{ return m_ptr; }
	const T*	operator->() const	{ MYASSERT(m_ptr); return static_cast<T* >(m_ptr); }

protected:
	T*			m_ptr;
};
template<TYPENAME T, TYPENAME T2>
inline void		safe_release(CAutoPtrF<T,T2>& pT)		{ pT.Release(); }


//////////////////////////////////////////////////////////////////////
// 说明：简单类JAVA对象，不支持COPY和=。该模板需要CMyClassFactory类厂支持
// 该类的指针不能为NULL，对象必须存在。
// 注意：包含该对象的类必须是个不能COYP和=的类。

template<TYPENAME T, TYPENAME T2=T>
class CJavaObjF
{
public:
	CJavaObjF() : m_ptr(NULL)						{}
	~CJavaObjF()										{ Release(); }
private: // can't use these
	CJavaObjF& operator=(const CJavaObjF& ptr);
	CJavaObjF(const CJavaObjF&);

public: // create new
	T* New(CLS_ID id)								{ CHECKF(!m_ptr); CreateNewObj(GUID_OF(T2), PPV_OF(m_ptr)); MYASSERT(m_ptr); return m_ptr; }
	void Release()									{ if(m_ptr) ReleaseObj(GUID_OF(T2), PPV_OF(m_ptr)); m_ptr=NULL; }

public:
	operator T*()									{ if(!m_ptr) CreateNewObj(GUID_OF(T2), PPV_OF(m_ptr)); MYASSERT(m_ptr); return m_ptr; }
	T*	operator->()								{ if(!m_ptr) CreateNewObj(GUID_OF(T2), PPV_OF(m_ptr)); MYASSERT(m_ptr); return static_cast<T* >(m_ptr); }
	operator const T*()	 	 const					{ if(!m_ptr) CreateNewObj(GUID_OF(T2), PPV_OF(m_ptr)); MYASSERT(m_ptr); return m_ptr; }
	const T*	operator->() const					{ if(!m_ptr) CreateNewObj(GUID_OF(T2), PPV_OF(m_ptr)); MYASSERT(m_ptr); return static_cast<T* >(m_ptr); }

protected:
	T*			m_ptr;
};
template<TYPENAME T, TYPENAME T2>
inline void		safe_delete(CJavaObjF<T,T2>& pT)		{ pT.Release(); }


//////////////////////////////////////////////////////////////////////
// 说明：普通指针，不支持COPY和=。该模板“不需要”Release()函数支持
// 该类的指针可以为NULL。

template<TYPENAME T, TYPENAME T2>
class CAutoRefF
{
public:
	CAutoRefF() : m_ptr(NULL)					{}
	CAutoRefF(T* ptr) : m_ptr(ptr)				{}		// may be null
	~CAutoRefF()									{}
	CAutoRefF& operator=(T* ptr)					{ m_ptr=ptr; return *this; }
	T*	pop()									{ T* p=m_ptr; m_ptr=NULL; return p; }
public: // enable
	CAutoRefF& operator=(const CAutoRefF& ptr)	{ m_ptr=ptr.m_ptr; }
	CAutoRefF(const CAutoRefF&)					{ m_ptr=ptr.m_ptr; }

public:
	operator T*()									{ return m_ptr; }
	T*	operator->()			{ MYASSERT(m_ptr); return static_cast<T* >(m_ptr); }
	operator const T*()	 	 const					{ return m_ptr; }
	const T*	operator->() const	{ MYASSERT(m_ptr); return static_cast<T* >(m_ptr); }

protected:
	T*			m_ptr;
};





#endif // !defined(AFX_GAMEAUTOPTR_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
