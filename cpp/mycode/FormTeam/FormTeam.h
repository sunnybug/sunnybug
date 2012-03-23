// FormTeam.h: interface for the CFormTeam class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMTEAM_H__758D0A09_6278_41C3_BC67_EC29E3E3D9E0__INCLUDED_)
#define AFX_FORMTEAM_H__758D0A09_6278_41C3_BC67_EC29E3E3D9E0__INCLUDED_

#pragma once

#ifdef _NOGAME
#include "GameWord.h"
#include "TimeOut.h"
#endif // _NOGAME

#include "FormTeamMember.h"
#include <AutoPtr.h>
#include <GameObjSet.h>

class CFormTeam  
{
public:
	enum FormTeamStaus
	{
		FORMTEAM_NONE,
		FORMTEAM_MARCH,			//行军
		FORMTEAM_SORTFORM,		//整队
		FORMTEAM_PASSBLOCK,		//过障碍
		FORMTEAM_IDLE,			//编队不移动
	};
	typedef CGameObjSet<CFormTeamMember> CFormTeamMemberSet;	//按照矩阵顺序进行排序
	typedef std::list<int> DirList;

public:
	CFormTeam();
	~CFormTeam();

	//////////////////////////////////////////////////////////////////////////
	//编队矩阵
public:
	int		GetDir()						{ return m_nDir;}
	int		GetOldDir()						{ return m_nOldDir;}
	void	SetDir(int nDir);
	const MatrixIndex& GetPathFinderIdx()	{ return m_idxPathFinder;}

protected:
	//以idxRotateCenter为轴心旋转nDegree°
	//ptRotateCenter	[in]	轴心的世界坐标
// 	bool	Rotate(OBJPOS& ptRotate, 
// 		const MatrixIndex& idxRotate, 
// 		const MatrixIndex& idxRotateCenter, 
// 		const OBJPOS& ptRotateCenter, 
// 		const int nDegree);
	bool	RotateDir(OBJPOS& ptRotate, 
		const MatrixIndex& idxRotate, 
		const MatrixIndex& idxRotateCenter, 
		const OBJPOS& ptRotateCenter, 
		const int nDir);
	int		GetMatrixWidth() { return m_szMatrix.cx;}
	//矩阵局部坐标->地图坐标
	bool	CalcMatrixPos(OBJPOS& posWorld, const MatrixIndex& idxMatrix);	

	//////////////////////////////////////////////////////////////////////////
	//生成编队
public:
	bool	Create();
	void	OnTimer();

	//////////////////////////////////////////////////////////////////////////
	//成员管理
public:
	CFormTeamMemberSet* QueryMemberSet() { return m_pTeamMembersSet;}

protected:
	bool	AddMember(CFormTeamMember* pMember);
	CFormTeamMember*	QueryMember(const MatrixIndex& idxMatrix);
	//编队中走的最慢的成员速度，为编队速度
	bool	CalcSpeed();

	//////////////////////////////////////////////////////////////////////////
	//状态
protected:
	FormTeamStaus	GetStatus() { return m_eStatus;}
	bool	SetStatus(FormTeamStaus eStatus);

	//不同状态处理
	bool	ProcessMarch();		//FORMTEAM_MARCH
	bool	ProcessSortForm();	//FORMTEAM_SORTFORM
	bool	ProcessPassBlock();
	bool	ProcessIdle();
	
	//////////////////////////////////////////////////////////////////////////
	//编队寻路
public:
	bool	SetTarget(const OBJPOS& ptTarget);	//设置编队前进目标点

	OBJPOS	GetFinderPos();
	OBJPOS	GetMatrix00Pos();
	bool	GetTmpTarget(OBJPOS& pt);

protected:
	//根据目标点相对于编队的位置，从离目标最近的格子开始寻路
	bool	SelectPathFinderPos(const OBJPOS& ptTarget);
	CFormTeamMember*	QueryPathFinderMember();	//可能有怪物在领路格，也可能没有

	//////////////////////////////////////////////////////////////////////////
protected:
	OBJSIZE			m_szMatrix;
	int				m_nDir;				//矩阵第一列的向量方向
	int				m_nOldDir;

	//////////////////////////////////////////////////////////////////////////
	//临时细节寻路
	MatrixIndex		m_idxPathFinder;	//寻路成员
	OBJPOS			m_ptTmpTarget;		//寻路成员所要到达的目标点
	OBJPOS			m_ptPosFinder;		//寻路成员当前坐标
	DirList			m_lstDir;			//寻路成员的临时路径

	//////////////////////////////////////////////////////////////////////////
	//最终目的寻路
	OBJPOS			m_ptFianlTarget;	//编队最终目标点
	DirList			m_lstFinalDir;		//寻路成员的最终路径

	//////////////////////////////////////////////////////////////////////////
	//成员
	CAutoPtr<CFormTeamMemberSet>	m_pTeamMembersSet;

	FormTeamStaus	m_eStatus;
	CTimeOutMS		m_MoveTimer;		//多少ms走1格

	//////////////////////////////////////////////////////////////////////////
	//link
public:
	const AUTOLINK_NOTE* QueryLink() const	{ return m_link.QueryNote(); }
protected:
	CAutoLink<CFormTeam>	m_link;
};

#endif // !defined(AFX_FORMTEAM_H__758D0A09_6278_41C3_BC67_EC29E3E3D9E0__INCLUDED_)
