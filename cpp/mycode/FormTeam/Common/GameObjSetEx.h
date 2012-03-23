// GameObjSet.h: interface for the CGameObjSetEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEOBJSETEX_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
#define AFX_GAMEOBJSETEX_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// 说明：该模板需要GetID()、Release()和GetUniKey(int idxKey)函数支持，同时也需要定义DWORD和ULONG
//       GetUniKey(int idxKey): idxKey是索引的INDEX值，按AddIndex()的顺序从0开始计数。

#include "Index.h"
#include "GameObjSet.h"
#include <vector>
using namespace std;

template<class T>
class CGameObjSetEx : public IGameObjSet<T>
{
protected:
	typedef	vector<T*>	VECTOR_SET;
protected:
	CGameObjSetEx(bool bOwner=true) { m_bOwner = bOwner; }
	virtual ~CGameObjSetEx();
public:
	static CGameObjSetEx<T>*	CreateNew(bool bOwner=true) { return new CGameObjSetEx<T>(bOwner); }		//? default is owner set
	void		Release			()				{ delete this; }

//////////////////////////////////////////////////////
public:	// new traversal
	typedef	SetIterator<T>	Iterator;
	/*class	Iterator{
	private: // create by parent class
		Iterator(VECTOR_SET* pSet) : m_pSet(pSet),m_idx(pSet->size()-1),m_pObj(0) {}
		friend class CGameObjSet<T>;
	public:
//		Iterator(const Iterator& iter);		// need not define this ^_^

	public: // application
		bool	Next()		{ if(m_idx<0) return m_pObj=NULL,false; ASSERT(m_pObj=m_pSet->at(m_idx)); m_idx--; return true; }
		void	Reset()		{ m_idx=m_pSet->size()-1; m_pObj=NULL; }
		operator T*()							{ return m_pObj; }
		T * operator->()						{ return m_pObj; }

	protected:
		const VECTOR_SET*	m_pSet;
		int			m_idx;
		T*			m_pObj;
	};*/
public: // traverse
	Iterator	NewEnum		() 						{ return Iterator(&m_setObj); }
	int			Size		() const				{ return m_setObj.size(); }
//////////////////////////////////////////////////////

public:	// old traversal
	int			GetAmount		() const		{ return m_setObj.size(); }
	T*			GetObjByIndex	(int idx)		{ CHECKF(idx>=0 && idx<m_setObj.size()); return m_setObj[idx]; }

public: // application
	bool		AddObj			(T* pObj);
	bool		DelObj			(DWORD idObj);
	bool		DelObj			(Iterator& iter);
	T*			GetObj			(DWORD idObj);
	T*			PopObj			(DWORD idObj);
	T*			PopObj			(Iterator& iter);
	void		Clear			();
	T*			GetUniqueObj	(int key, int idx=0)	{ CHECKF(idx>=0 && idx<m_setIndex.size()); return m_setIndex[idx][key]; }

public: // for index. obj set mast empty
	typedef	CIndexEx<int, T>		INDEX_TYPE;
	bool		SetIndex(int nCount=1)		{ CHECKF(m_setObj.size()==0); m_setIndex.resize(nCount); return true; }

protected:
	VECTOR_SET			m_setObj;
	bool				m_bOwner;
	vector<INDEX_TYPE>	m_setIndex;
};

//////////////////////////////////////////////////////////////////////
template<class T>
CGameObjSetEx<T>::~CGameObjSetEx()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
template<class T>
void CGameObjSetEx<T>::Clear()
{
	if(m_bOwner)
	{
		for(int i=0;i<m_setObj.size();i++)
		{ 
			T* pObj=m_setObj[i];
			if(pObj)
				pObj->Release();
		}
	}
	m_setObj.clear();

	for(int j = 0; j < m_setIndex.size(); j++)
	{
		INDEX_TYPE* pIndex = &m_setIndex[j];
		pIndex->Clear();
	}
}

//////////////////////////////////////////////////////////////////////
template<class T>
bool CGameObjSetEx<T>::AddObj(T* pObj)
{
	CHECKF(pObj && pObj->GetID());
	ASSERT(!DelObj(pObj->GetID()));

	m_setObj.push_back(pObj);

	for(int j = 0; j < m_setIndex.size(); j++)
	{
		INDEX_TYPE* pIndex = &m_setIndex[j];
		pIndex->Add(pObj->GetUniKey(j), pObj);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
template<class T>
bool CGameObjSetEx<T>::DelObj(Iterator& iter)
{
	T* pObj = PopObj(iter);
	if(pObj)
	{
		if(m_bOwner)
			pObj->Release();
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
template<class T>
T* CGameObjSetEx<T>::PopObj(Iterator& iter)
{
	if(iter.m_pObj)
	{
		T* pObj = iter.m_pObj;
		iter.m_pObj	= NULL;

		int	idx = iter.m_idx+1;
		IF_OK(idx < m_setObj.size())
		{
			for(int j = 0; j < m_setIndex.size(); j++)
			{
				INDEX_TYPE* pIndex = &m_setIndex[j];
				pIndex->Del(pObj->GetUniKey(j));
			}

			m_setObj.erase(m_setObj.begin() + idx);
			return pObj;
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
template<class T>
bool CGameObjSetEx<T>::DelObj(DWORD idObj)
{
	CHECKF(idObj);
	T* pObj=PopObj(idObj);
	if(pObj)
	{
		if(m_bOwner)
			pObj->Release();
		return true; 
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
template<class T>
T* CGameObjSetEx<T>::GetObj(DWORD idObj)
{
	for(int i=0;i<m_setObj.size();i++)
	{
		T* pObj=m_setObj[i];
		if(pObj && pObj->GetID()==idObj)
		{
			return pObj;
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
template<class T>
T* CGameObjSetEx<T>::PopObj(DWORD idObj)
{
	ASSERT(m_bOwner);

	for(int i=0;i<m_setObj.size();i++)
	{
		T* pObj=m_setObj[i];
		if(pObj && pObj->GetID()==idObj)
		{
			for(int j = 0; j < m_setIndex.size(); j++)
			{
				INDEX_TYPE* pIndex = &m_setIndex[j];
				pIndex->Del(pObj->GetUniKey(j));
			}

			m_setObj.erase(m_setObj.begin()+i);
			return pObj;
		}
	}
	return NULL;
}



#endif // !defined(AFX_GAMEOBJSETEX_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
