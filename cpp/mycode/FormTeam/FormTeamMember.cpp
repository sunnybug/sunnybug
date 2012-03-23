// FormTeamMember.cpp: implementation of the CFormTeamMember class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FormTeamMember.h"
#include "FormTeam.h"
#include <math.h>

#ifdef _NOGAME
#include "pathfind.h"
#endif // _NOGAME

CFormTeamMember* CFormTeamMember::CreateNew()
{
	CFormTeamMember* pNpc = new CFormTeamMember;
	pNpc->m_pNpc = new CNpc;
	
	static OBJID id = 0;
	pNpc->m_pNpc->m_id = ++id;
	
	return pNpc;
}

CFormTeamMember::CFormTeamMember()
:m_pTeam(NULL)
{
	m_link.Init(this);
}

CFormTeamMember::~CFormTeamMember()
{

}

bool CFormTeamMember::SetMatrixIndex(const MatrixIndex& val)
{
	m_idxMatrix = val;
	return true;
}

bool CFormTeamMember::Create( CFormTeam* pFormTeam )
{
	CHECKF(pFormTeam);
	m_pTeam = pFormTeam;
	return true;
}

OBJID CFormTeamMember::GetID()
{
	CHECKF(m_pNpc);
	return m_pNpc->GetID();
}

bool CFormTeamMember::Move2Target()
{
	CHECKF(m_pNpc);

	if (GetTargetPos() == OBJPOS_NONE)
		return false;
	if (GetTargetPos() == m_pNpc->GetPos())
		return false;

	PathMove();

	return true;
}

void CFormTeamMember::FindPath( const OBJPOS& ptTarget )
{
	CHECK(m_pNpc);
	if(ptTarget == m_pNpc->GetPos())
		return;

#ifdef _NOGAME
	CHECK(CPathFinder::FindPath(m_lstDir, m_pNpc->GetPos(), ptTarget));
#else
#pragma error("Ѱ·")
#endif // _NOGAME
}

bool CFormTeamMember::PathMove()
{
	CHECKF(m_pNpc);
	if(m_lstDir.empty())
		return false;
	
	const int nDir = m_lstDir.front();
	m_lstDir.pop_front();
	
	IF_NOT(MoveForward(nDir))
		return false;
		
	return true;
}

bool CFormTeamMember::MoveForward( int nDir )
{
	CHECKF(m_pNpc);

	OBJPOS ptNew = m_pNpc->GetPos();
	ptNew.x += CalcDeltaX(nDir);
	ptNew.y += CalcDeltaY(nDir);
	m_pNpc->SetPos(ptNew.x, ptNew.y, true);
	return true;
}
