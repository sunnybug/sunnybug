#ifndef _DISPCHART_H
#define _DISPCHART_H
#include <windows.h>
#include <vector>
#include <string>

// 图表基类:用于绘制坐标系和曲线
class CChartBase
{
protected:
	HDC       m_hChartDC;       // 图表绘制的DC
	RECT      m_rtWindow;       // 坐标窗口,其中包含图表显示区和坐标标尺,轴标等    
	RECT      m_rtChart;        // 图标显示区(依赖域m_rtWindow)

	// 坐标显示边界
	double    m_xStart;         // 实数域内x轴起点
	double    m_xEnd;           //         x轴终点
	double    m_yStart;         //         y轴起点
	double    m_yEnd;           //         y轴终点

	// 显示图表区
	COLORREF  m_clrChartBg;     // 显示区背景色
	COLORREF  m_clrChartBorder; //       边框色

	// 网格
	bool      m_bGridOn;        // 控制网格是否显示
	double    m_dxGrid;         // 网格单元宽
	double    m_dyGrid;         // 网格单元高
	COLORREF  m_clrGrid;        // 网格线颜色

	// 坐标轴,轴标,标尺
	char      m_xLabel[20];
	char      m_yLabel[20];
	bool      m_bxLabelOn; 
	bool      m_byLabelOn;

	bool      m_bxRulerOn;
	bool      m_byRulerOn;
	char      m_szRulerXFormat[128];
	char      m_szRulerYFormat[128];

	COLORREF  m_clrLabel;      // 轴标文字和刻度文字颜色

	// 文字字体
	LOGFONT     m_logFont;

	// 曲线数据

public:

	// 辅助函数
	void SetChartDC(HDC hdc);
	HDC  GetChartDC() const ;
	void SetChartWindowPosition(RECT rect);
	void SetChartWindowPosition(int left, int top, int right, int bottom);
	RECT GetChartWindowPosition() const;
	void SetChartPosition(RECT rect);
	void SetChartPosition(int left, int top, int right, int bottom);

	RECT GetChartPosition() const;
	int  GetChartWidth()const  { return m_rtChart.right - m_rtChart.left ; };
	int  GetChartHeight()const { return m_rtChart.bottom - m_rtChart.top ; };
	int  GetChartWindowWidth() const { return m_rtWindow.right - m_rtWindow.left; };
	int  GetChartWindowHeight() const{ return m_rtWindow.bottom - m_rtWindow.top; };

	// 坐标显示边界
	double SetXStart(double xStart);
	double SetXEnd(double xEnd);
	void   SetXRange(double xStart,double xEnd);
	double SetYStart(double yStart);
	double SetYEnd(double yEnd);
	void   SetYRange(double yStart,double yEnd);
	double GetXStart()const;
	double GetXEnd()const;
	double GetYStart()const;
	double GetYEnd()const;
    
	// 显示图表区
	void      SetClrChartBg(COLORREF clr);
	COLORREF  GetClrChartGb()const;
	void      SetClrChartBorder(COLORREF clr);
	COLORREF  GetClrChartBorder()const;

	// 网格
	double   SetGriddx(double dxGrid);              // 设置网格宽度,返回前一个值
	double   SetGriddy(double dyGrid);              // 设置网格高度,返回前一个值
	void     SetGridxy(double dxGrid,double dyGrid);
	COLORREF SetGridColor(COLORREF color);          // 设置网格颜色,返回前一个值
	void SetGridOn();                               // 设置网格为显示状态
	void SetGridOff();                              // 设置网格为关闭状态

	double   GetGriddx()const;
	double   GetGriddy()const;
	COLORREF GetGridColor()const;
	bool     GetGridStatus()const;

	// 坐标轴,轴标,标尺
	void    SetXLabel(const char* xLabel);
	void    SetYLabel(const char* yLabel);
	void    SetXLabelOn();
	void    SetXLabelOff();
	BOOL    GetXLabelStatus()const;
	void    SetYLabelOn();
	void    SetYLabelOff();
	BOOL    GetYLabelStatus()const;

	void    SetXRulerOn();
	void    SetXRulerOff();
	BOOL    GetXRulerStatus()const;
	void    SetYRulerOn();
	void    SetYRulerOff();
	BOOL    GetYRulerStatus()const;

	void    SetRulerXFormat(const char str[]) { strcpy(m_szRulerXFormat, str);};
	void    SetRulerYFormat(const char str[]) { strcpy(m_szRulerXFormat, str);};

	void     SetClrLabel(COLORREF clr);
	COLORREF GetClrLabel()const;

	// 字体
	void    SetLogFont(LOGFONT logFont);
	LOGFONT GetLogFont()const;


	// 每一个象素所代表的实数值
	double GetYPerPix() const;
	double GetXPerPix() const;

protected:
	int  ShowChartBg();
	int  ShowGrid();
	int  ShowRuler();
	int  ShowLabel();

public:
	CChartBase();
	~CChartBase();

