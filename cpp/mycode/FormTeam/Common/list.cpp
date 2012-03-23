#include "list.h"

void CListHead::remove()  //从列表中去掉本身（当前结点）
{
    //if(next!=this)
	{
	    prev->next = next;
	    next->prev = prev;
	    prev = next = this;
	}
}

CListHead *CListHead::removeHead(bool bHead)  //从列表中去掉（当前结点）下一个
{
	CListHead *t =next;
	if(next!=this||bHead)
	{
	    next->remove();
	}
	else return 0;
	return t;
}

void CListHead::add(CListHead *item)   //插在this前
{
	item->prev = prev;
	item->next = this;
	prev->next = item;
	prev = item;
}

void CListHead::addHead(CListHead *item)  //插在this后
{
	item->prev = this;
	item->next = next;
	next->prev = item;
	next = item;
}
