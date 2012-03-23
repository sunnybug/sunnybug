#include "stdafx.h"
#include "DispChart.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>


CChartBase::CChartBase()
{
	m_hChartDC = NULL;
	m_rtWindow.left  = 0;
	m_rtWindow.right = 600;
	m_rtWindow.top   = 0;
	m_rtWindow.bottom= 400;
	m_rtChart.left   = m_rtWindow.left + 20;
	m_rtChart.right  = m_rtWindow.right;
	m_rtChart.top    = m_rtWindow.top;
	m_rtChart.bottom = m_rtWindow.bottom;

	m_xStart = 0;
	m_xEnd   = 100;
	m_yStart = 0;
	m_yEnd   = 100;

	m_clrChartBg = RGB(255,255,255);
	m_clrChartBorder = RGB(0,0,0);

	m_bGridOn = true;
	m_dxGrid  = 10;
	m_dyGrid  = 10;
	m_clrGrid = RGB(0xc0,0xc0,0xc0);

	strcpy(m_xLabel,"x");
	strcpy(m_yLabel,"y");
	m_bxLabelOn = true;
	m_byLabelOn = true;
	m_bxRulerOn = true;
	m_byRulerOn = true;

	strcpy(m_szRulerXFormat, "%.2f");
	strcpy(m_szRulerYFormat, "%.2f");

	m_clrLabel = RGB(0,0,0);

	memset(&m_logFont, 0, sizeof(LOGFONT));
	m_logFont.lfHeight = 15;
	strcpy(m_logFont.lfFaceName, "Arial");


}
CChartBase::~CChartBase()
{
}

// 辅助函数 ----窗口设置
void CChartBase::SetChartDC(HDC hdc)
{ 
	m_hChartDC = hdc; 
}

HDC  CChartBase::GetChartDC() const
{
	return m_hChartDC;
}
void CChartBase::SetChartWindowPosition(RECT rect)
{ 
	m_rtWindow = rect;
}
void CChartBase::SetChartWindowPosition(int left, int top, int right, int bottom)
{
	m_rtWindow.left = left;
	m_rtWindow.top  = top;
	m_rtWindow.right = right;
	m_rtWindow.bottom = bottom;
}

RECT CChartBase::GetChartWindowPosition()const
{
	return m_rtWindow;
}

void CChartBase::SetChartPosition(RECT rect)
{
	m_rtChart = rect;
}
void CChartBase::SetChartPosition(int left, int top, int right, int bottom)
{
	m_rtChart.left = left;
	m_rtChart.top  = top;
	m_rtChart.right = right;
	m_rtChart.bottom = bottom;
}

RECT CChartBase::GetChartPosition()const
{
	return m_rtChart;
}

// 辅助函数---坐标显示区域
double CChartBase::SetXStart(double xStart)
{
	double temp = m_xStart;
	if(xStart<m_xEnd)
		m_xStart = xStart;
	return temp;
} 

double CChartBase::SetXEnd(double xEnd)
{
	double temp = m_xEnd;
	if(xEnd>m_xStart)
		m_xEnd = xEnd;
	return temp;
}

void CChartBase::SetXRange(double xStart,double xEnd)
{
	if(xStart<xEnd){
		m_xStart = xStart;
		m_xEnd   = xEnd;
	}
}

double CChartBase::SetYStart(double yStart)
{
	double temp = m_yStart;
	if(yStart<m_yEnd)
		m_yStart = yStart;
	return temp;
}

double CChartBase::SetYEnd(double yEnd)
{
	double temp = m_yEnd;
	if(yEnd>m_yStart)
		m_yEnd = yEnd;
	return temp;
}

void CChartBase::SetYRange(double yStart,double yEnd)
{
	if(yStart<yEnd){
		m_yStart = yStart;
		m_yEnd = yEnd;
	}
}

double CChartBase::GetXStart()const
{
	return m_xStart;
}

double CChartBase::GetXEnd()const
{
	return m_xEnd;
}

double CChartBase::GetYStart()const
{
	return m_yStart;
}

double CChartBase::GetYEnd()const
{
	return m_yEnd;
}

// 辅助函数---显示图表区
void  CChartBase::SetClrChartBg(COLORREF clr)
{
	m_clrChartBg = clr;
}
COLORREF  CChartBase::GetClrChartGb()const
{
	return m_clrChartBg;
}
void      CChartBase::SetClrChartBorder(COLORREF clr)
{
	m_clrChartBorder = clr;
}
COLORREF  CChartBase::GetClrChartBorder()const
{
	return m_clrChartBorder;
}


// 辅助函数---网格
double CChartBase::SetGriddx(double dxGrid)
{
	double temp = m_dxGrid;
	if(dxGrid>0)
	    m_dxGrid = dxGrid;
	return temp;
}
double CChartBase::SetGriddy(double dyGrid)
{
	double temp = m_dyGrid;
	if(dyGrid>0)
	    m_dyGrid = dyGrid;
	return temp;
}
void CChartBase::SetGridxy(double dxGrid,double dyGrid)
{
	if(dxGrid>0 && dyGrid>0)
	{
		m_dxGrid = dxGrid;
	    m_dyGrid = dyGrid;
	}
}
COLORREF CChartBase::SetGridColor(COLORREF color)
{
	COLORREF temp = m_clrGrid;
	m_clrGrid = color;
	return temp;
}
void CChartBase::SetGridOn()
{
	m_bGridOn = true;
}
void CChartBase::SetGridOff()
{
	m_bGridOn = false;
}

