// SafeLinkSet.h: interface for the CSafeLinkSet class.
// paled, 2007-3-21 新建
// paled, 2007-3-28 完成单元测试
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOLINKSET_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
#define AFX_AUTOLINKSET_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////
// 说明:
//    “SafeLinkSet”可直接用Iterator遍历，其中的空对象和“OWN_TRUE类型SET中的非MASTER对象指针”指针会被自动清除。
//     多重循环中的删除对象，不会导致迭代子非法。[此功能通过延迟删除实现]
//     PopObj弹出对象时，内部会临时存有CSafeLink(OWN_FALSE)指针对象。不会出现自动删除此对象的隐患。
//     ★注意★ Size()函数可能会遍历所有对象，效率不高。请调用时不要太频繁。
////////////////////////////////////////////////////////////////
// 需要实现的接口:
//     该模板需要GetID()和Release()函数支持，同时也需要定义DWORD

////////////////////////////////////////////////////////////////
// 使用示例：


////////////////////////////////////////////////////////////////
// 高级说明：(可不用看)
// 因为PopObj之后，外面可能有临时的生(raw)指针使用对象。如果这个SET也是临时的，并提前释放了，则那个临时的生指针会非法。另外，(最外层Iterator外的)Size()调用也会导致清除空指针。
// 当一个对象从一个OWN_SET移到另一个OWN_SET后，如果前一个没有显式执行PopObj，则Master权限会自动转移。但如果转移后这个对象又被删除了，这时对于可能出现的隐含逻辑问题，此模板没有提供断言。
// 因为无效指针的清空时机在新建最外层Iterator，或最外层Iterator外的Size()调用的时候，故有时内层的延迟删除对象，有可能导致不能及时释放内存，造成内存占用偏多。(此问题估计影响不大)
// 生指针向外层函数传递时，可能会因为内层删除了最后一个SafeLink(或强制删除了MASTER)，导致传递出去的生指针为非法地址。返回参数应当改成CSafePtr封装类型。
// DelObj时，OWN_TRUE型内部会自动立即删除对象。导致外部的生(raw)指针指向非法内存。(看是否可优化成放一临时集中，延迟删除。但什么时机删除合适呢?)
// 如果在被封装对象的Release调用中可清空自己的NOTE中的对象指针，则更安全。即使外部的生指针调用了Release，也不会导致封装器的非法内存操作。

#include "SafeLink.h"
#include <vector>
using namespace std;

////////////////////////////////////////////////////////////////
// 该接口仅用于Iterator内部，程序员请勿使用。
class IExpendForInterator
{
public:
	virtual int		CalcSize(bool bNeedCount=true) = 0;
	virtual long&	GetIteratorCount() = 0;
	virtual void	SetUpdateFlag() = 0;
};

template<class T, OWN_ENUM OWN>
class CSafeLinkSet : public IExpendForInterator
{
protected:
	typedef	CSafeLink<T,OWN>	SAFE_PTR_TYPE;
	typedef	CSafeLink<T,OWN_FALSE>	SAFE_PTR;
	typedef	vector<SAFE_PTR_TYPE>	VECTOR_SET;
public:
	CSafeLinkSet() {}
	virtual ~CSafeLinkSet();
public:
	static CSafeLinkSet<T,OWN>*	CreateNew()		{ return new CSafeLinkSet<T,OWN>(); }
	void	Release			()					{ delete this; }

public: // 提供给程序员的迭代器
	class	Iterator;
	Iterator	NewEnum		() 					{ return Iterator(&m_setObj, this); }
	int			Size		()					{ return CalcSize(); }

public: // 提供给程序员的接口
//	bool		AddObj			(const SAFE_PTR_TYPE& pObj);
	bool		AddObj			(T* ptr);
	bool		DelObj			(DWORD idObj);
	bool		DelObj			(Iterator& iter);
	T*			GetObj			(DWORD idObj);
	T*			PopObj			(DWORD idObj);
	T*			PopObj			(Iterator& iter);
	void		Clear			()				{ m_setObj.clear(); }

//////////////////////////////////////////////////////
// 内部成员
//////////////////////////////////////////////////////
protected:
	int			CalcSize(bool bNeedCount=true);		// return size of set only include valid obj.
	long&		GetIteratorCount()				{ return m_nCountIterator; }
	void		SetUpdateFlag()					{ m_bEraseFlag = true; }
protected:
	VECTOR_SET			m_setObj;
	auto_bool			m_bEraseFlag;		// true: 内部有无效指针
	auto_long			m_nCountIterator;	// 外面已经有多少个Iterator了？
	vector<SAFE_PTR>		m_setRecycle;		// 临时存一下循环中删除的对象，便于外部生(raw)指针使用。
public:	// new traversal
	class	Iterator{
	private: // create by parent class
		Iterator(VECTOR_SET* pSet, IExpendForInterator* pData) : m_pSet(pSet),m_idx(pSet->size()),m_pData(pData)
			{ m_pData->CalcSize(false); m_pData->GetIteratorCount()++; }
		friend class CSafeLinkSet<T,OWN>;
		Iterator& operator=(const Iterator&);	// invalid func! please cannot use this function. no function body
	public:
		Iterator(const Iterator& iter) : m_pSet(iter.m_pSet),m_idx(iter.m_idx),m_pData(iter.m_pData)	{ m_pData->GetIteratorCount()++; }
		~Iterator()								{ m_pData->GetIteratorCount()--; }

