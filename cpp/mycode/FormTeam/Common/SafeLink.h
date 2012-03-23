// 安全链接类
// CAutoPtr和CAutoLink的整合和加强。
// 仙剑修, 2007.3.14 新建
// 仙剑修, 2007.3.20 完成单元测试
// 仙剑修, 2007.3.21 模板参数调整。支持派生类相互赋值，并限制不同基类的不能相互赋值。
////////////////////////////////////////////////////////////////
// 说明:“CSafeLink”可使用赋值的方式关联到其它LINK。当存在两个OWN都指向同一对象时，会将自动删除的功能移给后一个OWNer。[能自动保证只有一个“真的”owner]
//		“OWN_TRUE型的CSafeLink”析购时会自动删除对象(只限于最后一个OWNer)，同时，所有有关的CSafeLink会自动清空(设为NULL)。[能手动强制删除对象]
//		当所有“CSafeLink”断开时(不论其中是否有OWN型对象)，都会自动删除。[类似smartptr，保证能自动回收]
//           所以尽量不要直接使用未封装的生(raw)指针，如果要使用，一定要保证至少有一个CSafeLink还存在。否则生指针有可能指向非法内存。
//		“CSafeLink”随时可赋值为NULL。[CSafeLink<CUser> pUser; pUser = NULL;]
//		“CSafeLink”支持用虚基类实现的接口类。[CSafeLink<CUser, OWN_FALSE, IRole> pRole;]
////////////////////////////////////////////////////////////////
// 使用方法：用CAutoLink(及CAutoLinkSet)代替所有的T*，对于T类型指针，不可再使用T*。具体用法类似使用T*。
// 例子：typedef CSafeLink<CUser> CUserPtr。以后所有使用CUser*的地方都改成CUserPtr。(为了使用方便，栈上的临时变量也可考虑直接用CUser*)
// 注意:“CSafeLink”所指的对象随时可能被其它函数清空，所以每次使用“CSafeLink”时都应当先检查是否为NULL。[类似CAutoLink的功能]
////////////////////////////////////////////////////////////////
// 需要添加的接口：
//		“CSafeLink”所管理的对象必须添加成员对象: SAFELINK_DEFINE		// 虚基类用VIRTUAL_SAFELINK_DEFINE
//		“CSafeLink”所管理的对象必须实现成员函数: void Release();


////////////////////////////////////////////////////////////////
// 实现方(server方)示例：
//	class ITest{					// 基类
//	public:
//		virtual void	Release() =0;
//		VIRTUAL_SAFELINK_DEFINE(ITest)
//	};
//	
//	class CTest : public ITest{		// 派生类，必须public继承。
//	public:
//		void	Release() { delete this; }
//		SAFELINK_DEFINE(ITest)
//	};
//	
//	class CTest2{					// 不相关的类，不能相互赋值
//	public:
//		void	Release() { delete this; }
//		SAFELINK_DEFINE(CTest2)
//	};

///////////////////////////////////////////////////////////////
// 调用方(client方)示例：
//	CSafeLink<CTest, OWN_TRUE , ITest>	pOwner = new CTest;
//	CSafeLink<CTest, OWN_FALSE, ITest>	ptr = pOwner;
//	CSafeLink<ITest, OWN_TRUE>	ptrI = ptr;			// 基类和派生类之间可相互赋值
//	ptrI.BreakOwner();
//	ptrI = ptr;
//	ptrI = new CTest;
//	ptr = (CTest*)(ITest*)ptrI;
//	ptrI = pOwner;
//	//error		CSafeLink<CTest2, OWN_FALSE, CTest2> ptrX = ptr;		// error : 不同类型的CSafeLink不能相互赋值
//	ptr = NULL;
//	ptrI = NULL;

////////////////////////////////////////////////////////////////
// 高级说明：(可不看)
// 从OWNER型容器POP出对象时，要调用BreakOwner()主动放弃自己的拥有权限。(当然，就算不放弃，另一个OWN对象拥有它时会自动断开上一对象的OWN权限)
// 所保护的对象的“全局指针”或者“成员变量指针”，必须使用CSafePtr进行封装，否则无法保护此类对象。
// 栈上的“临时变量指针”可以是普通的生(raw)指针(CUser*)，但使用时如果被封装器提前自动删除了对象，会产生“空指针”调用问题，但不会产生“非法指针”。
// 容器请使用CSafeLinkSet、CSafeLinkMap和CSafeLinkArray，也可使用其它stl容器，但容器中的对象必须是CSafeLink封装后的指针对象。
// 注意不能将此功能用在CreateNew之类的创建新对象的函数内部。因为这类函数如果返回生(raw)指针，里面的OWN_FALSE容器会自动将此对象析购掉。除非函数返回CSafeLink类型。
// 请不要将此类和CAutoPtr、CAutoLink、CGameObjSet、CSingleMap之类的混用。可将原来的CAutoPtr改为CSafeLink<T,OWN_TRUE>，将CAutoLink改为CSafeLink<T,OWN_FALSE>，CGameObjSet、CSingleMap改为CSafeSet和CSafeMap。
// 最好不要将全局指针用CSafeLink<T,OWN_TRUE>进行封装，否则全局的内存分配对象g_AllocForSafeLinkNote可能会提前被释放，而导致错误的LOG或其它异常。可以在程序退出之前，主动给全局指针对象赋空(NULL)值来解决这一问题。
// 当派生类的CSafePtr赋值(或提供构造参数)给基类的CSafePtr时，没有直接的赋值函数(或者构造函数)可供调用，会自动调用operator T*()转换成指针，再通过operator=(T* ptr)来赋值(或构造)。效率上会有损失，但能保证不会出现基类指针直接赋值给派生类指针的错误，也能防止不同的CSafePtr<T>之间的混用。

