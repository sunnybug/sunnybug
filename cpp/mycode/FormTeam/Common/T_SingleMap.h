// 索引类，用于建立数据表之间的快速查询。加入的KEY不能重复
// 仙剑修，2001.12.18
//////////////////////////////////////////////////////////////////////
// 说明：该模板需要GetID()和Release()函数支持，同时也需要定义DWORD和ULONG

#pragma warning(disable:4786)
#ifndef	TSINGLEMAP_H
#define TSINGLEMAP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Define.h"
#include "basefunc.h"
#include "logfile.h"


// USE_HASH_MAP 使用SGISTL的HASH_MAP来实现，以加速GetObj()之类的操作。
#ifdef	USE_HASH_MAP
#include <hash_map>
#define	base_class_map	hash_map
#else // USE_HASH_MAP
#include <map>
#define	base_class_map	map
#endif // USE_HASH_MAP
using namespace std;
enum {DEL_FALSE=0, DEL_TRUE, DEL_CATCH};

template<class T>
class CSingleMap;

template<class T>
class	MapIterator{
protected:		
	typedef	base_class_map<DWORD, T*>		MAP_SET;
	typename typedef	MAP_SET::iterator	MAP_ITER;
private: // create by parent class
	MapIterator(MAP_SET* pSet) : m_pSet(pSet),m_iter(pSet->empty() ? pSet->end() : pSet->begin()),m_pObj(0) {}
	friend class CSingleMap<T>;
public:
	//		Iterator(const Iterator& iter);		// need not define this ^_^
	
public: // application
	bool	Next()		{ if(m_iter==m_pSet->end()) return m_pObj=NULL,false; ASSERT(m_pObj=m_iter->second); m_iter++; return true; }
	void	Reset()		{ m_iter=m_pSet->begin(); m_pObj=NULL; }
	operator T*()							{ return m_pObj; }
	T * operator->()						{ return m_pObj; }
	
protected:
	MAP_SET*	m_pSet;
	MAP_ITER	m_iter;
	T*			m_pObj;
};

template<TYPENAME T>
class ISingleMap
{
protected:
	virtual ~ISingleMap() {}
public:
	typedef	MapIterator<T>	Iterator;
public:
	typedef	base_class_map<DWORD, T*> MAPINDEX;	
	typename typedef	MAPINDEX::iterator	Iter;
	virtual void		Release()							PURE_VIRTUAL_FUNCTION
	virtual Iter		Begin() 							PURE_VIRTUAL_FUNCTION_X
	virtual Iter		End()								PURE_VIRTUAL_FUNCTION_X
	virtual T*			GetObjByIter(Iter i)				PURE_VIRTUAL_FUNCTION_0
	virtual bool		AddObj(T* pObj)						PURE_VIRTUAL_FUNCTION_0
	virtual int		DelObj(DWORD key)						PURE_VIRTUAL_FUNCTION_0
	virtual T*			GetObj(DWORD key)						PURE_VIRTUAL_FUNCTION_0
	virtual int			GetAmount()							PURE_VIRTUAL_FUNCTION_0
	virtual MapIterator<T>	NewEnum()						PURE_VIRTUAL_FUNCTION_X
//	virtual DWORD			Key(iterator Iter)					PURE_VIRTUAL_FUNCTION_0
//	virtual T*			PopObj(DWORD idObj, int idType)		PURE_VIRTUAL_FUNCTION_0		// 用于从OWNER_SET中取出对象，不删除。
};

