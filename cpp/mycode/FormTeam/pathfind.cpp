#include "StdAfx.h"
#include "pathfind.h"
#include "Matrix.h"
#include <algorithm>
int g_cols_count = 0;
int g_rows_count = 0;

void set_cols_count(int val)
{
	g_cols_count = val;
}

int get_cols_count()
{
	return g_cols_count;
}

void set_rows_count(int val)
{
	g_rows_count = val;
}

int get_rows_count()
{
	return g_rows_count;
}


//this function calculates a pathfind from start to end cells using A* algorithm 
//based on "manhattan distance" heuristic
int pathfind(int** map, const cell_indexes& start, const cell_indexes& end, cell_ind * *path)
{
	//start or end not walkable or start == end
	if(!map[start.row][start.col] || !map[end.row][end.col] 
	|| start.row == end.row && start.col == end.col)
		return 0;
	
	//A* listes
	l_node * open = NULL;
	l_node * closed = NULL;
	//current node
	l_node * cur = NULL;
	//generated node
	l_node * gen = NULL;
	//node already present into OPEN pList
	l_node * already_ins = NULL;
	//travel node
	l_node * trav = NULL;

	//node extracted from OPEN
	cell_ind cur_ind;
	//expanded nodes
	cell_ind * exp;
	//management cell_ind
	cell_ind temp;
	
	//init first node 
	l_node * first = (l_node *)malloc(sizeof(l_node));
	first->pt.row = start.row; 
	first->pt.col = start.col;
	first->f = 0;
	first->g = 0;
	first->h = 0;
	first->next = NULL;
	first->father = NULL;

	//add first node to OPEN
	insert_node(&open,first);

	//local counter 
	int i;
	//nodes expanded at every cycle
	int num_exp_nodes = 0;

	//found a path
	bool found = false;

	//go on until found a path or open is empty
	while(!found && open) 
	{
		//extract the node with lowest f value
		cur = extract_head(&open);
		cur_ind.row = cur->pt.row;
		cur_ind.col = cur->pt.col;

		//expand nodes from the current obtaining cell indexes
		num_exp_nodes = gen_nodes(cur_ind,&exp);

		//insert current into CLOSED pList
		insert_head_node(&closed,cur);
		
		//manage expanded nodes
		for(i = 0; i < num_exp_nodes; i++) 
		{
			temp.row = (exp+i)->row;
			temp.col = (exp+i)->col;
				
			//walkable node & not yet into CLOSED (not yet analized)
			if(map[temp.row][temp.col] && get_node(closed,temp) == NULL)
			{
				//check if temp is already into OPEN
				already_ins = get_node(open,temp);
				
				//generated node is not yet into OPEN
				if(already_ins == NULL)
				{
					gen = (l_node *)malloc(sizeof(l_node));
					gen->pt.row = temp.row;
					gen->pt.col = temp.col;
					gen->g = cur->g + WALK_COST; 
					gen->h = manhattan(temp,end);
					gen->f = gen->g + gen->h;
					gen->father = cur;

					//insert generated node into OPEN
					insert_node(&open,gen);

					// h(n)= 0 ==> END NODE!!! get out!
					if(!gen->h)
					{
						found = true;
						break;
					}
				}
				//found a better path
				else if(already_ins->g > (cur->g + WALK_COST))
				{
					//recalculate node fields
					already_ins->pt.row = temp.row;
					already_ins->pt.col = temp.col;
					already_ins->g = cur->g + WALK_COST;
					already_ins->h = manhattan(temp,end);
					already_ins->f = already_ins->g + already_ins->h;
					//resort updated node
					sort_node(&open,already_ins);
				}
			}
		}
		//free expanded cell_ind
		free(exp);
	}

	//no path found... it's a defeat :(
	if(!found)
	{
		//free open & closed lists
		del_list(&open);
		del_list(&closed);

		return 0;
	}

	//calculate path length
	int	len_path = ((gen->g)/WALK_COST) + 1;
	int len_bk = len_path;

	//allocate memory for path
	*path = (cell_ind *)malloc(sizeof(cell_ind)*len_path);

	//fill path array with data from nodes
	trav = gen;
	while(trav != NULL) 
	{
		len_path--;
		((*path)+len_path)->row = trav->pt.row;
		((*path)+len_path)->col = trav->pt.col;
		trav = trav->father;
	}
	
	//free open & closed lists
	del_list(&open);
	del_list(&closed);

	return len_bk;
}