COLORREF CChartBase::GetGridColor()const
{
	return m_clrGrid;
}
double CChartBase::GetGriddx()const
{
	return m_dxGrid;
}
double CChartBase::GetGriddy()const
{
	return m_dyGrid;
}
bool   CChartBase::GetGridStatus()const
{
	return m_bGridOn;
}

// 辅助函数---坐标轴,轴标,标尺
void CChartBase::SetXLabel(const char* xLabel)
{
	strcpy(m_xLabel, xLabel);	
}

void  CChartBase::SetYLabel(const char* yLabel)
{
	strcpy(m_yLabel, yLabel);
}
void    CChartBase::SetXLabelOn()
{
	m_bxLabelOn = true;
}
void    CChartBase::SetXLabelOff()
{
	m_bxLabelOn = false;
}
BOOL    CChartBase::GetXLabelStatus()const
{
	return m_bxLabelOn;
}

void    CChartBase::SetYLabelOn()
{
	m_byLabelOn = true;
}
void    CChartBase::SetYLabelOff()
{
	m_byLabelOn = false;
}
BOOL    CChartBase::GetYLabelStatus()const
{
	return m_byLabelOn;
}

void    CChartBase::SetXRulerOn()
{
	m_bxRulerOn = true;
}
void    CChartBase::SetXRulerOff()
{
	m_bxRulerOn = false;
}
BOOL    CChartBase::GetXRulerStatus()const
{
	return m_bxRulerOn;
}

void    CChartBase::SetYRulerOn()
{
	m_byRulerOn = true;
}
void    CChartBase::SetYRulerOff()
{
	m_byRulerOn = false;
}
BOOL    CChartBase::GetYRulerStatus()const
{
	return m_byRulerOn;
}


void     CChartBase::SetClrLabel(COLORREF clr)
{
	m_clrLabel = clr;
}
COLORREF CChartBase::GetClrLabel()const
{
	return m_clrLabel;
}

// 辅助函数---字体
void CChartBase::SetLogFont(LOGFONT logFont)
{
	m_logFont = logFont;
}

LOGFONT CChartBase::GetLogFont()const
{
	return m_logFont;
}
// 辅助函数---每个象素所代表的实数值
double CChartBase::GetYPerPix()const
{
	return (m_yEnd - m_yStart) / (m_rtChart.bottom - m_rtChart.top);
}

double CChartBase::GetXPerPix()const
{
	return (m_xEnd - m_xStart) / (m_rtChart.right - m_rtChart.left);
}

// 显示函数
int  CChartBase::ShowChartBg()
{
	//绘制边框
	HBRUSH brush_disp =   CreateSolidBrush(GetClrChartGb());
	HBRUSH oldbrush   =   (HBRUSH)SelectObject(GetChartDC(), brush_disp);
	HPEN   pen_disp   =   CreatePen(PS_SOLID, 1, GetClrChartBorder());
	HPEN   pen_disp_old = (HPEN)SelectObject(GetChartDC(), pen_disp);

	Rectangle(GetChartDC(),
		      m_rtChart.left,
			  m_rtChart.top,
			  m_rtChart.right,
			  m_rtChart.bottom);

	SelectObject(GetChartDC(),oldbrush);
	DeleteObject(brush_disp);
	SelectObject(GetChartDC(),pen_disp_old);
	DeleteObject(pen_disp);
	return 0;
}
int  CChartBase::ShowGrid()
{
	if(m_bGridOn) 
	{   
		// 绘制网格
		HPEN newpen = CreatePen(PS_DOT, 1, m_clrGrid);
		HPEN oldpen = (HPEN)SelectObject(m_hChartDC, newpen);

		//x轴的格子
		double x= ceil(m_xStart);
		int xs;
		while(x < floor(m_xEnd))
		{
			xs = rx2sx(x);
			MoveToEx(m_hChartDC ,xs , m_rtChart.top, NULL);
			LineTo(m_hChartDC, xs, m_rtChart.bottom);

			x += m_dxGrid;
		}
		//y轴的格子
	    double y = ceil(m_yStart);
		int ys;
		while( y < floor(m_yEnd))
		{
			ys = ry2sy(y);
			MoveToEx(m_hChartDC,m_rtChart.left, ys, NULL);
			LineTo(m_hChartDC,m_rtChart.right, ys);
			y += m_dyGrid;
		}

		SelectObject( m_hChartDC, oldpen);
		DeleteObject( newpen);

	}
	return 0;
}
int  CChartBase::ShowRuler()
{
	LONG char_unit   = GetDialogBaseUnits();
	long char_width  = LOWORD(char_unit);
	long char_height = HIWORD(char_unit);
	char str[50];

    HPEN pen_ruler = CreatePen(PS_SOLID, 1, m_clrLabel);
	HPEN oldpen    = (HPEN)SelectObject(m_hChartDC, pen_ruler);
	HFONT font     = CreateFontIndirect(&m_logFont);
	HFONT oldfont  = (HFONT)SelectObject(m_hChartDC, font);

	SetTextColor(m_hChartDC, m_clrLabel);
	SetBkMode(m_hChartDC,TRANSPARENT);

	if(GetXRulerStatus()){
		//显示x轴的标尺
		double x = ceil(m_xStart);
		int xs;
		int counter = 1;  //计数器
		while( x< floor(m_xEnd))
		{
	
			xs = rx2sx(x);
			sprintf(str,m_szRulerXFormat, x);
			int label_length = strlen(str) * char_width + char_width;
             
			//label_length = 8*char_width; //此处屏蔽了上面的设置,仅假设要显示的数据为 -xxx.xx
			                             //上面的则坐标宽度是变化的

			if(label_length < counter*m_dxGrid/GetXPerPix()){

				MoveToEx(m_hChartDC, xs, m_rtChart.bottom, NULL);
				LineTo  (m_hChartDC, xs, m_rtChart.bottom + 5);
				
				TextOut (m_hChartDC ,xs, m_rtChart.bottom + 5, str, strlen(str));
				counter = 1;
			}
			
			counter++;
			x += m_dxGrid;
		}
	}

	if(GetYRulerStatus()){
		//显示y轴的标尺
		double y = ceil(m_yStart);
		int ys;
		int counter=1;
		while( y <floor(m_yEnd) )
		{
			ys = ry2sy(y);
			sprintf(str,m_szRulerYFormat,y);

			int label_height = char_height+5;
            
			if(label_height<counter*m_dyGrid/GetYPerPix()){
				MoveToEx (m_hChartDC,m_rtChart.left , ys, NULL);
				LineTo   (m_hChartDC,m_rtChart.left - 5, ys);
				TextOut  (m_hChartDC,
					      m_rtChart.left - 5 - strlen(str) * char_width,
					      ys - char_height/2,
					      str,
						  strlen(str));

				counter = 1;
			}
			counter++;
			y += m_dyGrid;
		}
	}

	SelectObject(m_hChartDC, oldpen);
    DeleteObject(pen_ruler);

	SelectObject(m_hChartDC, oldfont);
	DeleteObject(font);
	return 0;
}
int  CChartBase::ShowLabel()
{
	//绘制x,y标识
	LONG char_unit   = GetDialogBaseUnits();
	long char_width  = LOWORD(char_unit);
	long char_height = HIWORD(char_unit);

	HFONT font     = CreateFontIndirect(&m_logFont);
	HFONT oldfont  = (HFONT)SelectObject(m_hChartDC, font);
	SetTextColor(m_hChartDC,m_clrLabel);
	SetBkMode  (m_hChartDC,TRANSPARENT);
	
	if(GetXLabelStatus())
		TextOut(m_hChartDC,
		        m_rtChart.right  - strlen(m_xLabel) * char_width,
				m_rtChart.bottom + char_height + 8,
				m_xLabel,
				strlen(m_xLabel));
	if(GetYLabelStatus())
		TextOut(m_hChartDC,
		        m_rtChart.left,
				m_rtChart.top - char_height - 15,
				m_yLabel,
				strlen(m_yLabel));

	SelectObject(m_hChartDC, oldfont);
	DeleteObject(font);
	return 0;
}

