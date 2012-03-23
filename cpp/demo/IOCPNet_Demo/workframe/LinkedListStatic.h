////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		LinkedList.h
//	Author:
//		Chun-Hyok, Chong.
//	Description:
//		It is a class that implements a linked list uses static memory area.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __LINKEDLISTSTATIC_H__
#define __LINKEDLISTSTATIC_H__

#include "PreAllocator.h"
#include <windows.h>

#define RET_SUCCESS							1
#define RET_FAIL							-1
#define RET_NO_MATCH						2
#define RET_EQUAL							0
#define RET_NOT_EQUAL						1
#define RET_NO_ELEMENT						0

#ifndef __OELEMENT__
#define __OELEMENT__
template<class T> class OElement
{
public:
	T Element;
	OElement<T> *pPrev;
	OElement<T> *pNext;
};
#endif // __OELEMENT__

template<class T> class OLinkedListStatic
{
	// Properties.
private:
	OElement<T> *m_pHead;
	OElement<T> *m_pTail;
	OElement<T> *m_pCurrent;
	DWORD m_Count;
	OPreAllocator *m_pPreAllocator;

public:

	// Methods.
private:
	int Compare(BYTE *Compee, BYTE *Comper, DWORD Size);

public:
	OLinkedListStatic();
	OLinkedListStatic(OPreAllocator *pPA);
	BOOL SetPreAllocator(OPreAllocator *pPA);
	~OLinkedListStatic();
	int Add(T Element);
	int AddBeforeCurrent(T Element);
	int AddAfterCurrent(T Element);
	int Subtract(T Element);
	int SubtractAllDup(T Element);
	int SubtractFirst();
	int SubtractLast();
	int SubtractCurrent();
	T *MoveFirst();
	T *MoveLast();
	T *MovePrev();
	T *MoveNext();
	T *GetCurrent();
	DWORD GetCount();
	int Reset();
};

template<class T> OLinkedListStatic<T>::OLinkedListStatic()
{
	m_pHead = NULL;
	m_pTail = NULL;
	m_pCurrent = NULL;
	m_Count = 0;
	m_pPreAllocator = 0;
} // OLinkedListStatic()

template<class T> OLinkedListStatic<T>::OLinkedListStatic(OPreAllocator *pPA)
{
	m_pHead = NULL;
	m_pTail = NULL;
	m_pCurrent = NULL;
	m_Count = 0;
	m_pPreAllocator = pPA;
} // OLinkedListStatic()
	
template<class T> OLinkedListStatic<T>::~OLinkedListStatic()
{
	OElement<T> *pNext;
	OElement<T> *pNextTemp;

	if (NULL != m_pHead)
	{
		pNext = m_pHead;
		while (NULL != pNext)
		{
			pNextTemp = pNext->pNext;
			m_pPreAllocator->Free(pNext);
			pNext = pNextTemp;
		}
	}
} // ~OLinkecListStatic()

template<class T> BOOL OLinkedListStatic<T>::SetPreAllocator(OPreAllocator *pPA)
{
	if (0 != m_pPreAllocator)
	{
		m_pPreAllocator->FreePreAllocatedMemory();
		m_pPreAllocator = 0;
	}

	m_pPreAllocator = pPA;

	return 1;
} // SetPreAllocator()

template<class T> int OLinkedListStatic<T>::Add(T Element)
{
	OElement<T> *pElement;
	OElement<T> *pNext;

	pElement = (OElement<T> *)m_pPreAllocator->Allocate(sizeof (OElement<T>));
	if (NULL == pElement)
	{
		return RET_FAIL;
	}

	if (NULL == m_pHead) // The first element.
	{
		pElement->pPrev = NULL;
		pElement->pNext = NULL;
		memcpy(&(pElement->Element), &Element, sizeof (Element));
		m_pCurrent = m_pHead = m_pTail = pElement;
	}
	else
	{
		pNext = m_pTail;
		pNext->pNext = pElement;
		pElement->pPrev = pNext;
		pElement->pNext = NULL;
		m_pTail = pElement;
		memcpy(&pElement->Element, &Element, sizeof (Element));
	}

	m_Count++;

	return RET_SUCCESS;
} // Add()

template<class T> int OLinkedListStatic<T>::AddBeforeCurrent(T Element)
{
	OElement<T> *pElement;

	if (NULL == m_pCurrent)
	{
		return RET_FAIL;
	}

	pElement = (OElement<T> *)m_pPreAllocator->Allocate(sizeof (OElement<T>));
	if (NULL == pElement)
	{
		return RET_FAIL;
	}

	if (NULL == m_pCurrent->pPrev)
	{
		pElement->pPrev = NULL;
		m_pHead = pElement;
	}
	else
	{
		m_pCurrent->pPrev->pNext = pElement;
		pElement->pPrev = m_pCurrent->pPrev;
	}

	pElement->pNext = m_pCurrent;
	m_pCurrent->pPrev = pElement;
	memcpy(&pElement->Element, &Element, sizeof (Element));

	m_Count++;

	return RET_SUCCESS;
} // AddBeforeCurrent()