//this function expands nodes from a initial one
int gen_nodes(cell_ind center, cell_ind * *gen)
{
	int num_nodes = MAX_GEN_NODES;
	int c = 0;
	
	*gen = (cell_ind *)malloc(sizeof(cell_ind)*num_nodes);
	
	//initial node is on first or last row
	if(!center.row || center.row == get_rows_count())	
		num_nodes--;
	//initial node is on first or last col
	if(!center.col || center.col == get_cols_count())
		num_nodes--;
	
	//row > 0 => upper cell IN
	if(center.row)
	{
		(*gen)[c].row = center.row - 1;
		(*gen)[c].col = center.col; 

		c++;
	}

	//col < get_cols_count-1 => right cell IN
	if(center.col < (get_cols_count() - 1))
	{
		(*gen)[c].row = center.row;
		(*gen)[c].col = center.col + 1; 
		
		c++;
	}
	
	//row < get_cols_count()-1 => bottom cell IN
	if(center.row < (get_rows_count() - 1))
	{
		(*gen)[c].row = center.row + 1;
		(*gen)[c].col = center.col; 
		
		c++;
	}

	//col > 0 => left cell IN
	if(center.col)
	{
		(*gen)[c].row = center.row;
		(*gen)[c].col = center.col - 1; 
		
		c++;
	}

	return c;
}

//this function calculates the "manhattan distance" between two cells 
//just the orthogonal distance ;)
int manhattan(cell_ind n, cell_ind dest)
{
	return WALK_COST * (abs(n.row-dest.row) + abs(n.col-dest.col));
}

//this function inserts node into a pList of l_node ordering it by f and
//then by g values (for equal f nodes)
void insert_node(l_node * *pList, l_node * node)
{
	l_node * trav = *pList;
	
	//new first node
	if(trav == NULL || trav->f > node->f)
	{
		node->next = trav;
		*pList = node;
		return ;
	}

	while(trav->next != NULL)
	{
		//middle insertion
		if((trav->next->f >= node->f))		
		{
			//move on for equal f value
			while(trav->next->f == node->f && trav->next->g < node->g)
			{
				trav = trav->next;

				if(trav->next == NULL)
						break;
			}
			
			node->next = trav->next;
			trav->next = node;

			return ;
		}

		trav = trav->next;
	}

	//new last node
	node->next = NULL;
	trav->next = node;
}

//this function insert a node as head of a pList of l_node  
void insert_head_node(l_node * *pList, l_node * node)
{
	//new first node
	node->next = *pList;
	*pList = node;
}

//this function looks for a node into a pList 
l_node * get_node(l_node * pList, cell_ind node)
{
	//empy pList
	if(pList == NULL)
		return NULL;

	//movin on!
	while(pList->next != NULL)
	{
		if(pList->pt.row == node.row && pList->pt.col == node.col)
			return pList;

		pList = pList->next;
	}

	return NULL;
}

//this function extract the first node from a pList
l_node * extract_head(l_node * *pList)
{
	l_node * head = *pList;
	
	//empy pList
	if(pList == NULL)
		return NULL;
	
	*pList = (*pList)->next;

	return head;
}

//this function sort a node into a pList according f and then g values
void sort_node(l_node * *pList, l_node * node)
{
	l_node * trav = *pList;
	
	//look for the predecessor of node
	while(trav->next != node && trav->next != NULL)
		trav = trav->next;
	
	//connect previous and next nodes
	trav->next = node->next;

	//reinsert the node to obtain the right order
	insert_node(pList,node);
}

//this function deallocate all elements of a pList of l_node
void del_list(l_node * *pList)
{
	l_node * trav;

	//move along pList and free elements
	while(*pList)
	{
		trav = *pList;
		*pList = (*pList)->next;
		free(trav);
	}
}

//////////////////////////////////////////////////////////////////////////
//class CPathFinder