	int ShowAt(int left, int top, int right, int bottom); // 在rect中显示该图标窗口,外部最好用该函数
	int ShowAt(RECT rect);
	int Show();                                           // 通过设置各种参数显示

	// 坐标转换 r--real  s--screen 2--to
	int rx2sx(double rx);  
	int ry2sy(double ry);
	double sx2rx(int sx);
	double sy2ry(int sy);

	// 坐标变换
	void Move(double drx, double dry);                   // 坐标系平移
	void ScaleCenter(double times);                      // 坐标以坐标框的中心放缩
	                                                     // times>1时,显示范围扩大,起到缩小的作用
	                                                     // times<1时,显示范围缩小,起到放大的作用
	void ScaleX(double times);                           // X轴范围缩放（以x轴中心）
	void ScaleY(double times);                           // Y轴范围缩放（以y轴中心）
};

class CChart:public CChartBase
{
private:
	HWND       m_hWnd;           // 图表所在的窗口，该窗口可以处理消息（用于实现坐标变换等）
	HDC        m_memDCWindow;    // 存储整个绘图窗用的内存句柄
	HBITMAP    m_bmpInDCWindow;  
	HDC        m_memDCChart;     // 存储chart的内存设备句柄
    HBITMAP    m_bmpInDCChart;   // 图表所对应的位图句柄

	// 曲线数据
public:
	
	std::vector<std::vector<double>*> m_pvCurvesData;             // 向量地址的向量,所以可以链接上任何以vector<double>类型的数据
	std::vector<double>               m_vCurvesScale;             // 各条曲线数字的缩放因子,默认为1,可以设置
	std::vector<std::string>         m_vsCurvesName;              // 各条曲线的名称,默认依次为x,y1,y2,y3..

	void   AddACurve(std::vector<double>* pvCurveData);          // 添加一条曲线，第一条为x轴
	void   AddMCurves(std::vector<double> pvCuveData[], int n);  // 添加n条曲线

	double GetDataAt(int m, int n);                    // 取数据(考虑缩放)，m＝0时为x轴数据，m＝1,2,3..为曲线1,2,3..的y数据
	long   GetNumCurves();                             // 有多少条曲线
	long   GetNumDataInCurve(int m);                   // 曲线m中有多少个数据

	void   DeleteLastCurve();                          // 删除最后一条曲线
	void   DeleteAllCurves();                          // 删除所有曲线

	void   SetCurveScale(int m, double scale);          // 设定m号曲线的缩放因子
	double GetCurveScale(int m);                        // 获取m号曲线的缩放因之

	void   SetCurveName (int m, char * name) ;                       // 设定m号曲线的名称
    #define GetCurveName(m) (m_vsCurvesName.at(m).c_str())      // 获取m号曲线的名称

	long    FindDataInX(double x);        // 查找x的位置 （对于浮点数 x1=<x<x2 则取x1的位置） 二分法查找,没找到返回-1
	                                      // X随下标的增长而增长

	// 曲线属性
private:
	std::vector<bool>       m_vbCurvesShow;         // 控制曲线是否显示,默认为显示(true)
	std::vector<int>        m_vCurvesLineWidth ;    // 各条曲线的线宽,[0]时无意义;[1]..[n] 分别为曲线的线宽，默认为1
	std::vector<COLORREF>   m_vCurvesColor;         // 各条曲线的颜色,[0]时无意义;[1]..[n] 分别为曲线的颜色

public:
	void SetCurveLineWidth(int m, int width);       // 为m号曲线设置线宽
	void SetCurveColor  (int m, COLORREF clr);      // 为m号曲线设置颜色
	void SetCurveStatus (int m, bool bShow);        // 为m号曲线设置显示属性,true显示,false隐藏
	int  GetCurveLineWidth(int m);                  // 获得m号曲线线宽,错误返回-1
	COLORREF GetCurveColor(int m);                   // 获得m号曲线颜色,错误返回-1
	bool GetCurveStatus (int m);                    // 获得m号曲线显示属性

	// 曲线绘制和坐标系
	void SetChartAndWindowPosition(RECT rect);         // 同时改变m_rtChart和m_rtWindows
	void SetChartAndWindowPosition (int left, int top, int right, int bottom);

	void ShowCurves();


public:
	CChart(HWND hWnd = NULL);
	~CChart();

	// 窗口消息处理:以下函数用于特定的窗口消息
	// 左键拖动，坐标平移（限于x轴）
	// 右键拖动，画出一个矩形，选择特定的x轴范围并放大
	// 左键按下，提示当前x轴对应下所有曲线上的值，并绘出坐标提示窗口和坐标定位线
	// 鼠标滚珠，上滚放大，下滚缩小