int CChartBase::Show()
{
	ShowChartBg();
	ShowGrid();
	ShowRuler();
	ShowLabel();
	return 0;
}

int  CChartBase::ShowAt(int left, int top, int right, int bottom)
{
	LONG char_unit   = GetDialogBaseUnits();
	long char_width  = LOWORD(char_unit);
	long char_height = HIWORD(char_unit);

	SetChartWindowPosition(left, top ,right, bottom);
	SetChartPosition(left + 10*char_width, top + 2*char_height, right, bottom - 4*char_height);
	Show();
	return 0;
}

int CChartBase::ShowAt(RECT rect)
{
	int left   = rect.left;
	int right  = rect.right;
	int top    = rect.top;
	int bottom = rect.bottom;

	LONG char_unit   = GetDialogBaseUnits();
	long char_width  = LOWORD(char_unit);
	long char_height = HIWORD(char_unit);

	SetChartWindowPosition(left, top ,right, bottom);
	SetChartPosition(left + 10*char_width, top + 2*char_height, right, bottom - 4*char_height);
	Show();
	return 0;
}
// 坐标转换函数
int CChartBase::rx2sx(double rx)
{
	int sx;
	sx = m_rtChart.left + int((rx - m_xStart) / GetXPerPix());
	return sx;
}
int CChartBase::ry2sy(double ry)
{
	int sy;
	sy = m_rtChart.top + (m_rtChart.bottom - m_rtChart.top) - int((ry - m_yStart) / GetYPerPix()) - 1;
	return sy;
}
double CChartBase::sx2rx(int sx)
{
	double rx;
	rx = (sx - m_rtChart.left) * GetXPerPix() + m_xStart;
	return rx;
}
double CChartBase::sy2ry(int sy)
{
	double ry;
	ry = (m_rtChart.top + (m_rtChart.bottom - m_rtChart.top) - 1 - sy)*GetYPerPix() + m_yStart;
	return ry;
}

// 坐标变换
void CChartBase::Move(double drx, double dry)
{
	m_xStart  += drx;
	m_xEnd    += drx;
	m_yStart  += dry;
	m_yEnd    += dry;
}

void CChartBase::ScaleCenter(double times)
{
	if(times<0)
		return ;
	m_xStart  =((m_xStart + m_xEnd) / 2) - (m_xEnd - m_xStart) * times / 2;
	m_xEnd    =((m_xStart + m_xEnd) / 2) + (m_xEnd - m_xStart) * times / 2;
	m_yStart  =((m_yStart + m_yEnd) / 2) - (m_yEnd - m_yStart) * times / 2;
	m_yEnd    =((m_yStart + m_yEnd) / 2) +( m_yEnd - m_yStart) * times / 2;
}

void CChartBase::ScaleX(double times)
{
	if(times<0)
		return ;
	m_xStart  =((m_xStart + m_xEnd) / 2) - (m_xEnd - m_xStart) * times / 2;
	m_xEnd    =((m_xStart + m_xEnd) / 2) + (m_xEnd - m_xStart) * times / 2;
}