CPathFinder::CPathFinder()
{
	
}

CPathFinder::~CPathFinder()
{
	
}

bool CPathFinder::FindPath(DirList& lstDir, const OBJPOS& ptStart, const OBJPOS& ptEnd)
{
	PathList lstPath;
	if(!FindPath(lstPath, ptStart, ptEnd))
		return false;

	PathList::const_iterator itPath=lstPath.begin();
	while (itPath!=lstPath.end())
	{
		PathList::const_iterator itNextPath = itPath + 1;
		if (*itNextPath == ptEnd)
		{
			lstDir.push_back(GetDirByPos(*itPath, ptEnd));
			break;
		}
		else
			lstDir.push_back(GetDirByPos(*itPath, *itNextPath));
		
		++itPath;
	}

	return true;
}

bool CPathFinder::FindPath(PathList& lstPath, const OBJPOS& ptStart, const OBJPOS& ptEnd)
{
	lstPath.clear();

	if (ptStart == ptEnd
		|| ptStart==POINT_NONE
		|| ptEnd == POINT_NONE)
	{
		return false;
	}

	const int nWidthCell = get_rows_count();
	const int nHeightCell = get_cols_count();
	
	int** ppMap = new int*[nWidthCell];
	for (int i=0; i<nWidthCell; ++i)
	{
		ppMap[i] = new int[nHeightCell];
	}
	
	//初始化地图数据
	for (int x=0; x<nWidthCell; ++x)
	{
		for (int y=0; y<nHeightCell; ++y)
		{
			ppMap[x][y] = 1;
			const CMatrix::MatrixCell* pCell = CMatrix::Instance()->GetCell(CPoint(x,y));
			IF_OK(pCell)
			{
				if(pCell->bMask)
					ppMap[x][y] = 0;	//打上掩码
			}
		}
	}

	cell_indexes start;
	start.row = ptStart.x;
	start.col = ptStart.y;
	
	cell_indexes end;
	end.row = ptEnd.x;
	end.col = ptEnd.y;
	
	cell_ind* pNewPath = NULL;
	const int nNewPathLen = pathfind(ppMap, start, end, &pNewPath);
	TRACE("nNewPathLen=%d\r\n", nNewPathLen);

	//save path
	lstPath.reserve(nNewPathLen);
	for ( int j=0; j<nNewPathLen; ++j)
	{
		lstPath.push_back(OBJPOS(pNewPath[j].row, pNewPath[j].col));
	}

	//free
	for (int y=0; y<nWidthCell; ++y)
	{
		free(ppMap[y]);
	}
	free(ppMap);
	free(pNewPath);

	DelCrossPath(lstPath);

	return true;
}

bool CPathFinder::DelCrossPath( PathList& lstPath )
{
	PathList::iterator itPath = lstPath.begin();
	while(itPath != lstPath.end())
	{
		PathList::iterator itPathN = itPath + 1;
		PathList::iterator itPathNN = itPath + 2;
		if (itPathN == lstPath.end()
			|| itPathNN == lstPath.end())
			break;

		if (abs(itPath->x-itPathNN->x) == 1 
			&& abs(itPath->y-itPathNN->y) == 1)
		{
			//2步之后是对角线，就不走下一步
			itPath = lstPath.erase(itPathN);
		}
		else
		{
			++itPath;
		}
	}

	return true;
}

bool Add2Vec(OBJPOS_VEC& vec,const OBJPOS& x)
{
	if (std::find(vec.begin(), vec.end(), x) != vec.end())
		return false;
	vec.push_back(x);
	return true;
}