////////////////////////////////////////////////////////////////
// 以下为源码：(可不看)
#ifndef	SAFE_LINK_H
#define	SAFE_LINK_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"
#include "basetype.h"
#include "sgialloc.h"

////////////////////////////////////////////////////////////////
// 程序员不要直接使用此结构。仅在游戏对象的QuerySafeLink()中自动使用。
template<TYPENAME I>
struct SAFELINK_NOTE{
	I*				pTrueObj;				// 指向所封装的原始对象。(T* 类型)
	int				nCount;					// 指向本节点的CSafeLink的数量
	const void*		pMasterLink;			// 拥有者节点的反向指针。(const CSafeLink<T,OWN_TRUE>* 类型)

	void Init(I* ptr=NULL, int num=1)		{ pTrueObj=ptr; nCount=num; pMasterLink=NULL; }
	void Inc()								{ CHECK(pTrueObj); nCount++; }
	void Dec()								{ CHECK(!IsCorrupt()); nCount--; }
	bool IsCorrupt()						{ return nCount < 1; }
};

// 程序员使用这个SAFELINK_DEFINE放在.H文件的class定义中
// I是基类的类型(无基类的为类名T)
#define	SAFELINK_DEFINE(I)	\
public:	\
	SAFELINK_NOTE<I>* QuerySafeLink() const	{ return m_pSafeLinkHostNote; }	\
	void InitSafeLink(SAFELINK_NOTE<I>* ptr) { m_pSafeLinkHostNote=ptr; }	\
protected:	\
	auto_point<SAFELINK_NOTE<I> > m_pSafeLinkHostNote;		// 由CSafePtr赋初值，生存期内不会清空。

// 程序员使用这个VIRTUAL_SAFELINK_DEFINE放在虚基类的.H文件的class定义中
// I是基类的类型
#define VIRTUAL_SAFELINK_DEFINE(I)	\
public:	\
	virtual SAFELINK_NOTE<I>* QuerySafeLink() const	PURE_VIRTUAL_FUNCTION_0	\
	virtual void InitSafeLink(SAFELINK_NOTE<I>* ptr) PURE_VIRTUAL_FUNCTION

enum OWN_ENUM { OWN_FALSE=0, OWN_TRUE=1 };

////////////////////////////////////////////////////////////////
template<TYPENAME T, OWN_ENUM OWN=OWN_FALSE, TYPENAME I=T>
class	CSafeLink
{
public: // cter & dter & assignment
	CSafeLink()										{ m_pData=NULL; }
	CSafeLink(const CSafeLink<T,OWN_FALSE,I>& link)	{ m_pData=NULL; if(link._pData()) Insert(link._pData()); }
	CSafeLink(const CSafeLink<T,OWN_TRUE,I>& link)	{ m_pData=NULL; if(link._pData()) Insert(link._pData()); }
	~CSafeLink()									{ Break(); }
	CSafeLink(T* ptr)								{ m_pData=NULL; if(ptr) Insert(ptr); }
	CSafeLink&	operator=(const CSafeLink<T,OWN_FALSE,I>& link)	{ if(link._pData()==m_pData) return *this; Break(); if(link._pData()) Insert(link._pData()); return *this; }
	CSafeLink&	operator=(const CSafeLink<T,OWN_TRUE,I>& link)	{ if(link._pData()==m_pData) return *this; Break(); if(link._pData()) Insert(link._pData()); return *this; }
	CSafeLink&	operator=(T* ptr)					{ if(ptr && ptr->QuerySafeLink()==m_pData) return *this; Break(); if(ptr) Insert(ptr); return *this; }

public: // const for client call
	operator T*() const								{ return m_pData ? (T*)m_pData->pTrueObj : NULL; }
	POINT_NO_RELEASE<T>* operator->() const			{ MYASSERT(IsValidPt()); return static_cast<POINT_NO_RELEASE<T>*>((T*)m_pData->pTrueObj); }

