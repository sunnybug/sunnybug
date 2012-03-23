// MyClassFactory.h: declare for the CMyClassFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYCLASSFACTORY_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
#define AFX_MYCLASSFACTORY_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_

#pragma warning(disable:4786)
#include "common.h"
#include "basetype.h"
#include "mycom.h"
#pragma warning(disable:4786)
#include <map>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IMyClassFactory
{
public:
	virtual bool CreateNewObj(void** ppObj)				PURE_VIRTUAL_FUNCTION_0
	virtual void ReleaseObj(void** ppObj)				PURE_VIRTUAL_FUNCTION
};

namespace myclassfactory{
	bool CreateNewObj(CLS_ID idCls, void** ppObj);
	void ReleaseObj(CLS_ID idCls, void** ppObj);
	void RegisterMyClass(CLS_ID idCls, IMyClassFactory* pFactory);

	typedef	std::map<CLS_ID, IMyClassFactory*>	FACTORY_SET;
	inline FACTORY_SET* QueryFactorySet()		{ extern FACTORY_SET* x_psetFactory; return x_psetFactory; }	// 每个DLL都会返回自己的静态SET
	void AddFactorySet(FACTORY_SET* pSet);	// 合并SET，用于SHELL的类厂整合其它DLL的类厂。(当ID重复时，内部会报断言)
};

template<class T>
class CMyClassFactory : public IMyClassFactory
{
	virtual bool CreateNewObj(void** ppObj)
	{
		CHECKF(ppObj);
		*ppObj = new T;
		return true;
	}
	virtual void ReleaseObj(void** ppObj)
	{
		CHECK(ppObj);
		if(*ppObj)
			delete ( (T*) (*ppObj) );
		*ppObj = NULL;
	}
};

template<class T>
class CMyClassFactoryCom : public IMyClassFactory
{
	virtual bool CreateNewObj(void** ppObj)
	{
		CHECKF(ppObj);
		*ppObj = T::CreateNew();
		return true;
	}
	virtual void ReleaseObj(void** ppObj)
	{
		CHECK(ppObj);
		if(*ppObj)
			( (T*) (*ppObj) )->Release();
		*ppObj = NULL;
	}
};

namespace myclassfactory{
	struct auto_register_factory{
		auto_register_factory(CLS_ID idCls, IMyClassFactory* pFactory){
			myclassfactory::RegisterMyClass(idCls, pFactory);
		};
	};
};

//////////////////////////////////////////////////////////////////////
// 用于外部定义和注册类厂，要写在CPP中。
#define	REGISTER_FACTORY(ID, PTR)	namespace myclassfactory{ myclassfactory::auto_register_factory x_objAuto##ID((ID), (PTR)); };

#define	DEFINE_FACTORY(T,ID)		namespace myclassfactory{ CMyClassFactory<T> x_pFactory##T; };	\
									REGISTER_FACTORY(ID, static_cast<IMyClassFactory*>(&x_pFactory##T))

#define	DEFINE_FACTORY_COM(T,ID)	namespace myclassfactory{ CMyClassFactoryCom<T> x_pFactory##T; };	\
									REGISTER_FACTORY(ID, static_cast<IMyClassFactory*>(&x_pFactory##T))

#endif // !defined(AFX_MYCLASSFACTORY_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