//线段相交点
bool GetCrossPoint(const OBJLINE& line1, const OBJLINE& line2, OBJPOS& posCrossPoint)
{
	//一条线段的两个端点为A(x1,y1),B(x2,y2),另一条线段的两个端点为C(x3,y3),D(x4,y4)
	//	d   =   (y2-y1)(x4-x3)-(y4-y3)(x2-x1)
	// 	x0   =   [(x2-x1)*(x4-x3)*(y3-y1)+(y2-y1)*(x4-x3)*x1-(y4-y3)*(x2-x1)*x3]/d
	// 	y0   =   [(y2-y1)*(y4-y3)*(x3-x1)+(x2-x1)*(y4-y3)*y1-(x4-x3)*(y2-y1)*y3]/(-d)
	const int d = (line1.posEnd.y-line1.posStart.y)*(line2.posEnd.x-line2.posStart.x)
		-(line2.posEnd.y-line2.posStart.y)*(line1.posEnd.x-line1.posStart.x);
	if(d == 0)	//平行或重合
		return false;
	posCrossPoint.x = ((line1.posEnd.x-line1.posStart.x)*(line2.posEnd.x-line2.posStart.x)*(line2.posStart.y-line1.posStart.y)
		+(line1.posEnd.y-line1.posStart.y)*(line2.posEnd.x-line2.posStart.x)*line1.posStart.x
		-(line2.posEnd.y-line2.posStart.y)*(line1.posEnd.x-line1.posStart.x)*line2.posStart.x)/d;
	posCrossPoint.y = -1*((line1.posEnd.y-line1.posStart.y)*(line2.posEnd.y-line2.posStart.y)*(line2.posStart.x-line1.posStart.x)
		+(line1.posEnd.x-line1.posStart.x)*(line2.posEnd.y-line2.posStart.y)*line1.posStart.y
		-(line2.posEnd.x-line2.posStart.x)*(line1.posEnd.y-line1.posStart.y)*line2.posStart.y)/d;

	return true;
}

bool CPathFinder::GetCrossPoint( const OBJLINE& line, OBJPOS_VEC& vecPos )
{
	OBJLINE line2;
	OBJPOS posCross;

	const int nX1 = min(line.posStart.x, line.posEnd.x);
	const int nX2 = max(line.posStart.x, line.posEnd.x);
	const int nY1 = min(line.posStart.y, line.posEnd.y);
	const int nY2 = max(line.posStart.y, line.posEnd.y);

	vecPos.reserve(Distance(line.posStart, line.posEnd));

	//和x=n的焦点
	if (line.posStart.x != line.posEnd.x)
	{
		const int nVectorX = line.posStart.x>line.posEnd.x ? -1 : 1;	//轴向量
		int nX = line.posStart.x + nVectorX;	//过滤起点
		const int nOffsetX = abs(line.posStart.x - line.posEnd.x);
		for (int i=0; i<nOffsetX; ++i)
		{
			line2.posStart.x = line2.posEnd.x = nX;
			line2.posStart.y = nY1-1;
			line2.posEnd.y = nY2+1;
			if(::GetCrossPoint(line, line2, posCross))
				Add2Vec(vecPos, posCross);
			nX += nVectorX;
		}
	}

	//和y=n的焦点
	if (line.posStart.y != line.posEnd.y)
	{
		const int nVectorY = line.posStart.y>line.posEnd.y ? -1 : 1;	//轴向量
		int nY = line.posStart.y + nVectorY;	//过滤起点
		const int nOffsetY = abs(line.posStart.y - line.posEnd.y);
		for (int i=0; i<nOffsetY; ++i)
		{
			line2.posStart.y = line2.posEnd.y = nY;
			line2.posStart.x = nX1-1;
			line2.posEnd.x = nX2+1;
			if(::GetCrossPoint(line, line2, posCross))
				Add2Vec(vecPos, posCross);
			nY += nVectorY;
		}
	}


	class CalcDistance
	{
	public:
		CalcDistance(const OBJPOS& posStart):m_posStart(posStart){}
		bool operator() (const OBJPOS& pos1, const OBJPOS& pos2)	{ return Distance(m_posStart,pos1) < Distance(m_posStart,pos2); }
	protected:
		const OBJPOS& m_posStart;
	};
	std::sort(vecPos.begin(),vecPos.end(), CalcDistance(line.posStart));
	// 	//unique()将进行排序，将重复的元素移动到队列后面
	// 	OBJPOS_VEC::iterator new_end = std::unique(vecPos.begin(),vecPos.end());
	// 
	// 	//队列前面的元素都是唯一的，现在删掉new_end后面的元素
	// 	vecPos.erase(new_end,vecPos.end());

	return true;
}