	///////////////////////////////////////////////////////////////////////////////////////////
	// 外部高级接口。一般情况下请勿使用。
public:
	// 断开自己的OWN权限，相当于Pop出对象，自己变成OWN_FALSE对象。仅限于真的OWNer(Master)调用。
	void	BreakOwner()							{ CHECK(m_pData && m_pData->pMasterLink == this); m_pData->pMasterLink = NULL; }
	// 强制删除所封装的对象。(非MASTER也能调用)
	void	ReleaseObj()							{ if(IsValidPt()) SAFE_RELEASE(reinterpret_cast<T*&>(m_pData->pTrueObj)); }	// 强行删除，不管是不是Owner。
	// 是不是MASTER
	bool	IsMaster()		const					{ return OWN && IsValidPt() && reinterpret_cast<const CSafeLink<T,OWN,I>*>(m_pData->pMasterLink) == this; }	// 最后一个OWNer是主人
	// 取指针
	T*		GetPtr()		const					{ return m_pData ? ((T*)m_pData->pTrueObj) : NULL; }
	// 指针是否为空
	bool	IsValidPt()		const					{ return m_pData && m_pData->pTrueObj; }

	///////////////////////////////////////////////////////////////////////////////////////////
	// 以下为内部接口及数据
protected:
	void	NewNote(I* pTrueObj);
	void	DelNoteAndObj();
	void	Insert(I* pTrueObj);					// insert to next, only for !IsMaster
	void	Insert(SAFELINK_NOTE<I>* pNote)			{ if(pNote && pNote->pTrueObj) Insert(reinterpret_cast<T*>(pNote->pTrueObj)); }
	void	Erase();								// erase from list, only for !IsMaster
	void	Break();								// clear this note. Erase() for !IsMaster, erase all note for IsMaster

protected:
	SAFELINK_NOTE<I>*		m_pData;	// 生存期比pTrueObj长，pTrueObj可能提前清空。生成期比所有的相关CSafeLink都长，保证指针不会失效。

public: // 内部高级接口，请勿使用。仅用于不同模板参数的CSafeLink之间调用。
	SAFELINK_NOTE<I>* _pData() const				{ return m_pData; }
};

////////////////////////////////////////////////////////////////
template<TYPENAME T, OWN_ENUM OWN, TYPENAME I>
void CSafeLink<T,OWN,I>::Break()			// 断开此对象和旧的NOTE之间的链接，准备接收新数据。
{
	if(IsMaster())		// master
	{
		SAFE_RELEASE(reinterpret_cast<T*&>(m_pData->pTrueObj));
		m_pData->pMasterLink=NULL;
	}

	Erase();
}

////////////////////////////////////////////////////////////////
template<TYPENAME T, OWN_ENUM OWN, TYPENAME I>
void CSafeLink<T,OWN,I>::NewNote(I* pTrueObj)		// 新建一个NOTE，并初始化TrueObj内部节点。可重复调用。
{
	CHECK(!m_pData);
	CHECK(pTrueObj);
	CHECK(!pTrueObj->QuerySafeLink());		// 没有NOTE

	extern CSgiAlloc g_AllocForSafeLinkNote;	// only for SAFELINK_NOTE<I>
	CHECK(m_pData = (SAFELINK_NOTE<I>*)g_AllocForSafeLinkNote.Alloc(sizeof(SAFELINK_NOTE<I>)));
	m_pData->Init(pTrueObj, 1);

	pTrueObj->InitSafeLink(m_pData);		// 同时初始化TrueObj内部的节点指针
}

////////////////////////////////////////////////////////////////
template<TYPENAME T, OWN_ENUM OWN, TYPENAME I>
void CSafeLink<T,OWN,I>::DelNoteAndObj()						// erase note obj
{
	IF_OK(m_pData->pMasterLink == NULL)		// 我是最后一个，肯定没有主人了
	{
		SAFE_RELEASE(reinterpret_cast<T*&>(m_pData->pTrueObj));	// 当没有MASTER时，自动删除对象。类似smart_point。
	}

	extern CSgiAlloc g_AllocForSafeLinkNote;	// only for SAFELINK_NOTE<I>
	g_AllocForSafeLinkNote.Free(m_pData, sizeof(SAFELINK_NOTE<I>));
	m_pData=NULL;
}

////////////////////////////////////////////////////////////////
template<TYPENAME T, OWN_ENUM OWN, TYPENAME I>
void CSafeLink<T,OWN,I>::Insert(I* pTrueObj)		// insert to reference list
{
	if(!pTrueObj->QuerySafeLink())
		NewNote(pTrueObj);
	else
	{
		SAFELINK_NOTE<I>* pData = pTrueObj->QuerySafeLink();
		CHECK(!m_pData);
		CHECK(pData);
		m_pData		= pData;
		m_pData->Inc();
	}
	CHECK(m_pData);

	if(OWN)
		m_pData->pMasterLink	= this;
}

////////////////////////////////////////////////////////////////
template<TYPENAME T, OWN_ENUM OWN, TYPENAME I>
void CSafeLink<T,OWN,I>::Erase()						// erase from reference list
{
	if(m_pData)
	{
		CHECK(!m_pData->IsCorrupt());

		m_pData->Dec();
		if(m_pData->IsCorrupt())	// 没有指针指向对象了，可以释放对象了。
			DelNoteAndObj();

		m_pData = NULL;		// 不再指向NOTE
	}
}

#endif // SAFE_LINK_H