void CChartBase::ScaleY(double times)
{
	if(times<0)
		return ;
	m_yStart  =((m_yStart + m_yEnd) / 2) - (m_yEnd - m_yStart) * times / 2;
	m_yEnd    =((m_yStart + m_yEnd) / 2) +( m_yEnd - m_yStart) * times / 2;
}




//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

CChart::CChart(HWND hWnd)
{
	m_hWnd    = hWnd;

	m_memDCWindow   = NULL;
	m_memDCChart    = NULL;
	m_bmpInDCWindow = NULL;
	m_bmpInDCChart  = NULL;

	m_bLeftButtonDown  = false;
	m_bRightButtonDown = false;

	m_ptLeftButtonDown.x = -1;
	m_ptLeftButtonDown.y = -1;
	m_ptRightButtonDown.x = -1;
	m_ptRightButtonDown.y = -1;

    m_clrScaleSelectBg = RGB( 0,255,0);          
    m_clrScaleSelectBorder = RGB(0,0,0);   
    m_nLineWidthOfScaleSelectBorder = 1; 
	m_nTransRate  = 0x7f;

	m_clrCoordinateLine = RGB( 0,0x7f, 0x7f);
	m_nLineWidthCoordinateLine = 1;

	m_clrCoordWindowBg      =  RGB( 255,255, 0 ) ;   
	m_clrCoordWindowBorder  =  RGB(0, 0x7f, 0x7f );;      
	m_nLineWidthOfCoordWindowBorder = 1; 
	m_nCoordWindowTransRate = 200;   

	m_lfCoordWindow;  
	memset(&m_lfCoordWindow, 0, sizeof(LOGFONT));
	m_lfCoordWindow.lfHeight = 13;
	strcpy(m_lfCoordWindow.lfFaceName, "Arial");
}

CChart::~CChart()
{
	if(m_memDCWindow != NULL)
		DeleteDC( m_memDCWindow);
	if(m_memDCChart  != NULL)
		DeleteDC( m_memDCChart);
	if(m_bmpInDCWindow != NULL)
		DeleteObject( m_bmpInDCWindow);
	if(m_bmpInDCChart  != NULL)
		DeleteObject( m_bmpInDCChart);
}

// 曲线数据
void CChart::AddACurve(std::vector<double>* pvCurveData)
{
	// 添加一条曲线
	m_pvCurvesData.push_back(pvCurveData);
	m_vCurvesScale.push_back(1.0);

	// 添加曲线名称
	if( m_vsCurvesName.empty() )
		m_vsCurvesName.push_back( "x  " );
	int    num = m_vsCurvesName.size() ;
	char str[128];
	sprintf( str, "y%d ", num);
	m_vsCurvesName.push_back( str );

	// 设置默认颜色,线宽
	m_vbCurvesShow.push_back(true);
	m_vCurvesLineWidth.push_back(1);

	srand( (unsigned)time( NULL ) );
	BYTE  r = rand() % 256;
	BYTE  g = rand() % 256;
	BYTE  b = rand() % 256;
	m_vCurvesColor.push_back( RGB(r,g,b) );
}

void CChart::AddMCurves(std::vector<double> pvCurveData[],int n)
{
	// 获得已有曲线名的个数
	int numCurves = m_vsCurvesName.size();

	// 添加n条曲线数据
	for(int i=0;i<n;i++)
	{
		m_pvCurvesData.push_back(&pvCurveData[i]);
		m_vCurvesScale.push_back(1.0);

		if ( i==0 && m_vsCurvesName.empty() )
			m_vsCurvesName.push_back( "x  " );
		else
		{
			char str[128];
			sprintf( str, "y%d ", i + numCurves );
			m_vsCurvesName.push_back( str );
		}
	}

	// 设置默认颜色和线宽
	srand( (unsigned)time( NULL ) );
	for(int i=0;i<n;i++)
	{
		BYTE  r = rand() % 256;
		BYTE  g = rand() % 256;
		BYTE  b = rand() % 256;
		
		m_vbCurvesShow.push_back(true);
		m_vCurvesLineWidth.push_back(1);
		m_vCurvesColor.push_back( RGB(r,g,b) );
	}
}

double CChart::GetDataAt(int m,int n)
{
	return (m_pvCurvesData.at(m)->at(n)) * (m_vCurvesScale.at(m));
}

long  CChart::GetNumCurves()
{
	return m_pvCurvesData.size();
}

long CChart::GetNumDataInCurve(int m)
{
	if( m_pvCurvesData.empty() )
		return 0;
	return m_pvCurvesData.at(m)->size();
}

void CChart::DeleteLastCurve()
{
	m_pvCurvesData.pop_back();
	m_vCurvesScale.pop_back();
	m_vsCurvesName.pop_back();

	m_vbCurvesShow.pop_back();
	m_vCurvesLineWidth.pop_back();
	m_vCurvesColor.pop_back();
}

void CChart::DeleteAllCurves()
{
	m_pvCurvesData.clear();
	m_vCurvesScale.clear();
	m_vsCurvesName.clear();

	m_vbCurvesShow.clear();
	m_vCurvesLineWidth.clear();
	m_vCurvesColor.clear();
}

void CChart::SetCurveScale(int m ,double scale)
{
	if( m <= 0 || m>=GetNumCurves() )
		return;
	m_vCurvesScale.at(m) = scale;
}

double CChart::GetCurveScale(int m)
{
	if( m <= 0 || m>=GetNumCurves() )
		return 0;
	return m_vCurvesScale.at(m);
}

