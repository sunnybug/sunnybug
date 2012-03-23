// MyClassFactory.cpp
//
//////////////////////////////////////////////////////////////////////
// 注意：此类厂是通过全局函数自动注册的，不支持在全局对象的构造函数中调用CreateNewObj

#include <afxwin.h>
#include "MyClassFactory.h"
using namespace myclassfactory;

namespace myclassfactory{
//	typedef	map<CLS_ID, IMyClassFactory*>	FACTORY_SET;
	FACTORY_SET*	x_psetFactory=NULL;		// 内部使用。(因为没有extern声明，每个DLL会自建一个集)
	int				x_nCountObj = 0;
};

bool myclassfactory::CreateNewObj(CLS_ID idCls, void** ppObj)
{
	CHECKF(ppObj);
	CHECKF(x_psetFactory);
	*ppObj = NULL;

	FACTORY_SET::iterator iter=x_psetFactory->find(idCls);
	if(iter != x_psetFactory->end())
	{
		iter->second->CreateNewObj(ppObj);
		x_nCountObj++;
		return *ppObj != NULL;
	}

	MYASSERT(!"invalid idCls in myclassfactory::CreateNewObj.");	// 注意：REGISTER_FACTORY(及相关宏)必须放在有“有效代码”的CPP中。不能只放此类自动宏。
	return false;
}

void myclassfactory::ReleaseObj(CLS_ID idCls, void** ppObj)
{
	CHECK(ppObj);
	if(!*ppObj)
		return;		// 未初始化的对象，也可能用NULL指针调用此函数

	CHECK(x_psetFactory);
	CHECK(x_nCountObj > 0);
	FACTORY_SET::iterator iter=x_psetFactory->find(idCls);
	if(iter != x_psetFactory->end())
	{
		iter->second->ReleaseObj(ppObj);
		x_nCountObj--;
//BUG		if(x_nCountObj <= 0)
//BUG		{
//BUG			// 实现了自动删除集对象，可能有BUG	- paled
//BUG			// 自动删除有可能会频繁删除和创建集对象。
//BUG			delete x_psetFactory;
//BUG			x_psetFactory = NULL;
//BUG		}
		return ;
	}

	MYASSERT(!"invalid idCls in myclassfactory::ReleaseObj.");
}

void myclassfactory::RegisterMyClass(CLS_ID idCls, IMyClassFactory* pFactory)
{
	TRACE("RegisterMyClass - 0x%X\n", idCls);

	if(!x_psetFactory)
		x_psetFactory = new FACTORY_SET;	// 如果不释放，会有固定大小内存泄漏	- paled

	FACTORY_SET::iterator iter=x_psetFactory->find(idCls);
	if(iter != x_psetFactory->end())
	{
		MYASSERT(!"multi register idCls in myclassfactory::RegisterMyClass.");
	}

	(*x_psetFactory)[idCls] = pFactory;
}

void AddFactorySet(FACTORY_SET* pSet)
{
	if(!x_psetFactory)
		x_psetFactory = new FACTORY_SET;	// 如果不释放，会有固定大小内存泄漏	- paled

	CHECK(x_psetFactory != pSet);

	for(FACTORY_SET::iterator iter = pSet->begin(); iter != pSet->end(); iter++)
	{
		CLS_ID idCls = iter->first;
		IMyClassFactory* pFactory = iter->second;

		TRACE("AddFactorySet - 0x%X\n", idCls);

		FACTORY_SET::iterator iter2=x_psetFactory->find(idCls);
		if(iter2 != x_psetFactory->end())
		{
			MYASSERT(!"multi AddFactorySet idCls in myclassfactory::RegisterMyClass.");
		}

		(*x_psetFactory)[idCls] = pFactory;
	}	
}
