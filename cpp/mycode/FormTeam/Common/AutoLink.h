// 自动链接类
// 设计：仙剑修, 2003.7.7
// 修改：仙剑修，2004.11.21
////////////////////////////////////////////////////////////////
// 说明:“主AutoLink”必须在构造函数中，用父对象的指针初始化自己。例如: Init(this);
//		“从AutoLink”可使用赋值的方式关联到主LINK。(注意：最好不要出现太多的临时对象，否则会有一定代价)。例如: m_lnkUser = pUser;
//		“从AutoLink”内部的指针可能随时被“主AutoLink”清空，所以每次使用“从AutoLink”时必须先检查是否为空。(!)
////////////////////////////////////////////////////////////////
//		“主AutoLink”的父对象必须实现一个成员函数。例如: const AUTOLINK_NOTE* QueryLink() const	{ return m_link.QueryNote(); }


#ifndef	AUTO_LINK_H
#define	AUTO_LINK_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"
#include "basetype.h"

////////////////////////////////////////////////////////////////
// 程序员不要直接使用此结构。仅在游戏对象的QueryLink()中自动使用。
struct AUTOLINK_NOTE{
	void*			pOwner;
	AUTOLINK_NOTE*	pPrev;
	AUTOLINK_NOTE*	pNext;

	void Clear()	{ pOwner = pPrev = pNext = NULL; }
};

////////////////////////////////////////////////////////////////
template<TYPENAME T>
class	CAutoLink;
template<TYPENAME T>
class	CAutoLink
{
public:
	CAutoLink()										{ m_data.Clear(); }
	~CAutoLink()									{ Break(); }
	CAutoLink(const AUTOLINK_NOTE* pNote, T* ptr)	{ m_data.Clear(); Insert(ptr->QueryLink(), ptr); }
	CAutoLink(const CAutoLink<T>& link)				{ m_data.Clear(); if(link.m_data.pOwner) Insert(&link.m_data, link.m_data.pOwner); }
	CAutoLink(T* ptr)								{ m_data.Clear(); if(ptr) Insert(ptr->QueryLink(), ptr); }
	CAutoLink&	operator=(const CAutoLink<T>& link)	{ Break(); if(link.m_data.pOwner) Insert(&link.m_data, link.m_data.pOwner); return *this; }
	CAutoLink&	operator=(T* ptr)					{ Break(); if(ptr) Insert(ptr->QueryLink(), ptr); return *this; }

public: // modify
	void	Init(T* pOwner)							{ MYASSERT(!IsValid() && pOwner); m_data.pOwner=pOwner; }

public: // const for client call
	operator T*() const								{ return (T*)m_data.pOwner; }
	POINT_NO_RELEASE<T>* operator->() const			{ MYASSERT(IsValid()); return static_cast<POINT_NO_RELEASE<T>* >((T*)m_data.pOwner); }

public: // const for auto call
	const AUTOLINK_NOTE* QueryNote() const			{ return &m_data; }

protected:
	void	Insert(const AUTOLINK_NOTE* pPrev, void* pOwner);		// insert to next, only for !IsMaster
	void	Erase();							// erase from list, only for !IsMaster
	void	Break();							// clear this note. Erase() for !IsMaster, erase all note for IsMaster
	bool	IsValid()		const					{ return m_data.pOwner != NULL; }
	bool	IsMaster()		const					{ return IsValid() && m_data.pPrev == NULL; }

public:
protected:
	AUTOLINK_NOTE m_data;
};

////////////////////////////////////////////////////////////////
template<TYPENAME T>
void CAutoLink<T>::Break()
{
	if(IsMaster())		// master
	{
		AUTOLINK_NOTE* pNextNote = m_data.pNext;
		int	nCount=0;
		while(pNextNote)
		{
			AUTOLINK_NOTE* pCurr = pNextNote;
			pNextNote	= pNextNote->pNext;			// ++

			MYASSERT(pCurr->pOwner != NULL);
			pCurr->Clear();

			DEAD_LOOP_BREAK(nCount, 100)
		}

		m_data.Clear();
	}
	else
	{
		Erase();
	}
}

////////////////////////////////////////////////////////////////
template<TYPENAME T>
void CAutoLink<T>::Insert(const AUTOLINK_NOTE* pPrev_, void* pOwner)		// insert to next
{
	AUTOLINK_NOTE* pPrev = const_cast<AUTOLINK_NOTE*>(pPrev_);

	CHECK(pPrev);
	CHECK(pPrev != &m_data);
	CHECK(pOwner);
	CHECK(!IsMaster());
	Erase();

	m_data.pOwner				= pOwner;
	if(IsValid())
	{
		m_data.pPrev			= pPrev;
		m_data.pNext			= pPrev->pNext;
		if(pPrev->pNext)
			pPrev->pNext->pPrev	= &m_data;
		pPrev->pNext			= &m_data;
	}
}

////////////////////////////////////////////////////////////////
template<TYPENAME T>
void CAutoLink<T>::Erase()						// erase from list
{
	if(IsValid())
	{
		CHECK(!IsMaster());

		m_data.pPrev->pNext	= m_data.pNext;
		if(m_data.pNext)
			m_data.pNext->pPrev	= m_data.pPrev;

		m_data.Clear();
	}
}


#endif // AUTO_LINK_H
