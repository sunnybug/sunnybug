#ifndef _XOBJPOOLEX__
#define _XOBJPOOLEX__

#include <vector>

template<class T, class TQ_LOCK>
class xObjPoolEx
{
public:
	xObjPoolEx();
	~xObjPoolEx();

	T*		Alloc();
	void	Free(T* p);
	bool	IsValidObj(T* p);

protected:
	typedef std::vector<T*>
};

template<class T, class TQ_LOCK>
T* xObjPoolEx<T, TQ_LOCK>::Alloc()
{
	return construct();
}

template<class T, class TQ_LOCK>
void xObjPoolEx<T, TQ_LOCK>::Free( T* p )
{
	if(!p)
		return;

	p->~T();
	store().free(p);
}

template<class T, class TQ_LOCK>
bool xObjPoolEx<T, TQ_LOCK>::IsValidObj( T* p )
{
	return is_from(p);
}


#endif