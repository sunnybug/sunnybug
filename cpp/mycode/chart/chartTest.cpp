// chartTest.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "chartTest.h"
#include "DispChart.h"
#include <vector>
#include <math.h>
#define MAX_LOADSTRING 100



// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

VOID CALLBACK TimerProc( HWND hwnd,  UINT uMsg, UINT_PTR idEvent,DWORD dwTime);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CHARTTEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CHARTTEST);

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释: 
//
//    仅当希望在已添加到 Windows 95 的
//    “RegisterClassEx”函数之前此代码与 Win32 系统兼容时，
//    才需要此函数及其用法。调用此函数
//    十分重要，这样应用程序就可以获得关联的
//   “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CHARTTEST);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(CreateSolidBrush(RGB(0xcc,0xcc,0xcc)));
	wcex.lpszMenuName	= (LPCTSTR)IDC_CHARTTEST;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HANDLE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, unsigned, WORD, LONG)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static int cxClient, cyClient;

	static std::vector<double> data[3];

	//static CDAQContAI ai("ai0","Dev1/ai0",1,100,10000,100);
    static CChart chart(hWnd);

	

	chart.SetClrScaleSelectBg(RGB(0,0,0));

	switch (message) 
	{
	case WM_CREATE:
		{
		
			/*
			// 设定时器 
			SetTimer(hWnd,1,1000,NULL);

			
			chart.SetGridColor(RGB(255,0,0));
			chart.SetGriddx(10);
			chart.SetClrLabel(RGB(0,0,255));
			chart.SetXLabel("t/min");
			chart.SetYLabel("V/v");
			chart.SetRulerXFormat("%.2f");
			chart.SetGriddy( 0.01);
			chart.SetXRange( 0, 100);
			chart.SetYRange( -5, 5);
	
		*/
			chart.SetYRange(-10,10);
			chart.SetGriddy(2);
			
			int length = 1000;
			double amp = 5;
			for(int i=0;i<length;i++)
			{
				double t = (20 * 3.1415926 / length) * i;
				double y1 = amp*sin(t);
				double y2 = (amp/2)*cos(t);
				data[0].push_back(t);
				data[1].push_back(y1);
				data[2].push_back(y2);
			}
			 chart.AddMCurves(data,3);
			 chart.SetCurveColor(0,RGB(0,0,0));
			
            /*

			chart.AddACurve(&ai.m_Time);
			chart.AddMCurves(ai.m_Data,5);

			ai.StartTask();*/

		}
		break;
	case WM_TIMER:
		chart.Move(1,0);
		chart.ShowCurves();
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_LBUTTONDOWN:
		
		chart.OnLeftButtonDown(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
		chart.OnLeftButtonUp(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_RBUTTONDOWN:
		chart.OnRightButtonDown(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_RBUTTONUP:
		chart.OnRightButtonUp(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_MOUSEMOVE:
		chart.OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		break;
	case 0x020A:
		{
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient( hWnd,&pt );
		    chart.OnMouseWheel(wParam, pt.x, pt.y);
		}
		break;
	case WM_SIZE:
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码..
		RECT rect;
		GetClientRect(hWnd,&rect);

		chart.SetChartDC(hdc);
 
		chart.SetChartAndWindowPosition(rect);
		//chart.ShowAt(rect);
		//chart.ShowAt(rect);
		 chart.ShowCurves();
		

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		KillTimer(hWnd,1);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

