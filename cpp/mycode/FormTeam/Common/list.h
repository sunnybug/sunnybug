#ifndef _LIST_H_
#define _LIST_H_


#define LIST_ENTRY(ptr, type, member) \
	((type *) ((char *) (ptr) - (unsigned long) (&((type *) 0)->member)))

#define LIST_FOR_EACH(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)


class CListHead {
public:
	CListHead() {
		prev = next = this;
	}

	bool isEmpty() {
		return (next == this);
	}

	CListHead *removeHead(bool bHead=true);
	void add(CListHead *item);
	void addHead(CListHead *item);
	void remove();

	CListHead *prev, *next;
};


#endif
