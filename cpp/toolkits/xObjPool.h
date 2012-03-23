#ifndef _XOBJPOOL__
#define _XOBJPOOL__

#include <boost/pool/object_pool.hpp>

template<class T, class TQ_LOCK>
class xObjPool : protected boost::object_pool<T>
{
public:
	xObjPool();
	~xObjPool();

	T*		Alloc();
	void	Free(T* p);
	bool	IsValidObj(T* p);
};

template<class T, class TQ_LOCK>
T* xObjPool<T, TQ_LOCK>::Alloc()
{
	return construct();
}

template<class T, class TQ_LOCK>
void xObjPool<T, TQ_LOCK>::Free( T* p )
{
	if(!p)
		return;

	p->~T();
	store().free(p);
}

template<class T, class TQ_LOCK>
bool xObjPool<T, TQ_LOCK>::IsValidObj( T* p )
{
	return is_from(p);
}


#endif