	public: // application
		bool	Next()
		{
			if(m_idx<=0)
				return false; 

			bool bFound = false;
			do
			{
				m_idx--;

				SAFE_PTR_TYPE& pObj = m_pSet->at(m_idx);
				if(OWN && pObj)		// 空指针，或者非MASTER指针，会自动清除。
				{
					IF_NOT(pObj.IsMaster())					// 如果断言错，表示没有从OWN_TRUE型SET中PopObj，就将对象放到另一OWN_TRUE容器中了。导致此容器失去了OWN权限。
					{
						pObj = NULL;						// m_setObj中的指针清空。OWN_TRUE型会删除OBJ
						m_pData->SetUpdateFlag();
					}
				}

				if(!pObj.IsValidPt())
				{
					m_pData->SetUpdateFlag();
					//不需要	m_pSet->at(m_idx).BreakOwner();		// 仅变为OWN_FALSE，不删除，以利外部生(raw)指针使用。
					//m_pSet->erase(m_pSet->begin() + m_idx);			//??? client方在使用多重循环时删除对象(或自动维护)，可能会有问题!
				}
				else
					bFound = true;
			}
			while(m_idx>0 && !bFound);

			return bFound;
		}

		void	Reset()							{ m_idx=m_pSet->size(); }
		operator T*()							{ CHECKF(m_idx>=0&&m_idx<m_pSet->size()); return m_pSet->at(m_idx); }
		POINT_NO_RELEASE<T>* operator->()		{ CHECKF(m_idx>=0&&m_idx<m_pSet->size()); return static_cast<POINT_NO_RELEASE<T>*>((T*)m_pSet->at(m_idx)); }

	protected:
		VECTOR_SET*		m_pSet;
		int				m_idx;		// 改为当前迭代子所指位置(以前为下一位置)
//		SAFE_PTR		m_pObj;		// OWN_FALSE型的CSafePtr
		IExpendForInterator*	m_pData;
	};
};

//////////////////////////////////////////////////////////////////////
template<class T, OWN_ENUM OWN>
CSafeLinkSet<T,OWN>::~CSafeLinkSet()
{
	// 无需代码，CSafeLink会完成一切。

	// more log
//	if(OWN)
//	{
//		for(int i=0;i<m_setObj.size();i++)
//			MYASSERT(m_setObj.at(i).IsMaster());		// more log for auto change owner.
//	}
}

/*/////////////////////////////////////////////////////////////////////
template<class T, OWN_ENUM OWN>
bool CSafeLinkSet<T,OWN>::AddObj(const SAFE_PTR_TYPE& pObj)
{
	CHECKF(pObj && pObj->GetID());
	ASSERT(!DelObj(pObj->GetID()));

	m_setObj.push_back(pObj);
	return true;
}*/

