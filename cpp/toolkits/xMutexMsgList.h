#ifndef	_XMUTEXTMSGLIST_H_
#define _XMUTEXTMSGLIST_H_

//最简单的消息队列，比较性能时使用

#include <exception>
#include <windows.h>
#include <list>
#include <xCriticalSection.h>
#include <xGuard.h>


template< class T >
class xMutexMsgList
{
public:	
	// default constructor
	xMutexMsgList()
	{
	}
	~xMutexMsgList()
	{
	}
	void enqueue(const T& t)
	{
		T* pMsg = new T(t);
		xGuard<xCriticalSection> g(m_cs);
		m_lst.push_back(pMsg);
	}

	bool dequeue(T& t)
	{
		T* pMsg = NULL;
		{
			xGuard<xCriticalSection> g(m_cs);
			if(m_lst.empty())
				return false;

			pMsg = m_lst.front();
			m_lst.pop_front();
		}
		t = *pMsg;
		delete pMsg;
		return true;
	}

private:
	typedef std::list<T*>	MsgList;
private:
	xCriticalSection m_cs;
	MsgList m_lst;
};

#endif
