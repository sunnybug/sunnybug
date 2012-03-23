// FormTeam.cpp: implementation of the CFormTeam class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FormTeam.h"
#include <math.h>

#ifdef _NOGAME
#include "pathfind.h"
#else
#define TRACE LOG
#endif // _NOGAME

const MatrixIndex MATRIX_00(0, 0);

//////////////////////////////////////////////////////////////////////////
//math
const float	pi	= 3.14159265f;

inline	float	DegreesToRadians(float deg)
{
	return deg * pi / 180.0f;
}

inline	float	RadiansToDegrees(float rad)
{	
	return rad * 180.0f / pi;
}

int manhattan(const OBJPOS& pt1, const OBJPOS& pt2)
{
	return (abs(pt1.x-pt2.x) + abs(pt1.y-pt2.y));
}

// void Convert2Pix( const OBJPOS& cell, Pix& pix )
// {
// 	if (cell.x >= 0)
// 		pix.x = cell.x * MATRIX_WIDTH + MATRIX_WIDTH/2;
// 	else
// 		pix.x = cell.x * MATRIX_WIDTH - MATRIX_WIDTH/2;
// 	
// 	if (cell.y >= 0)
// 		pix.y = cell.y * MATRIX_HEIGHT + MATRIX_HEIGHT/2;
// 	else
// 		pix.y = cell.y * MATRIX_HEIGHT - MATRIX_HEIGHT/2;
// }
// 
// void Convert2Cell( const Pix& pix, OBJPOS& cell )
// {
// 	cell.x = pix.x / MATRIX_WIDTH;
// 	cell.y = pix.y / MATRIX_HEIGHT;
// }

//cos()
int MatrixCos(const int nDegree)
{
	const float ESPLONG = 0.00001;

	const float fCos = cos(DegreesToRadians(nDegree));

	if (nDegree % 90 == 0)
	{
		if(fCos > ESPLONG)
			return 1;
		if (fCos <= ESPLONG && fCos>=-ESPLONG)
			return 0;
		return -1;
	}
	else if(nDegree %45 == 0)	//矩阵的45°旋转需要特殊处理
		return fCos>0 ? 1 : -1;

	ASSERT(false);
	return 0;
}

int MatrixSin(const int nDegree)
{
	const float ESPLONG = 0.00001;
	
	const float fSin = sin(DegreesToRadians(nDegree));
	
	if (nDegree % 90 == 0)
	{
		if(fSin > ESPLONG)
			return 1;
		if (fSin <= ESPLONG && fSin>=-ESPLONG)
			return 0;
		return -1;
	}
	else if(nDegree %45 == 0)	//矩阵的45°旋转需要特殊处理
		return fSin>0 ? 1 : -1;
	
	ASSERT(false);
	return 0;
}