template	< TYPENAME T >
class CSingleMap : public ISingleMap<T>
{
protected:
	typedef	base_class_map<DWORD, T*>		MAP_SET;
	typename typedef	MAP_SET::iterator	MAP_ITER;

public:
	typedef	MapIterator<T>	Iterator;
	/*class	Iterator{
	private: // create by parent class
		Iterator(MAP_SET* pSet) : m_pSet(pSet),m_iter(pSet->begin()),m_pObj(0) {}
		friend class CSingleMap<T>;
	public:
//		Iterator(const Iterator& iter);		// need not define this ^_^

	public: // application
		bool	Next()		{ if(m_iter==m_pSet->end()) return m_pObj=NULL,false; ASSERT(m_pObj=m_iter->second); m_iter++; return true; }
		void	Reset()		{ m_iter=m_pSet->begin(); m_pObj=NULL; }
		operator T*()							{ return m_pObj; }
		T * operator->()						{ return m_pObj; }

	protected:
		MAP_SET*	m_pSet;
		MAP_ITER	m_iter;
		T*			m_pObj;
	};*/
public: // traverse
	Iterator	NewEnum		() 						{ return Iterator(&m_map); }
	int			Size		() const				{ return m_map.size(); }

//////////////////////////////////////////////////////
protected:
	CSingleMap(bool bOwner=true) { m_bOwner = bOwner; }
	virtual ~CSingleMap();
public:
	static CSingleMap<T>*	CreateNew(bool bOwner=true) { return new CSingleMap<T>(bOwner); }
public:
//	typedef	base_class_map<DWORD, T*> MAPINDEX;	typedef	MAPINDEX::iterator	Iter;
	void	Release() { delete this; }
	Iter	Begin() 										{ return m_map.begin(); }
	Iter	End()											{ return m_map.end(); }
	T*		GetObjByIter(Iter i)							{ return i->second; }
	bool	AddObj(T* pObj);
	int	DelObj(DWORD key);
	int	DelObj(Iterator& iter);
	T*		PopObj(Iterator& iter);
	T*		PopObj(DWORD key);
	T*		GetObj(DWORD key);
	int		GetAmount()										{ return m_map.size(); }
///	DWORD		Key(iterator Iter)								{ return Iter->first; }
//	bool	IsIn(DWORD key);
protected:
	void		Clear			();
protected:
	bool		m_bOwner;
	MAPINDEX	m_map;
};

template	< TYPENAME T >
CSingleMap<T>::~CSingleMap()
{
	Clear();
}

template	< TYPENAME T >
void CSingleMap<T>::Clear()
{
	if(m_bOwner && m_map.size())
	{
		for(Iter i = Begin(); i != End(); i++)
		{
			GetObjByIter(i)->Release();
			i->second = 0;		// 加强安全性
		}
	}
	m_map.clear();
}

template	< TYPENAME T >
bool	CSingleMap<T>::AddObj(T* pObj)
{
	CHECKF(pObj && pObj->GetID());

	if(m_bOwner)
	{
		ASSERT(!DelObj(pObj->GetID()));
	}
	m_map[pObj->GetID()] = pObj;
	return true;
}

template	< TYPENAME T >
int	CSingleMap<T>::DelObj(Iterator& iter)
{
	int iRet=DEL_FALSE;
	if(iter.m_pObj)
	{
		T* pObj = PopObj(iter);
		if(pObj)iRet=DEL_TRUE;

		try{
			if(m_bOwner && pObj)
				pObj->Release();
		}catch(...)
		{ 
			ASSERT(!"DelObj(Iterator& iter)"); 
			iRet=DEL_CATCH;
		}
		return iRet;//pObj != NULL;
	}
	return iRet;
}

template	< TYPENAME T >
T*	CSingleMap<T>::PopObj(Iterator& iter)
{
	if(iter.m_pObj)
	{
		T* pObj = iter.m_pObj;
		iter.m_pObj	= NULL;

		IF_OK(iter.m_iter != m_map.begin())
		{
#ifdef	USE_HASH_MAP
			m_map.erase(pObj->GetID());
#else // USE_HASH_MAP
			MAP_ITER	temp = iter.m_iter;
			temp--;
			m_map.erase(temp);
#endif // USE_HASH_MAP
			return pObj;
		}
	}
	return NULL;
}

template	< TYPENAME T >
T*	CSingleMap<T>::PopObj(DWORD key)
{
	if(m_bOwner)
	{
		int iRet=DEL_FALSE;
		Iter i = m_map.find(key);
		if(i != m_map.end())
		{
			T* pObj = i->second;
			m_map.erase(i);
			return pObj;
		}
		return NULL;
	}
	return NULL;
}

template	< TYPENAME T >
int	CSingleMap<T>::DelObj(DWORD key)
{
	if(m_bOwner)
	{
		int iRet=DEL_FALSE;
		Iter i = m_map.find(key);
		if(i != m_map.end())
		{
			iRet=DEL_TRUE;
			try{
				GetObjByIter(i)->Release();
			}
			catch(...)
			{ 
				ASSERT(!"DelObj(Iterator& iter)"); 
				iRet=DEL_CATCH;
			}
			m_map.erase(i);
			return iRet;
		}
		return iRet;
	}
	else
		return m_map.erase(key) != 0;
}

template	< TYPENAME T >
T*		CSingleMap<T>::GetObj(DWORD key)
{
	Iter i;
	if((i=m_map.find(key)) != m_map.end())
		return i->second;

	return NULL;
}

/*template	< TYPENAME T >
bool	CSingleMap<T>::IsIn(DWORD key)
{
	return (m_map.find(key) != m_map.end());
}*/


#endif // TSINGLEMAP_H

