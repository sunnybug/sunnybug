// FormTeamMember.h: interface for the CFormTeamMember class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMTEAMMEMBER_H__6DAB4E40_68EE_43E6_A384_657B61537018__INCLUDED_)
#define AFX_FORMTEAMMEMBER_H__6DAB4E40_68EE_43E6_A384_657B61537018__INCLUDED_

#pragma once

class CFormTeam;

class CFormTeamMember
{
public:
	static CFormTeamMember* CreateNew();
	bool	Create(CFormTeam* pFormTeam);
	void	Release() { delete this;}
protected:
	CFormTeamMember();
	~CFormTeamMember();

	//////////////////////////////////////////////////////////////////////////
	//编队矩阵
public:
	bool	SetMatrixIndex(const MatrixIndex& val);
	inline const MatrixIndex& GetMatrixIndex()		{return m_idxMatrix;}

	bool	SetTargetPos(const OBJPOS& ptTarget)	{ ASSERT(ptTarget.x>=0 && ptTarget.y>=0); m_ptTarget = ptTarget; return true;}
	inline const OBJPOS&	GetTargetPos()			{ return m_ptTarget;}

	bool	Move2Target();
	bool	MoveForward(int nDir);
	void	FindPath(const OBJPOS& ptTarget);

protected:
	bool	PathMove();

	//////////////////////////////////////////////////////////////////////////
	//属性
public:
	OBJID	GetID();
	CNpc*	QueryNpc() { return m_pNpc;}

protected:
	MatrixIndex				m_idxMatrix;	//在编队中的位置
	OBJPOS					m_ptTarget;		//
	DirList					m_lstDir;

	CAutoLink<CFormTeam>	m_pTeam;
	CAutoLink<CNpc>			m_pNpc;

	//////////////////////////////////////////////////////////////////////////
	//link
public:
	const AUTOLINK_NOTE* QueryLink() const	{ return m_link.QueryNote(); }
protected:
	CAutoLink<CFormTeamMember>	m_link;
};

#endif // !defined(AFX_FORMTEAMMEMBER_H__6DAB4E40_68EE_43E6_A384_657B61537018__INCLUDED_)