long  CChart::FindDataInX(double x)
{
	if( m_pvCurvesData.empty())
		return -1;

	long size = GetNumDataInCurve(0);  

	// 没有找到的情况
	if( size == 0)
		return -1;
	if( x < GetDataAt(0,0))
		return -1;
	if( x > GetDataAt(0,size - 1))
		return -1;

	// 二分法查找
	long start  = 0 ;         // 起始下标
	long end    = size - 1 ;  // 
	long middle;

	// 是两个端点
	if( x == GetDataAt(0, 0))
		return 0;
	if( x == GetDataAt(0, size - 1))
		return size - 1;

	while(1)
	{
		middle = start + (end - start)/2;
		double data = GetDataAt(0, middle);
		
		if( x < data )
			end   = middle;
		if( x > data )
			start = middle;

		if( start == end - 1)
			return start;
	}

}


void CChart::SetCurveLineWidth(int m, int width)
{
	if( m <= 0 || m>=GetNumCurves() )
		return;
	m_vCurvesLineWidth.at(m) = width;
}

void CChart::SetCurveColor(int m, COLORREF clr)
{
	if( m <= 0 || m>=GetNumCurves() )
		return;
	m_vCurvesColor.at(m) = clr;
}

void CChart::SetCurveStatus(int m,bool bShow)
{
	if( m <= 0 || m>=GetNumCurves() )
		return ;
	m_vbCurvesShow.at(m) = bShow;
}

void CChart::SetCurveName(int m, char * name)
{
	if( m <= 0 || m>=GetNumCurves() )
		return ;
	m_vsCurvesName.at(m) = name ;
}

int  CChart::GetCurveLineWidth(int m)
{
	if( m <= 0 || m>=GetNumCurves() )
		return -1;
	return m_vCurvesLineWidth.at(m);
}

COLORREF CChart::GetCurveColor(int m)
{
	if( m <= 0 || m>=GetNumCurves() )
		return -1;
	return m_vCurvesColor.at(m);
}

bool CChart::GetCurveStatus(int m)
{
	if( m < 0 || m>=GetNumCurves() )
		return false;
	return m_vbCurvesShow.at(m);
}


// 曲线绘制函数

// 设置chart window 和 chart位置(和ShowAt()= SetChartAndWindowPosition()+ Show())
void CChart::SetChartAndWindowPosition( RECT rect)
{
	int left   = rect.left;
	int right  = rect.right;
	int top    = rect.top;
	int bottom = rect.bottom;

	SetChartAndWindowPosition(left, top, right, bottom ) ;
}
void CChart::SetChartAndWindowPosition(int left, int top, int right, int bottom)
{
	LONG char_unit   = GetDialogBaseUnits();
	long char_width  = LOWORD(char_unit);
	long char_height = HIWORD(char_unit);
    SetChartWindowPosition(left, top ,right, bottom);
	SetChartPosition(left + 10*char_width, top + 2*char_height, right, bottom - 4*char_height);
}

