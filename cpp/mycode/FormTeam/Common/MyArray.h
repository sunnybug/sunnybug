// MyArray.h: interface for the CMyArray class.
// paled, 2004.6.4
// 数组封装类：
// 用于封装C++的数组类型，可在边界溢出时输出LOG。
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYARRAY_H__04538BAC_0EF7_46EB_AAF7_9201677B5D10__INCLUDED_)
#define AFX_MYARRAY_H__04538BAC_0EF7_46EB_AAF7_9201677B5D10__INCLUDED_

#pragma warning( disable : 4786 ) 
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "vector"
#include "common.h"

//////////////////////////////////////////////////////////////////////
// 示例：
// 只需要修改H文件中定义部分，其它完全不变。
// 原定义: int									m_setPuzzleCells[PUZZLE_CELLS_NUM];
// 可改成: CMyArray<int,PUZZLE_CELLS_NUM>		m_setPuzzleCells;
//////////////////////////////////////////////////////////////////////

// T: 数组的元素类型
// SIZE: 数组的尺寸
// HINT: 如果定义了许多不同类型的数组，最好填入此数，用于输出LOG时区分是哪个类型的数组有问题。也可不填。
template<class T, int SIZE, int HINT=0>
class CMyArray
{
public:
	CMyArray()	{ MYASSERT(SIZE); memset(m_set, 0, SIZE*sizeof(T));}
	CMyArray(const T* buf, int nSize)	{ MYASSERT(SIZE); IF_OK(nSize && nSize>=SIZE) memcpy(m_set, buf, SIZE*sizeof(T)); }
//不支持重载	virtual ~CMyArray() {}

public: // 重载数组操作符
	template<class E>
	T& operator [] (E nPos)	{ IF_OK(nPos>=0 && nPos<SIZE) return m_set[nPos]; LogSave("★MyArray(?,%d,%d)★",SIZE,HINT); throw; }
	template<class E>
	const T& operator [] (E nPos) const	{ IF_OK(nPos>=0 && nPos<SIZE) return m_set[nPos]; LogSave("★MyArray(?,%d,%d)★",SIZE,HINT); throw; }
public: // 兼容接口
	operator char* ()			{ return (char*)m_set; }
	operator T*		()			{ return (T*)m_set; }
	template<class E>
	T& GetObj(E nPos)	{ IF_OK(nPos>=0 && nPos<SIZE) return m_set[nPos]; LogSave("★MyArray(?,%d,%d)★",SIZE,HINT); throw; }
protected:
	T	m_set[SIZE];
};

// 二维数组
#define	MY_ARRAY2(T,CX,CY,H)		CMyArray<CMyArray<T, CY, (H)+1>, CX, H>

// 三维数组
#define	MY_ARRAY3(T,CX,CY,CZ,H)		CMyArray<MY_ARRAY2(T, CY,CZ, (H)+1), CX, H>

////////////////////////////////////////////////////////////////////////////////////
// 用数组指针初始化的CMyArray，
// 1、外面的数组不能提前删除(!!!)
// 2、尺寸必须和原数组完全相同(!!!)
template<class T, int SIZE, int HINT=0>
class CMyArrayPtr
{
public:
	CMyArrayPtr(T* buf) : m_set(0)	{ MYASSERT(SIZE && buf); m_set = buf; }
	CMyArrayPtr(T* buf, int nSize) : m_set(0)	{ MYASSERT(SIZE && buf); IF_OK(nSize && nSize>=SIZE) m_set = buf; }
//不支持重载	virtual ~CMyArrayPtr() {}

public: // 重载数组操作符
	template<class E>
	T& operator [] (E nPos)	{ IF_OK(nPos>=0 && nPos<SIZE) return m_set[nPos]; LogSave("★MyArrayPtr(?,%d,%d)★",SIZE,HINT); throw; }
	template<class E>
	const T& operator [] (E nPos) const	{ IF_OK(nPos>=0 && nPos<SIZE) return m_set[nPos]; LogSave("★MyArrayPtr(?,%d,%d)★",SIZE,HINT); throw; }
public: // 兼容接口
	operator char* ()			{ return (char*)m_set; }
	operator T*		()			{ return (T*)m_set; }
protected:
	T*	m_set;
};

#endif // !defined(AFX_MYARRAY_H__04538BAC_0EF7_46EB_AAF7_9201677B5D10__INCLUDED_)