	// 鼠标光标定义：在chart区域移动时为十字型，其他区域为指针
	//               左键拖动时为手型，右键拖动时为放大镜
	//               左键显示作标提示窗口时为十字型
private:
	bool   m_bLeftButtonDown;             // 左键按下为true
	bool   m_bRightButtonDown;            // 右键按下为ture
	POINT  m_ptLeftButtonDown;            // 记录前一次左键按下的坐标,(-1,-1)时表示，按下的位置不在chart内
	POINT  m_ptRightButtonDown;           // 记录前一次右键按下的坐标,(-1,-1)时表示，按下的位置不在chart内


    // 用于右键拖动一个范围缩放的成员变量和成员函数
	HBITMAP m_bmpRightButtonDown;             // 右键按下时整个显示区的位图
	HDC     m_memDCRightButtonDown;
	COLORREF m_clrScaleSelectBg;              // 右键选择半透明区域背景色
	COLORREF m_clrScaleSelectBorder;          //               边框色
	int      m_nLineWidthOfScaleSelectBorder; //               边框宽度
	int      m_nTransRate;                    //               背景透明度0---255

	// 坐标定位线
	COLORREF m_clrCoordinateLine;             // 坐标定位线颜色
	int      m_nLineWidthCoordinateLine;      // 坐标定位线宽度

	// 坐标显示窗口
	COLORREF m_clrCoordWindowBg;               // 坐标显示窗口的背景色
	COLORREF m_clrCoordWindowBorder;           // 坐标显示窗口的边框色
	int      m_nLineWidthOfCoordWindowBorder;  //               边框线宽
	int      m_nCoordWindowTransRate;          //               透明度 0---255
	LOGFONT  m_lfCoordWindow;                  // 坐标显示窗口上的字体


public:
	void SetClrScaleSelectBg(COLORREF clr) { m_clrScaleSelectBg = clr;};
	void SetClrScaleSelectBorder( COLORREF clr){ m_clrScaleSelectBorder = clr;};
	void SetLineWidthScaleSelectBorder( int width) {m_nLineWidthOfScaleSelectBorder = width;};
	void SetTransRate(int rate){ m_nTransRate = rate;};
	void SetClrCoordinateLine(COLORREF clr) { m_clrCoordinateLine = clr; };
	void SetLineWidthCoordinateLine( int width) { m_nLineWidthCoordinateLine = width; };


	COLORREF GetClrScaleSelectBg() const { return m_clrScaleSelectBg;};
	COLORREF GetClrScaleSelectBorder() const { return m_clrScaleSelectBorder;};
	COLORREF GetClrCoordinateLine() const { return m_clrCoordinateLine; };
	int      GetLineWidthScaleSelectBorder()const{ return m_nLineWidthOfScaleSelectBorder;};
	int      GetTransRate()const { return m_nTransRate;};
	int      GetLineWidthCoordinateLine()const { return m_nLineWidthCoordinateLine;};

	// 坐标显示窗口
	void  SetClrCoordWindowBg(COLORREF clr) { m_clrCoordWindowBg = clr ; } ;
	void  SetClrCoordWindowBorder(COLORREF clr) { m_clrCoordWindowBorder = clr ; } ;
	void  SetLineWidthCoordWindowBorder(int width) { m_nLineWidthOfCoordWindowBorder = width ; } ;
	void  SetCoordWindowTransRate(int rate) {  m_nCoordWindowTransRate = rate ; } ;
	void  SetCoordWindowLogFont(LOGFONT lf) {  m_lfCoordWindow = lf ; } ;

	COLORREF GetClrCoordWindowBg()const { return  m_clrCoordWindowBg ; } ;
	COLORREF GetClrCoordWindowBorder()const { return m_clrCoordWindowBorder ; } ;
	int      GetLineWidthCoordWindowBorder()const { return m_nLineWidthOfCoordWindowBorder ; } ;
	int      GetCoordWindowTransRate()const { return m_nCoordWindowTransRate ; } ; 
	LOGFONT  GetCoordWindowLogFont()const { return m_lfCoordWindow; } ;

public:
	int  OnLeftButtonDown(int x, int y);  // WM_LBUTTONDOWN x,y为鼠标在窗口上的位置
	int  OnLeftButtonUp(int x, int y);    // WM_LBUTTONUP
	int  OnRightButtonDown(int x, int y); // WM_RBUTTONDOWN
	int  OnRightButtonUp(int x, int y);   // WM_RBUTTONUP
	int  OnMouseMove(int x, int y);       // WM_MOUSEMOVE
	int  OnMouseWheel(int zDelta, int x, int y);        // WM_MOUSEWHEEL               

	// 辅助窗口消息处理
	bool IsMouseInChart(int x, int y);    // 判断pt（x，y）是否在chart窗口中
	bool IsMouseInChart(POINT pt); 


};
#endif