void CChart::ShowCurves()
{
	// 获得屏幕设备句柄
	HDC hdc        = GetDC( m_hWnd );

	// 创建ChartWindow内存设备
	m_memDCWindow  = CreateCompatibleDC( hdc );
	m_bmpInDCWindow= CreateCompatibleBitmap( hdc, GetChartWindowWidth(), GetChartWindowHeight() );
	SelectObject( m_memDCWindow, m_bmpInDCWindow ) ;

	// 创建一个与背景色相同的画刷
	HBRUSH  hBrush    = CreateSolidBrush(GetPixel(hdc, m_rtWindow.right/2 - 5,m_rtWindow.top + 5));
	HBRUSH  hOldBrush = (HBRUSH)SelectObject(m_memDCWindow, hBrush);


	// 保存原始值,设定新值
	HDC  dcOld = GetChartDC();
	RECT rtOld = GetChartWindowPosition();   // 保存原始chart window位置值
	RECT rtNew = { 0 , 0 , GetChartWindowWidth(), GetChartWindowHeight() };
	SetChartDC( m_memDCWindow );
	SetChartAndWindowPosition( rtNew ) ;
	
	Rectangle( m_memDCWindow, -1, -1, GetChartWindowWidth() + 1, GetChartWindowHeight() + 1 );
	Show();

    SelectObject( m_memDCWindow, hOldBrush ) ;
	DeleteObject( hBrush);

	// 恢复原始chart window位置属性
	SetChartDC( dcOld );
	SetChartAndWindowPosition( rtOld ) ;  


	// 创建Chart内存设备
	m_memDCChart  = CreateCompatibleDC( hdc );
	m_bmpInDCChart= CreateCompatibleBitmap( hdc, GetChartWidth(), GetChartHeight() );
	SelectObject( m_memDCChart, m_bmpInDCChart ) ;
	BitBlt( m_memDCChart, 0 , 0, GetChartWidth(), GetChartHeight() ,
		    m_memDCWindow, m_rtChart.left, m_rtChart.top ,SRCCOPY);


	// 保存原始值,设定新值
    dcOld = GetChartDC();
	rtOld = GetChartPosition();   // 保存原始chart位置值
	RECT rtNew2 = { 0 , 0 , GetChartWidth(), GetChartHeight() };
	SetChartDC( m_memDCChart );
	SetChartPosition( rtNew2 ) ;

	// 获得曲线数
	int numCurves  = GetNumCurves( );

	// 获得数据长度 
	int nDataLength = GetNumDataInCurve( 0 );   // 取最小的,不然会溢出,所以比较安全
	for ( int i = 1; i<numCurves ; i++ )
	{
		int num = GetNumDataInCurve( i ) ;
		if( nDataLength >  num )
			nDataLength =  num ;
	}

	// m=0 是x轴数据,m=1,2,3...为第1,2,3..条曲线
	for (int m = 1; m< numCurves  ; m++)
	{
		if( GetCurveStatus( m ) && !m_pvCurvesData.at(m)->empty() )  // 判断是否需要显示
		{
			HPEN pen    = CreatePen(PS_SOLID, GetCurveLineWidth( m ), GetCurveColor( m ) );
			HPEN oldpen = (HPEN) SelectObject(  m_memDCChart, pen ) ;

			// int nDataLength = GetNumDataInCurve( m );

			// 绘制曲线,0 -- 1  1 -- 2    n-1 -- n
			int i =0 ;
			int indexEnd = nDataLength - 1;

			double x0 = GetDataAt( 0, 0 ) ;
			double xn = GetDataAt( 0, indexEnd ) ;

			if( x0 < m_xStart && xn > m_xStart)
			{
				i = FindDataInX( m_xStart );
				indexEnd = indexEnd ;
			}
			else if( xn > m_xEnd && x0 < m_xEnd )
			{
				i = 0;
				indexEnd = FindDataInX( m_xEnd );
			}else if ( m_xStart > xn || m_xEnd < x0)
			{
				i = 0 ;
				indexEnd =0 ;
			}

		
			for(  ; i < indexEnd - 1; i++ )
			{
				if( GetDataAt( 0, i) >= GetXStart() &&
					GetDataAt( 0, i) <= GetXEnd() )        // 限定x轴范围
				{
					int sx   = rx2sx( GetDataAt( 0, i ) ) ;
					int sy   = ry2sy( GetDataAt( m, i ) ) ;
					int sxdx = rx2sx( GetDataAt( 0, i + 1 ) ) ;
					int sydy = ry2sy( GetDataAt( m, i + 1 ) ) ;

					MoveToEx(  m_memDCChart, sx, sy, NULL );
					LineTo  (  m_memDCChart, sxdx, sydy );  
				}
			}

			SelectObject( m_memDCWindow, oldpen ) ;
			DeleteObject( pen ) ;
		}
	}

	// 恢复原始chart 位置属性
	SetChartDC( dcOld );
	SetChartPosition( rtOld ) ; 

	BitBlt( m_memDCWindow,m_rtChart.left, m_rtChart.top, GetChartWidth(), GetChartHeight(),
		    m_memDCChart, 0, 0, SRCCOPY);

	// 将内存设备中显示的内容显示在窗口中
	BitBlt( hdc, m_rtWindow.left, m_rtWindow.top,
		    GetChartWindowWidth(), GetChartWindowHeight() ,
			m_memDCWindow, 0 ,0 ,SRCCOPY );

	// 释放资源

	DeleteObject( m_bmpInDCChart ) ;
	DeleteDC( m_memDCChart ) ;

	DeleteObject( m_bmpInDCWindow ) ;
	DeleteDC( m_memDCWindow ) ;

	ReleaseDC( m_hWnd, hdc ) ;
}

// 消息处理辅助
bool CChart::IsMouseInChart(int x, int y)
{
	if( x>m_rtChart.left  &&
		x<m_rtChart.right &&
		y>m_rtChart.top   &&
		y<m_rtChart.bottom)
		return true;
	else
		return false;
}

bool CChart::IsMouseInChart(POINT pt)
{
	return IsMouseInChart(pt.x, pt.y);
}


// 消息处理函数