template<class T> int OLinkedListStatic<T>::AddAfterCurrent(T Element)
{
	OElement<T> *pElement;

	if (NULL == m_pCurrent)
	{
		return RET_FAIL;
	}

	pElement = (OElement<T> *)m_pPreAllocator->Allocate(sizeof (OElement<T>));
	if (NULL == pElement)
	{
		return RET_FAIL;
	}

	if (NULL == m_pCurrent->pNext)
	{
		pElement->pNext = NULL;
		m_pTail = pElement;
	}
	else
	{
		m_pCurrent->pNext->pPrev = pElement;
		pElement->pNext = m_pCurrent->pNext;
	}

	pElement->pPrev = m_pCurrent;
	m_pCurrent->pNext = pElement;
	memcpy(&pElement->Element, &Element, sizeof (Element));

	m_Count++;

	return RET_SUCCESS;
} // AddAfterCurrent()

template<class T> int OLinkedListStatic<T>::Subtract(T Element)
{
	OElement<T> *pNext;
	int RetCode;

	if (NULL == m_pHead)
	{
		return RET_NO_ELEMENT;
	}

	RetCode = RET_NO_MATCH;
	pNext = m_pHead;
	while (NULL != pNext)
	{
		if (RET_EQUAL == Compare((BYTE *)&pNext->Element, (BYTE *)&Element, sizeof (T)))
		{
			if (NULL == pNext->pNext)
			{
				if (NULL == pNext->pPrev)
				{
					// There will be no element in the list.
					m_pHead = NULL;
					m_pTail = NULL;
					m_pCurrent = NULL;
					// m_Count = 0; see below 'm_Count--'.
				}
				else
				{
					pNext->pPrev->pNext = NULL;
					m_pTail = pNext->pPrev;
					if (m_pCurrent == pNext)
					{
						m_pCurrent = m_pTail;
					}
				}
			}
			else
			{
				if (NULL == pNext->pPrev)
				{
					m_pHead = pNext->pNext;
					pNext->pNext->pPrev = NULL;
					if (m_pCurrent == pNext)
					{
						m_pCurrent = m_pHead;
					}
				}
				else
				{
					pNext->pNext->pPrev = pNext->pPrev;
					pNext->pPrev->pNext = pNext->pNext;
					if (m_pCurrent == pNext)
					{
						m_pCurrent = pNext->pNext;
					}
				}
			}
			m_Count--;

			if (0 == m_pPreAllocator->Free(pNext))
			{
				return RET_FAIL;
			}
			RetCode = RET_SUCCESS;
			break;
		}
		pNext = pNext->pNext;
	}

	return RetCode;
} // Subtract()

template<class T> int OLinkedListStatic<T>::SubtractAllDup(T Element)
{
	OElement<T> *pNext;
	OElement<T> *pNextTemp;
	int RetCode;

	if (NULL == m_pHead)
	{
		return RET_NO_ELEMENT;
	}

	RetCode = RET_NO_MATCH;
	pNext = m_pHead;
	while (NULL != pNext)
	{
		pNextTemp = pNext->pNext;
		if (RET_EQUAL == Compare((BYTE *)&pNext->Element, (BYTE *)&Element, sizeof (T)))
		{
			if (NULL == pNext->pNext)
			{
				if (NULL == pNext->pPrev)
				{
					// There will be no element in the list.
					m_pHead = NULL;
					m_pTail = NULL;
					m_pCurrent = NULL;
					// m_Count = 0; see below 'm_Count--'.
					pNextTemp = NULL;
				}
				else
				{
					pNext->pPrev->pNext = NULL;
					m_pTail = pNext->pPrev;
					if (m_pCurrent == pNext)
					{
						m_pCurrent = m_pTail;
					}
					pNextTemp = NULL;
				}
			}
			else
			{
				if (NULL == pNext->pPrev)
				{
					m_pHead = pNextTemp = pNext->pNext;
					pNext->pNext->pPrev = NULL;
					if (m_pCurrent == pNext)
					{
						m_pCurrent = m_pHead;
					}
				}
				else
				{
					pNext->pNext->pPrev = pNext->pPrev;
					pNext->pPrev->pNext = pNextTemp = pNext->pNext;
					if (m_pCurrent == pNext)
					{
						m_pCurrent = pNext->pNext;
					}
				}
			}
			m_Count--;

			if (0 == m_pPreAllocator->Free(pNext))
			{
				return RET_FAIL;
			}
			RetCode = RET_SUCCESS;

			if (NULL == pNextTemp)
			{
				break;
			}
		}
		pNext = pNextTemp;
	}

	return RetCode;
} // SubtractAllDup()