bool CalcMatrixRotate(OBJPOS& ptResult, const OBJPOS& ptRotate, const int nDir)
{
	int nDegree = (180+nDir*45)%360;

	//	繞原點逆時針旋轉 θ 度角的變換公式是 x' = xcosθ- ysinθ, y' = xsinθ + ycosθ
	ptResult.x = ptRotate.x * MatrixCos(nDegree) - ptRotate.y * MatrixSin(nDegree);
	ptResult.y = ptRotate.x * MatrixSin(nDegree) + ptRotate.y * MatrixCos(nDegree);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFormTeam::CFormTeam()
:m_eStatus(FORMTEAM_NONE)
,m_nDir(4)
,m_nOldDir(4)
{
	m_link.Init(this);
	m_pTeamMembersSet = CFormTeamMemberSet::CreateNew();
	ASSERT(m_pTeamMembersSet);
}

CFormTeam::~CFormTeam()
{

}

bool CFormTeam::Create()
{
	CHECKF(CalcSpeed());

#ifdef _NOGAME
	m_szMatrix.cx = 5;
	m_szMatrix.cy = 7;
#else
#pragma error("矩阵大小")
#endif // _DEBUG

	CHECKF(m_szMatrix.cx>0 && m_szMatrix.cy>0);

// 	m_members.resize(m_szMatrix.cx*m_szMatrix.cy);

	//编队0格初始位置
	OBJPOS posCell0(15, 15);
	m_ptPosFinder = posCell0;

	//create member
	for (int y=0; y<m_szMatrix.cy; ++y)
	{
		for (int x=0; x<m_szMatrix.cx; ++x)
		{
			CAutoPtr<CFormTeamMember> pMember = CFormTeamMember::CreateNew();
			IF_NOT(pMember)
				continue;
			IF_NOT(pMember->Create(this))
				continue;
			IF_NOT(pMember->QueryNpc())
				continue;

			pMember->SetMatrixIndex(MatrixIndex(x,y));

			OBJPOS ptMapCell(x,y);
			ptMapCell += posCell0;	//转为地图坐标
			pMember->QueryNpc()->SetPos(ptMapCell.x, ptMapCell.y);

			CFormTeamMember* ptr = pMember.pop();
			IF_NOT(AddMember(ptr))
			{
				SAFE_RELEASE(ptr);
			}
		}
	}

	return true;
}

bool CFormTeam::AddMember(CFormTeamMember* pMember)
{
	CHECKF(pMember);

	IF_NOT(m_pTeamMembersSet->AddObj(pMember))
		return false;

	return true;
}

void CFormTeam::OnTimer()
{
	switch (GetStatus())
	{
	case FORMTEAM_NONE:
		break;
	case FORMTEAM_MARCH:
		ProcessMarch();
		break;
	case FORMTEAM_SORTFORM:
		ProcessSortForm();
		break;
	case FORMTEAM_PASSBLOCK:
		ProcessPassBlock();
		break;
	case FORMTEAM_IDLE:
		ProcessIdle();
		break;
	default:
		ASSERT(false);
		break;
	}
}

bool CFormTeam::SetStatus( FormTeamStaus eStatus )
{
	m_eStatus = eStatus;
	return true;
}

bool CFormTeam::ProcessMarch()
{
	//前进状态，队伍前进一步，成员重新计算目标位置

	if (m_lstDir.empty())
		return false;
	
	const int nNewDir = m_lstDir.front();
	m_lstDir.pop_front();
	
	//编队移动
	CalcDelta(m_ptPosFinder, nNewDir);
	TRACE("MoveNext->[%d,%d]\r\n", m_ptPosFinder.x, m_ptPosFinder.y);

	bool bBeBlock = false;

	//所有成员跟着编队
	OBJPOS posNew;
	int nLoop = 0;
	CFormTeamMemberSet::Iterator itMem = m_pTeamMembersSet->NewEnum();
	while (itMem.Next())
	{
		DEAD_LOOP_BREAK(nLoop, 10000);
		CNpc* pNpc = itMem->QueryNpc();
		IF_NOT(pNpc)
			continue;

		posNew = pNpc->GetPos();
		CalcDelta(posNew, nNewDir);

		//该成员遇到障碍了
		if (IsMaskCell(posNew))
		{
			TRACE("行军时[%u]遇到障碍[%d,%d]", pNpc->GetID(), posNew.x, posNew.y);
			bBeBlock = true;
			continue;
		}
		itMem->MoveForward(nNewDir);
	}

	if (bBeBlock)
	{
		SetStatus(FORMTEAM_PASSBLOCK);
	}

	return true;
}

//重新整队状态，队伍停止前进，等待所有成员走到自己的编队位置
bool CFormTeam::ProcessSortForm()
{
	CNpc* pNpc = NULL;
	bool bFinishSort = true;

	int nLoop = 0;
	CFormTeamMemberSet::Iterator itMem = m_pTeamMembersSet->NewEnum();
	while (itMem.Next())
	{
		DEAD_LOOP_BREAK(nLoop, 10000);

		pNpc = itMem->QueryNpc();
		IF_NOT(pNpc)
			continue;

		if(itMem->Move2Target())
			bFinishSort = false;
	}

	if(bFinishSort)
	{
		//所有成员整队完毕，队伍继续往下一个目标移动
		SetStatus(FORMTEAM_MARCH);
	}

	return true;
}

bool CFormTeam::ProcessPassBlock()
{
	return true;
}

bool CFormTeam::ProcessIdle()
{
	return true;
}

bool CFormTeam::CalcSpeed()
{
	m_MoveTimer.SetInterval(300);
	return true;
}

// bool CFormTeam::Rotate(OBJPOS& ptRotate,
// 					   const MatrixIndex& idxRotate, 
// 					   const MatrixIndex& idxRotateCenter, 
// 					   const OBJPOS& ptRotateCenter, 
// 					   const int nDegree)
// {
// 	if (idxRotate == idxRotateCenter)
// 	{
// 		ptRotate = ptRotateCenter;
// 		return true;
// 	}
// 
// 	const float fRadian = DegreesToRadians(nDegree);
// 
// 	//以旋转轴点为原点的局部向量
// 	MatrixIndex idxMy = idxRotate - idxRotateCenter;
// 	Pix pixLocal;
//  	Convert2Pix(OBJPOS(idxMy.x, idxMy.y), pixLocal);
// 	
// 	//	繞原點逆時針旋轉 θ 度角的變換公式是 x' = xcosθ- ysinθ, y' = xsinθ + ycosθ
// 	Pix pixTarget;
// 	pixTarget.x = pixLocal.x*cos(fRadian) - pixLocal.y*sin(fRadian);
// 	pixTarget.y = pixLocal.x*sin(fRadian) + pixLocal.y*cos(fRadian);
// 	
// 	Convert2Cell(pixTarget, ptRotate);
// 
// 	//调整为地图向量
// 	ptRotate = ptRotateCenter + ptRotate;
// 
// 	return true;
// }

bool CFormTeam::RotateDir(OBJPOS& ptRotate,
					   const MatrixIndex& idxRotate, 
					   const MatrixIndex& idxRotateCenter, 
					   const OBJPOS& ptRotateCenter, 
					   const int nDir)
{
	if (idxRotate == idxRotateCenter)
	{
		ptRotate = ptRotateCenter;
		return true;
	}
	
	//以旋转轴点为原点的局部向量
	MatrixIndex idxMy = idxRotate - idxRotateCenter;
// 	Pix pixLocal;
// 	Convert2Pix(OBJPOS(idxMy.x, idxMy.y), pixLocal);
	
//	Pix pixTarget;
	CHECKF(CalcMatrixRotate(ptRotate, OBJPOS(idxMy.x,idxMy.y), nDir));
	
//	Convert2Cell(pixTarget, ptRotate);
	
	//调整为地图向量
	ptRotate = ptRotateCenter + ptRotate;
	
	return true;
}

bool CFormTeam::CalcMatrixPos( OBJPOS& posWorld, const MatrixIndex& idxMatrix )
{
	IF_NOT(RotateDir(posWorld, idxMatrix, GetPathFinderIdx(), GetFinderPos(), GetDir()))
		return false;

	return true;
}

bool CFormTeam::SelectPathFinderPos(const OBJPOS& ptTarget )
{
	//只判断TopLeft和TopRight两个端点
	OBJPOS posTopLeft;
	CalcMatrixPos(posTopLeft, MATRIX_00);

	OBJPOS posTopRight;
	CalcMatrixPos(posTopRight, MatrixIndex(m_szMatrix.cx-1, 0));

	if (manhattan(posTopLeft, ptTarget) < manhattan(posTopRight, ptTarget))
	{
		m_ptPosFinder = posTopLeft;
		m_idxPathFinder = MATRIX_00;
	}
	else
	{
		m_ptPosFinder = posTopRight;
		m_idxPathFinder = MatrixIndex(m_szMatrix.cx-1, 0);
	}
	TRACE("New Finder Pos[%d,%d] Idx[%d,%d]\r\n", m_ptPosFinder.x, m_ptPosFinder.y,
		m_idxPathFinder.x, m_idxPathFinder.y);

	return true;
}

CFormTeamMember* CFormTeam::QueryMember( const MatrixIndex& idxMatrix )
{
	int nLoop = 0;
	CFormTeamMemberSet::Iterator itMem = m_pTeamMembersSet->NewEnum();
	while (itMem.Next())
	{
		DEAD_LOOP_BREAK(nLoop, 10000);
		if (itMem->GetMatrixIndex() == idxMatrix)
		{
			return itMem;
		}
	}
	return NULL;
}

// bool CFormTeam::SetFinalTarget( const OBJPOS& ptTarget )
// {
// 	m_lstFinalDir.clear();
// 	m_ptFianlTarget = ptTarget;
// 
// #ifdef _NOGAME
// 	CHECKF(CPathFinder::FindPath(m_lstFinalDir, GetFinderPos(), ptTarget));
// #else
// #pragma error("寻路")
// #endif // _NOGAME
// 
// 	return true;
//}

bool CFormTeam::SetTarget( const OBJPOS& ptTarget )
{
	CHECKF(SelectPathFinderPos(ptTarget));
//	CHECKF(SetFinalTarget(ptTarget));

	m_lstDir.clear();
	m_ptTmpTarget = ptTarget;

#ifdef _NOGAME
	CHECKF(CPathFinder::FindPath(m_lstDir, GetFinderPos(), ptTarget));
#else
#pragma error("寻路")
#endif // _NOGAME

	CHECKF(!m_lstDir.empty());
	
	const int nDir = m_lstDir.front();
	CHECKF(nDir>=0 && nDir<=MAX_DIRSIZE);
	
	//m_nDegree = (180+nDir*45)%360;
	SetDir(nDir);
	
	TRACE("m_nDir=%d posFinderIdx[%d,%d] Cell[%d,%d]\r\n", 
		GetDir(), GetPathFinderIdx().x, GetPathFinderIdx().y,
		GetFinderPos().x, GetFinderPos().y);

	//准备重新编队
	OBJPOS posNew;
	CFormTeamMemberSet::Iterator it = m_pTeamMembersSet->NewEnum();
	CNpc* pNpc = NULL;
	int nLoop = 0;
	while (it.Next())
	{
		DEAD_LOOP_BREAK(nLoop, 10000);
		pNpc = it->QueryNpc();
		IF_NOT(pNpc)
			continue;

		const MatrixIndex& idx = it->GetMatrixIndex();
		CalcMatrixPos(posNew, idx);
		it->SetTargetPos(posNew);
		it->FindPath(posNew);
		pNpc->SetPos(posNew.x, posNew.y);

		TRACE("%u [%d,%d]==[%d,%d]\r\n", it->GetID(), it->GetMatrixIndex().x, it->GetMatrixIndex().y, posNew.x, posNew.y);
	}

	ASSERT(SetStatus(FORMTEAM_SORTFORM));

	return true;
}

OBJPOS CFormTeam::GetFinderPos()
{
	ASSERT(m_ptPosFinder.x>=0 && m_ptPosFinder.y>=0);
	return m_ptPosFinder;
}

OBJPOS CFormTeam::GetMatrix00Pos()
{
	OBJPOS pos;
	ASSERT(CalcMatrixPos(pos, MATRIX_00));

	ASSERT(pos.x>=0 && pos.y>=0);
	return pos;
}

CFormTeamMember* CFormTeam::QueryPathFinderMember()
{
	return QueryMember(GetPathFinderIdx());
}

bool CFormTeam::GetTmpTarget( OBJPOS& pt )
{
	pt = m_ptTmpTarget; 
	return m_ptTmpTarget != OBJPOS_NONE;
}

void CFormTeam::SetDir( int nDir )
{
	m_nOldDir = m_nDir;
	m_nDir = nDir;
}