int  CChart::OnLeftButtonDown(int x, int y)
{
	HDC hdc = GetDC( m_hWnd);
	// 当鼠标左键按下时，鼠标在图表显示区域时
	if(IsMouseInChart(x,y))
	{
		// 设置左键按下状态
		m_bLeftButtonDown = true;

		// 设定鼠标指针
		SetCursor(LoadCursor(NULL,IDC_CROSS));

		// 记录此时鼠标按下的位置
		m_ptLeftButtonDown.x = x;
		m_ptLeftButtonDown.y = y;


		////////////////////////////////////////////////////////////////////////////////////

		// 绘制坐标定位线和坐标窗口
		double rx = sx2rx( x );
		int index = FindDataInX( rx );

		HPEN pen = CreatePen( PS_SOLID, GetLineWidthCoordinateLine(), GetClrCoordinateLine() );
		HPEN old = (HPEN) SelectObject( hdc, pen ) ;

		// 若坐标定位线和曲线相交,则定位线显示在鼠标指向处最近的实际数据处。
		if(index != -1)
			x = rx2sx( GetDataAt( 0, index ) );

		MoveToEx( hdc, x, m_rtChart.top, NULL ) ;
		LineTo(hdc, x, m_rtChart.bottom ) ;

		SelectObject( hdc, old ) ;
		DeleteObject( pen );
        
		
		// 确定坐标显示窗口的大小
		// 形如:
		//        ---------------------------
		//        |   x         |           |
		//        -------------------------
		//        |   y         |           |
		//        -------------------------
		//        |   y2        |           |
		//        -------------------------
		//        |   y3        |           |
	    //        -------------------------
		LONG char_unit   = GetDialogBaseUnits();
	    long char_width  = LOWORD(char_unit);
	    long char_height = HIWORD(char_unit);

		int numCurves = GetNumCurves() ;
		int nWidth    = 10 * char_width + 50 ;                  // 坐标显示窗口矩形宽,50指左边legend区域的宽度
		int nRowHeight= 30 ;                                    // 每一行的高度
		int nHeight   = numCurves * nRowHeight;                 // 坐标显示窗口矩形高

		HDC memDC         = CreateCompatibleDC( hdc );
		HBITMAP hBitmap   = CreateCompatibleBitmap( hdc ,nWidth, nHeight);
		SelectObject( memDC, hBitmap ) ;

		// 绘制边框
		HBRUSH hBrush    = CreateSolidBrush( GetClrCoordWindowBg() );
		HPEN   hPen      = CreatePen( PS_SOLID, GetLineWidthCoordWindowBorder(),GetClrCoordWindowBorder() );
		HBRUSH hOldBrush = (HBRUSH) SelectObject( memDC, hBrush ) ;
		HPEN   hOldPen      = (HPEN)   SelectObject( memDC, hPen   ) ;
		Rectangle( memDC, 0 , 0, nWidth, nHeight );
		SelectObject( memDC, hOldPen ) ;
		DeleteObject( hPen );

		// 绘制坐标信息
		for(int i=0 ; i< numCurves ; i++)
		{
			HPEN  pen  = CreatePen( PS_SOLID, GetCurveLineWidth( i ) ,GetCurveColor( i ) ) ; 
			HFONT font = CreateFontIndirect( &m_lfCoordWindow ) ;
			HPEN  oldpen  = (HPEN)  SelectObject( memDC , pen ) ;
			HFONT oldfont = (HFONT) SelectObject( memDC, font ) ;
			COLORREF oldclr = SetTextColor( memDC, GetCurveColor( i ));
			SetBkMode( memDC, TRANSPARENT) ;

			// 绘制legend的线
			if( i != 0 )
			{
				MoveToEx( memDC, nWidth - 40, i * nRowHeight + nRowHeight/2, NULL );
				LineTo  ( memDC, nWidth - 10, i * nRowHeight + nRowHeight/2) ;
			}
            
			// 绘制legend的文字部分
			char szFormat[128];       //  x (23.23)  -------
			// 获得曲线名称
			sprintf( szFormat,"%s：",GetCurveName( i ) ) ;

			// 获得曲线标尺的文字格式
			if( i == 0  )
				strcat  ( szFormat, m_szRulerXFormat ) ;
			else
				strcat  ( szFormat, m_szRulerYFormat ) ;

			char szData[128];
            if( index != -1 && GetCurveStatus( i ) )
			    sprintf( szData, szFormat, GetDataAt( i, index ) );
			else
				sprintf( szData, "%s：" ,GetCurveName( i ));

			// 显示坐标文字
			TextOut( memDC, 10 , i * nRowHeight + nRowHeight/2 - m_lfCoordWindow.lfHeight/2, szData, strlen(szData));

			// 删除创建的GDI对象
			SetTextColor( memDC, oldclr );
			SelectObject( memDC, oldpen ) ;
			SelectObject( memDC, oldfont ) ;
			DeleteObject( pen );
			DeleteObject( font );
		}

		SelectObject( memDC, hOldBrush ) ;
		DeleteObject( hBrush ) ; 

		// 与屏幕设备alpha混合,显示为透明效果
		BLENDFUNCTION bf;
		bf.BlendOp              =  AC_SRC_OVER ;
		bf.BlendFlags           =  0 ;
		bf.SourceConstantAlpha  =  m_nCoordWindowTransRate;
		bf.AlphaFormat          =  0 ;

		AlphaBlend( hdc, 
			        m_rtChart.right - nWidth, m_rtChart.bottom - nHeight,
					nWidth, nHeight,
					memDC, 0 , 0, nWidth, nHeight, bf ) ;

		DeleteObject( hBitmap );
		DeleteDC( memDC );
		
		///////////////////////////////////////////////////////////////////////////////////

	}else
	{
		m_ptLeftButtonDown.x = -1;
		m_ptLeftButtonDown.y = -1;
	}

	ReleaseDC(m_hWnd, hdc);
	return 0;
}

int  CChart::OnLeftButtonUp(int x, int y)
{
	// 设置左键按下状态
	m_bLeftButtonDown = false;
	if(IsMouseInChart(x, y))
	{
		// 设定鼠标指针
		SetCursor(LoadCursor(NULL, IDC_CROSS));

		ShowCurves();
		//InvalidateRect(m_hWnd, &m_rtWindow, TRUE);
	}
	return 0;
}

int CChart::OnRightButtonDown(int x,int y)
{
	HDC hdc = GetDC( m_hWnd);
	if(IsMouseInChart(x, y))
	{
		// 设置右键按下状态
		m_bRightButtonDown = true;

		// 设定鼠标指针
		SetCursor(LoadCursor(NULL,IDC_IBEAM));

	    // 记录此时鼠标右键按下的位置
		m_ptRightButtonDown.x = x;
		m_ptRightButtonDown.y = y;


		m_memDCChart   =  CreateCompatibleDC(hdc);
		m_bmpInDCChart =  CreateCompatibleBitmap(hdc, GetChartWidth(), GetChartHeight());
		SelectObject( m_memDCChart, m_bmpInDCChart);

		// 记录右键按下时显示在图表窗的位图，用于实现右键选择
		m_memDCRightButtonDown = CreateCompatibleDC( hdc );
		m_bmpRightButtonDown   = CreateCompatibleBitmap( hdc, GetChartWidth() , GetChartHeight());
		SelectObject( m_memDCRightButtonDown, m_bmpRightButtonDown);
		BitBlt( 
			m_memDCRightButtonDown,
			0, 0, 
			GetChartWidth() , GetChartHeight(),
			hdc, 
			m_rtChart.left,
			m_rtChart.top,
			SRCCOPY);

	}else
	{
		m_ptRightButtonDown.x = -1;
		m_ptRightButtonDown.y = -1;
	}

	ReleaseDC(m_hWnd, hdc);
	return 0;
}