//////////////////////////////////////////////////////////////////////
template<class T, OWN_ENUM OWN>
bool CSafeLinkSet<T,OWN>::AddObj(T* ptr)
{
	CHECKF(ptr && ptr->GetID());
	ASSERT(!DelObj(ptr->GetID()));

	SAFE_PTR_TYPE	link(ptr);
	m_setObj.push_back(link);
	return true;
}

//////////////////////////////////////////////////////////////////////
template<class T, OWN_ENUM OWN>
bool CSafeLinkSet<T,OWN>::DelObj(Iterator& iter)
{
	int idx = iter.m_idx;
	CHECKF(idx>=0 && idx < m_setObj.size());

	if(m_setObj[idx])
	{
		m_setObj[idx] = NULL;						// m_setObj中的指针清空。OWN_TRUE型会删除OBJ
		SetUpdateFlag();
		//m_setObj.erase(m_setObj.begin() + idx);

		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
template<class T, OWN_ENUM OWN>
bool CSafeLinkSet<T,OWN>::DelObj(DWORD idObj)
{
	CHECKF(idObj);
	for(int idx=0;idx<m_setObj.size();idx++)
	{ 
		T* pObj=m_setObj[idx];
		if(pObj && pObj->GetID()==idObj)
		{
			m_setObj[idx] = NULL;						// m_setObj中的指针清空。OWN_TRUE型会删除OBJ
			SetUpdateFlag();
			//m_setObj.erase(m_setObj.begin()+idx);
			return true; 
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
template<class T, OWN_ENUM OWN>
T* CSafeLinkSet<T,OWN>::GetObj(DWORD idObj)
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
template<class T, OWN_ENUM OWN>
T* CSafeLinkSet<T,OWN>::PopObj(DWORD idObj)
{
	for(int idx=0;idx<m_setObj.size();idx++)
	{
		T* pObj=m_setObj[idx];
		if(pObj && pObj->GetID()==idObj)
		{
			MYASSERT(OWN);	// more check, 只有OWN_TRUE才可调用。(可删除)
			m_setObj[idx].BreakOwner();
			m_setRecycle.push_back(pObj);			// 暂时保持，便于外部临时生(raw)指针使用。
			m_setObj[idx] = NULL;
			SetUpdateFlag();
			//m_setObj.erase(m_setObj.begin() + idx);
			return pObj;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
template<class T, OWN_ENUM OWN>
T* CSafeLinkSet<T,OWN>::PopObj(Iterator& iter)
{
	int idx = iter.m_idx;
	CHECKF(idx>=0 && idx < m_setObj.size());

	if(m_setObj[idx])
	{
		MYASSERT(OWN);	// more check, 只有OWN_TRUE才可调用。(可删除)
		MYASSERT(!OWN || OWN && m_setObj[idx].IsMaster());		// 只能POP一次

		T* pObj = m_setObj[idx];
		m_setObj[idx].BreakOwner();
		m_setRecycle.push_back(pObj);			// 暂时保持，便于外部临时生(raw)指针使用。
		m_setObj[idx] = NULL;
		SetUpdateFlag();
		//m_setObj.erase(m_setObj.begin() + idx);
		return pObj;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
template<class T, OWN_ENUM OWN>
int CSafeLinkSet<T,OWN>::CalcSize(bool bNeedCount)
{
	if(!m_bEraseFlag || m_nCountIterator>0 && !bNeedCount)
		return m_setObj.size();

	bool bErase = m_bEraseFlag && m_nCountIterator==0;

	if(bErase)
		m_setRecycle.clear();

	int nCount = 0;		// only for !bErase
	for(int idx=m_setObj.size()-1; idx>=0; idx--)		// 删除，倒序
	{
		if(m_setObj[idx].IsValidPt())
			nCount++;		// only for !bErase
		else
		{
			if(bErase)
				m_setObj.erase(m_setObj.begin()+idx);
		}
	}

	if(bErase)
	{
		m_bEraseFlag = false;
		return m_setObj.size();
	}
	else
		return nCount;		// only for !bErase
}



#endif // !defined(AFX_AUTOLINKSET_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
