#ifndef PATHFIND_HEADER
#define PATHFIND_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

class CMatrix;

struct OBJLINE
{
	OBJPOS posStart;
	OBJPOS posEnd;
	OBJLINE(){}
	OBJLINE(const OBJPOS& _ptStart, const OBJPOS& _ptEnd):posStart(_ptStart),posEnd(_ptEnd){}
};
typedef std::vector<OBJPOS> OBJPOS_VEC;

class CPathFinder
{
public:
	CPathFinder();
	~CPathFinder();

	static bool	FindPath(DirList& lstDir, const OBJPOS& ptStart, const OBJPOS& ptEnd);
	static bool	FindPath(PathList& lstPath, const OBJPOS& ptStart, const OBJPOS& ptEnd);

protected:
	//走对角线
	static bool	DelCrossPath(PathList& lstPath);

	//////////////////////////////////////////////////////////////////////////
public:
	//线段相交公式
	static bool GetCrossPoint(const OBJLINE& line, OBJPOS_VEC& vecPos);
// 	static bool compare_double(double x1,double x2)	{ return(x1-x2<0.000001&&x1-x2>-0.000001); }
// 	//pt是否在线段上
// 	template<class T>
// 	static bool IsInnerPt(const T& pt, const T& pt1, const T& pt2)
// 	{
// 		//pt为端点？
// 		if (pt==pt1 || pt==pt2)
//			return true;
// 
// 		const int x1 = pt.x - pt1.x;
// 		const int x2 = pt2.x - pt1.x;
// 		const int y1 = pt.y - pt1.y;
// 		const int y2 = pt2.y - pt1.y;
// 		if( x1*y2 - x2*y1 ==0 )
// 			return 0;
// 		return((min(pt1.x,pt2.x)<=pt.x&&pt.x<=max(pt1.x,pt2.x)&&min(pt1.y,pt2.y)<=pt.y&&pt.y<=max(pt1.y,pt2.y)));
// 	}
			/* 点是否在线段上: 向量判断 */
};

//////////////////////////////////////////////////////////////////////////
//点是否在线段上
struct Vector
{
	int x;
	int y;
};

// 由两个点构造一个向量
template<class T> void VectorConstruct(const T& A, const T& B, Vector& v)	{ v.x = B.x - A.x; v.y = B.y - A.y; }
// 向量的叉积
inline double CrossProduct(const Vector& a, const Vector& b)	{ return a.x * b.y - a.y * b.x; }

template<class T> static bool PointIsOnSegment(const T& P, const T& A, const T& B)
{
	Vector AP;
	VectorConstruct(A, P, AP);
	Vector AB;
	VectorConstruct(A, B, AB);
	
	// 两向量不平行
	if (CrossProduct(AP, AB) == 0
		 && P.x >= min(A.x, B.x) && P.x <= max(A.x, B.x)
		 && P.y >= min(A.y, B.y) && P.y <= max(A.y, B.y))
	{
		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
// #define DIM_H	800
// #define DIM_V	600
// 
// #define CELL_SIDE 20	//40
// #define NUM_ROWS (DIM_V - CELL_SIDE) / CELL_SIDE
// #define NUM_COLS (DIM_H - CELL_SIDE) / CELL_SIDE

#define WALK_COST 10

#define MAX_GEN_NODES 4

//////////////////////////////////////////////////////////////////////////
//used structures
typedef struct point
{
	int x;
	int y;
} point;

typedef struct cell_indexes
{
	int row;
	int col;
} cell_ind;

void	set_cols_count(int val);
int		get_cols_count();

void	set_rows_count(int val);
int		get_rows_count();

/* 	
  	F = G + H
	G = previous movement cost
	H = euristic future movement cost
*/

typedef struct list_node 
{
	cell_indexes pt;
	int f;
	int g;
	int h;
	struct list_node *next;
	struct list_node *father;
} l_node;

int pathfind(int** map, const cell_indexes& start, const cell_indexes& end, cell_ind * *path);
int gen_nodes(cell_ind center, cell_ind * *gen);
int manhattan(cell_ind n, cell_ind dest);
void insert_node(l_node * *pList, l_node * node);
void insert_head_node(l_node * *pList, l_node * node);
l_node * get_node(l_node * pList, cell_ind node);
l_node * extract_head(l_node * *pList);
void sort_node(l_node * *pList, l_node * node);
void del_list(l_node * *pList);

#endif