int CChart::OnRightButtonUp(int x, int y)
{
	HDC hdc = GetDC( m_hWnd);

	// 设置右键按下状态
	m_bRightButtonDown = false;
	if(IsMouseInChart(x, y))
	{
		// 设定鼠标指针
		SetCursor(LoadCursor(NULL, IDC_CROSS)) ;

		// 释放memDC和Bitmap
		DeleteObject( m_bmpRightButtonDown ) ;
		DeleteDC( m_memDCRightButtonDown ) ;

		DeleteObject( m_bmpInDCChart);
		DeleteDC( m_memDCChart);

		if(m_memDCChart == NULL)
			MessageBox(NULL,"TEST","TEST",MB_OK);

		if(m_ptRightButtonDown.x != -1)
		{
			double xStart,xEnd;
			if(x>m_ptRightButtonDown.x)
			{
				xStart = sx2rx(m_ptRightButtonDown.x);
				xEnd   = sx2rx(x);
			}else
			{
				xStart = sx2rx(x);
				xEnd   = sx2rx(m_ptRightButtonDown.x);
			}
			SetXRange(xStart, xEnd);
		}

		ShowCurves();
		//InvalidateRect( m_hWnd, &m_rtWindow, TRUE);	
	}

	ReleaseDC(m_hWnd, hdc);
	return 0;
}

int CChart::OnMouseMove(int x, int y)
{
	HDC hdc = GetDC( m_hWnd);

	if(IsMouseInChart(x, y))
	{
		// 设定鼠标形状
		if(m_bLeftButtonDown)        // 左键拖动
		{
			SetCursor(LoadCursor(NULL, IDC_HAND));

			if( m_ptLeftButtonDown.x != -1)
			{
				Move( -(x - m_ptLeftButtonDown.x) * GetXPerPix() ,0);
				m_ptLeftButtonDown.x = x;
				m_ptLeftButtonDown.y = y;

				// 绘制曲线和坐标系
				ShowCurves();
			}
		}
		else if(m_bRightButtonDown)  // 右键拖动
		{
			// 设定鼠标形状
			SetCursor(LoadCursor(NULL, IDC_IBEAM));

			if(m_ptRightButtonDown.x != -1)
			{
				// 设定为开始读取的背景图
				BitBlt( m_memDCChart,
					0,
					0,
					GetChartWidth(),
					GetChartHeight(),
					m_memDCRightButtonDown,
					0,0,SRCCOPY );
				// 绘制选择框
				HDC memDC;
				HBITMAP hBitmap,hOldBitmap;
				UINT  width  = abs(x - m_ptRightButtonDown.x) ;
				UINT  height = abs(m_rtChart.bottom - m_rtChart.top) ; 

				memDC      = CreateCompatibleDC(hdc);
				hBitmap    = CreateCompatibleBitmap(hdc,width, height);
				hOldBitmap = (HBITMAP) SelectObject(memDC, hBitmap);

				// 在内存设备上绘制一个矩形
				HPEN    newpen    =   CreatePen(PS_SOLID, m_nLineWidthOfScaleSelectBorder, m_clrScaleSelectBorder);
				HPEN    oldpen    =   (HPEN)SelectObject(memDC , newpen);
				HBRUSH  newbrush  =   CreateSolidBrush( m_clrScaleSelectBg ) ;
				HBRUSH  oldbrush  =   (HBRUSH)SelectObject(memDC, newbrush);

				Rectangle(memDC, 0, 0, width, height ) ;
		

				BLENDFUNCTION bf;
				bf.BlendOp              =  AC_SRC_OVER ;
				bf.BlendFlags           =  0 ;
				bf.SourceConstantAlpha  =  m_nTransRate;
				bf.AlphaFormat          =  0 ;

				// 在m_memDCChart中alpha混合
				if(x>m_ptRightButtonDown.x)
					AlphaBlend( m_memDCChart ,
								m_ptRightButtonDown.x - m_rtChart.left, 0 , width , height, 
								memDC, 
								0 , 0, width , height, 
								bf);
				else
					AlphaBlend( m_memDCChart ,
								x - m_rtChart.left, 0 , width , height, 
								memDC, 
								0 , 0, width , height, 
								bf);

				// 将混合后的结果显示在窗口上
				BitBlt( hdc,
					m_rtChart.left,
					m_rtChart.top,
					GetChartWidth(),
					GetChartHeight(),
					m_memDCChart,
					0,0,SRCCOPY );
				

				SelectObject( memDC, oldpen);
				DeleteObject( newpen);
				SelectObject( memDC, oldbrush);
				DeleteObject( newbrush);

				DeleteObject( hBitmap);
				DeleteDC( memDC);

			}
			
		}
		else                         // 鼠标指针移动
			SetCursor(LoadCursor(NULL, IDC_CROSS));

	}

	ReleaseDC(m_hWnd, hdc);
	return 0;
}
int CChart::OnMouseWheel(int zDelta,int x, int y)
{
	if(IsMouseInChart(x,y))
	{
		if(zDelta > 0)
			ScaleX(0.85);
		if(zDelta < 0)
			ScaleX(1/0.85);
		
		ShowCurves();
		// InvalidateRect( m_hWnd, &m_rtWindow, TRUE);
	}
	return 0;
}