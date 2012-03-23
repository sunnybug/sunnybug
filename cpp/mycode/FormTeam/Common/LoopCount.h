// LoopCount.h: interface for the CLoopCount class.
// 可循环输出连续整数的类。
// Init()	: 可指定头、尾、起点和总数。头尾不能相同，但可从小到大也可从大到小。起点可不在范围内，但意义不确定。可重复调用。
// Next()	: 返回当前数，内部状态移到下一个数。可连续输出整数，增减根据头尾来确定。如果指定了总数，则调用次数不能超过该值。
// IsValid(): 检测是否超过了总数。
// Last()	: 前一个状态。(上一次调用Next()返回的值)
// GetCount(): 返回取出的次数。即调用Next()的次数。
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOOPCOUNT_H__B2E9D39C_2685_4AF5_8C78_78AF1F82A6C8__INCLUDED_)
#define AFX_LOOPCOUNT_H__B2E9D39C_2685_4AF5_8C78_78AF1F82A6C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLoopCount  
{
public:
	CLoopCount();
	/*virtual*/ ~CLoopCount();
	bool Init(int nFrom, int nTo, int nStart, int nAllCount=0);

public:
	int		Next();
	void	SetPos(int nPos);
public: // const
	int		Last()		const		{ return m_nLast; }
	bool	IsValid()	const		{ return m_bValid; }
	int		GetFirst()	const		{ return m_nStart; }
	int		GetCount()	const		{ return m_nCount; }

protected:
	int		m_nFrom;
	int		m_nTo;
	int		m_nStart;
	int		m_nCurr;
	int		m_nCount;
	int		m_nLimit;
	bool	m_bValid;
	int		m_nLast;
};

#endif // !defined(AFX_LOOPCOUNT_H__B2E9D39C_2685_4AF5_8C78_78AF1F82A6C8__INCLUDED_)
