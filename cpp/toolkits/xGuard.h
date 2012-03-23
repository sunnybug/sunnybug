#ifndef _XGUARD_H_
#define _XGUARD_H_
#pragma once

template <class LOCK>
class xGuard
{
public:
	xGuard(LOCK& l):m_l(l){ m_l.Lock(); }
	~xGuard() { m_l.Unlock(); }
protected:
	xGuard(const xGuard &);
	void operator=(const xGuard &);
protected:
	LOCK& m_l;
};


#endif