template<class T> int OLinkedListStatic<T>::SubtractFirst()
{
	OElement<T> *pNext;

	if (NULL == m_pHead)
	{
		return RET_NO_ELEMENT;
	}

	pNext = m_pHead;
	if (NULL == pNext->pNext)
	{
		m_pHead = NULL;
		m_pTail = NULL;
		m_pCurrent = NULL;
	}
	else
	{
		pNext->pNext->pPrev = NULL;
		m_pHead = pNext->pNext;
		if (m_pCurrent == pNext)
		{
			m_pCurrent = pNext->pNext;
		}
	}
	m_Count--;
	
	if (0 == m_pPreAllocator->Free(pNext))
	{
		return RET_FAIL;
	}

	return RET_SUCCESS;
} // SubtractFirst()

template<class T> int OLinkedListStatic<T>::SubtractCurrent()
{
	OElement<T> *pNext;

	if (NULL == m_pCurrent)
	{
		return RET_NO_ELEMENT;
	}

	pNext = m_pCurrent;
	if (NULL == pNext->pNext)
	{
		if (NULL == pNext->pPrev)
		{
			m_pCurrent = m_pHead = m_pTail = NULL;
		}
		else
		{
			pNext->pPrev->pNext = NULL;
			m_pTail = m_pCurrent = pNext->pPrev;
		}
	}
	else
	{
		if (NULL == pNext->pPrev)
		{
			m_pHead = m_pCurrent = pNext->pNext;
			pNext->pNext->pPrev = NULL;
			if (m_pCurrent == pNext)
			{
				m_pCurrent = m_pHead;
			}
		}
		else
		{
			pNext->pNext->pPrev = pNext->pPrev;
			pNext->pPrev->pNext = pNext->pNext;
			if (m_pCurrent == pNext)
			{
				m_pCurrent = pNext->pNext;
			}
		}
	}
	m_Count--;

	if (0 == m_pPreAllocator->Free(pNext))
	{
		return RET_FAIL;
	}

	return RET_SUCCESS;
} // SubtractCurrent()

template<class T> int OLinkedListStatic<T>::SubtractLast()
{
	OElement<T> *pNext;

	if (NULL == m_pTail)
	{
		return RET_NO_ELEMENT;
	}

	pNext = m_pTail;
	if (NULL == pNext->pPrev)
	{
		m_pHead = NULL;
		m_pTail = NULL;
		m_pCurrent = NULL;
	}
	else
	{
		pNext->pPrev->pNext = NULL;
		m_pTail = pNext->pPrev;
		if (m_pCurrent == pNext)
		{
			m_pCurrent = pNext->pPrev;
		}
	}
	m_Count--;
	
	if (0 == m_pPreAllocator->Free(pNext))
	{
		return RET_FAIL;
	}

	return RET_SUCCESS;
} // SubtractLast()

template<class T> T *OLinkedListStatic<T>::MoveFirst()
{
	if (NULL == m_pHead)
	{
		return NULL;
	}
	
	m_pCurrent = m_pHead;
	
	return &m_pCurrent->Element;
} // MoveFirst()

template<class T> T *OLinkedListStatic<T>::MoveLast()
{
	if (NULL == m_pTail)
	{
		return NULL;
	}
	
	m_pCurrent = m_pTail;

	return &m_pCurrent->Element;
} // MoveLast()

template<class T> T *OLinkedListStatic<T>::MovePrev()
{
	if (NULL == m_pCurrent || NULL == m_pCurrent->pPrev)
	{
		return NULL;
	}
	
	m_pCurrent = m_pCurrent->pPrev;

	return &m_pCurrent->Element;
} // MovePrev()

template<class T> T *OLinkedListStatic<T>::MoveNext()
{
	if (NULL == m_pCurrent || NULL == m_pCurrent->pNext)
	{
		return NULL;
	}

	m_pCurrent = m_pCurrent->pNext;

	return &m_pCurrent->Element;
} // MoveNext()

template<class T> T *OLinkedListStatic<T>::GetCurrent()
{
	if (NULL == m_pCurrent)
	{
		return NULL;
	}

	return &m_pCurrent->Element;
} // GetCurrent()

template<class T> DWORD OLinkedListStatic<T>::GetCount()
{
	return m_Count;
} // GetCount()

template<class T> int OLinkedListStatic<T>::Reset()
{
	OElement<T> *pNext;
	OElement<T> *pNextTemp;

	if (NULL != m_pHead)
	{
		pNext = m_pHead;
		while (NULL != pNext)
		{
			pNextTemp = pNext->pNext;
			if (0 == m_pPreAllocator->Free(pNext))
			{
				return RET_FAIL;
			}
			pNext = pNextTemp;
		}
	}

	m_pHead = NULL;
	m_pTail = NULL;
	m_pCurrent = NULL;
	m_Count = 0;

	return RET_SUCCESS;
} // Reset()

template<class T> int OLinkedListStatic<T>::Compare(BYTE *Compee, BYTE *Comper, DWORD Size)
{
	DWORD LCounter;

	for (LCounter = 0; LCounter < Size; LCounter++)
	{
		if (*(Compee + LCounter) != *(Comper + LCounter))
		{
			return RET_NOT_EQUAL;
		}
	}

	return RET_EQUAL;
} // Compare()

#endif // __LINKEDLISTSTATIC_H__