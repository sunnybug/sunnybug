// 索引类，用于建立数据表之间的快速查询。加入的KEY不能重复
// 仙剑修，2001.12.18
// paled, 2003.8.7 modify
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)
#ifndef	TEMPLATE_INDEX_H
#define TEMPLATE_INDEX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Define.h"
#include "baseType.h"
#include "basefunc.h"

// USE_HASH_MAP 使用SGISTL的HASH_MAP来实现，以加速GetObj()之类的操作。
#ifdef	USE_HASH_MAP
#include <hash_map>
#define	base_class_map	hash_map
#else // USE_HASH_MAP
#define	base_class_map	map
#endif // USE_HASH_MAP
#include <map>
using namespace std;

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////

template	<TYPENAME K, TYPENAME D>
class Index
{
public:
	Index() {}
	virtual ~Index() {}
public:
	bool		Add(K key, D data)					{ m_map[key] = data; return true; }
	bool		Del(K key)							{ m_map.erase(key); return true; }
	bool		IsIn(K key)	const					{ if(m_map.find(key) !=m_map.end()) return true; return false; }
	const D&	At(K key)	const					{ return operator[](key); }
	D&			operator[](K key);
	const D&	operator[](K key) const;
	void		Clear() { m_map.clear(); }
	typedef	map<typename K,typename D> MAPINDEX;
	typename typedef	MAPINDEX::iterator	Iterator;
	typename typedef	MAPINDEX::const_iterator	ConstIterator;
	const K&	Key(Iterator iter)	const			{ return iter->first; }
	const D&	Data(Iterator iter)	const			{ return iter->second; }
	Iterator	Begin()								{ return m_map.begin(); }
	Iterator	End()								{ return m_map.end(); }
	DWORD		Size()								{ return m_map.size();}
public: // dump
	String		Dump()	const						{ String str="{ ("; 
														MAPINDEX::const_iterator iter=m_map.begin(); 
														for(; iter!=m_map.end(); iter++){ 
															if(iter!=m_map.begin()) str+="), (";
															str+=::DumpString(iter->first);
															str+=",";
															str+=::DumpString(iter->second); } 
														return str+=") }"; }
protected:
	MAPINDEX	m_map;
};

template	<TYPENAME K, TYPENAME D>
D&	Index<K, D>::operator[](K key)
{
	Iterator iter	= m_map.find(key);
	ASSERT(iter !=m_map.end());
	return iter->second;
}

template	<TYPENAME K, TYPENAME D>
const D&	Index<K, D>::operator[](K key) const
{
	ConstIterator iter	= m_map.find(key);
	ASSERT(iter !=m_map.end());
	return iter->second;
}


template	<TYPENAME K, TYPENAME D>
class CIndexEx
{
public:
	CIndexEx() {}
	virtual ~CIndexEx() {}
public:
	bool		Add(K key, D* data)					{ m_map[key] = data; return true; }
	bool		Del(K key)							{ m_map.erase(key); return true; }
	bool		IsIn(K key)	const					{ if(m_map.find(key) !=m_map.end()) return true; return false; }
	const D*	At(K key)	const					{ return operator[](key); }
	D*			operator[](K key);
	const D*	operator[](K key) const;
	void		Clear() { m_map.clear(); }
	typedef	base_class_map<K, D*> MAPINDEX;
	typename typedef	MAPINDEX::iterator	Iterator;
	typename typedef	MAPINDEX::const_iterator	ConstIterator;
	const K&	Key(Iterator iter)	const			{ return iter->first; }
	const D*	Data(Iterator iter)	const			{ return iter->second; }
	Iterator	Begin()								{ return m_map.begin(); }
	Iterator	End()								{ return m_map.end(); }
	DWORD		Size()								{ return m_map.size();}
public: // dump
	String		Dump()	const						{ String str="{ ("; 
														MAPINDEX::const_iterator iter=m_map.begin(); 
														for(; iter!=m_map.end(); iter++){ 
															if(iter!=m_map.begin()) str+="), (";
															str+=::DumpString(iter->first);
															str+=",";
															str+=::DumpString(iter->second); } 
														return str+=") }"; }
protected:
	MAPINDEX	m_map;
};

template	<TYPENAME K, TYPENAME D>
D*	CIndexEx<K, D>::operator[](K key)
{
	Iterator iter	= m_map.find(key);
	if(iter !=m_map.end())
		return iter->second;
	else
		return NULL;
}

template	<TYPENAME K, TYPENAME D>
const D*	CIndexEx<K, D>::operator[](K key) const
{
	ConstIterator iter	= m_map.find(key);
	if(iter !=m_map.end())
		return iter->second;
	else
		return NULL;
}


#endif // TEMPLATE_INDEX_H
