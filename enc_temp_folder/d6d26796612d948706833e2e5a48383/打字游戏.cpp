// 打字游戏.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "warren.h"
#include <mmsystem.h>

#define MAX_LOADSTRING 100

#pragma comment(lib,"winmm.lib")

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int left = 100, top = 20, right = left + 250, bottom = top + 400;   //定义变量设置字母的下落区
char c1, c2, c3;
COLORREF Clr1 = RGB(255, 0, 0), Clr2 = RGB(0, 255, 0), Clr3 = RGB(0, 0, 255), Clr4 = RGB(0, 0, 0);
int k1;
int x = -1, y = -1;                                                 
int iScoring = 0, iFail = 0;                                        
int gameover = 0;                  
int tools=1;

void DrawBk(HDC hdc, int x1, int y1, int x2, int y2)
{
    Rectangle(hdc, x1, y1, x2, y2);   //绘制一个矩形标识字母下落范围
    COLORREF Color1 = RGB(rand() % 255, rand() % 255, rand() % 255);//字体颜色随机
    char s[100] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    SetTextColor(hdc, Color1);
    TextOutA(hdc, x1 + 5, y2 - 25, s, strlen(s));                   //矩形下面一行显示一排字母
}

void ShowScoring(HDC hdc, int x, int y, int iScoring, int iFail)
{
    char szTemp[32];
    TextOutA(hdc, x, y, "Current Score：", strlen("Current Score："));
    y += 20;
    sprintf_s(szTemp, "%d", iScoring);
    TextOutA(hdc, x, y, szTemp, strlen(szTemp));
    y += 20;
    TextOutA(hdc, x, y, "Current Failure：", strlen("Current Failure："));
    y += 20;
    sprintf_s(szTemp, "%d", iFail);
    TextOutA(hdc, x, y, szTemp, strlen(szTemp));
    y += 20;
    TextOutA (hdc, x, y, "Current Tools：", strlen("Current Tools："));
    y += 20;
    sprintf_s(szTemp, "%d", tools);
    TextOutA(hdc, x, y, szTemp, strlen(szTemp));
   
}

void GameOver(HDC hdc, int x, int y)
{
    COLORREF OldColor, NewColer = RGB(rand() % 255, rand() % 255, rand() % 255);
    OldColor = SetTextColor(hdc, NewColer);                         //设置文本颜色为红色
    TextOutA(hdc, x, y, "Game Over！", strlen("Game Over！"));
    SetTextColor(hdc, OldColor);                                    //恢复原文本颜色
}

void Fire(HDC hdc, int x, int y1, int y2)
{
    HPEN hOldPen, hNewPen = CreatePen(PS_DASHDOTDOT, 1, RGB(0, 255, 0));    //创建光束绿色
    hOldPen = (HPEN)SelectObject(hdc, hNewPen);                     //选择光束画笔，保存旧画笔
    MoveToEx(hdc, x, y1, NULL);                                     //绘制起点
    LineTo(hdc, x, y2);                                             //绘制射击光束
    Sleep(100);                                                     //光束显示时间
    HPEN hNewPen2 = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));     //创建清除光束黑色画笔
    SelectObject(hdc, hNewPen2);                                    //选择清除光束画笔，保存旧画笔
    MoveToEx(hdc, x, y1, NULL);                                     //绘制起点
    LineTo(hdc, x, y2);                                             //清除设计光束
    SelectObject(hdc, hOldPen);                                     //恢复旧画笔
    DeleteObject(hNewPen);                                          //删除画笔
    DeleteObject(hNewPen2);                                         //删除画笔
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    PlaySound(TEXT("1.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    
    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {

        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
    }

    return (int) msg.wParam;
}

void frozen(HDC hdc,HWND hWnd)                  //道具1
{

    Sleep(510);
TextOutA(hdc, 100, 20, "Frozen！", strlen("Frozen！"));
InvalidateRect(hWnd, 0, 0);
tools--;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;                                            //WNDCLASSEXW是窗口类，此句的意思是创建一个名为wcex的窗口对象，也就是我们要创建的窗口

    wcex.cbSize = sizeof(WNDCLASSEX);                           // wcex窗口的大小，一般都用sizeof获取标准大小

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(100, 100, 255));
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 将实例句柄存储在全局变量中

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_TIMER:                                                  //增加定时器消息处理
        switch (wParam)
        {
        case 1:       //接受定时器1消息，字母下落
            
            y = y + iScoring / 10 + 1;                              //根据当前分数计算下落速度
            if ((y > bottom - 40) && k1 != 3)                                    //判断是否落地&&不为炸弹
            {
                gameover = 1;                                       //游戏结束
                KillTimer(hWnd, 1);                                  //关掉字母下落定时器1
                SetTimer(hWnd, 2, 300, NULL);                       //建立显示“游戏结束！”的定时器2
            }
            else if ((y > bottom - 40)) {
                KillTimer(hWnd, 1);                                  //关掉字母下落定时器1
                SetTimer(hWnd, 1, 10, NULL);
            }

            InvalidateRect(hWnd, 0, 0);                             //刷新画面
            break;
        case 2:                                                     //接到定时器2消息，显示“游戏结束！”动画效果
            InvalidateRect(hWnd, 0, 0);                             //刷新画面
            break;
        }
        break;
    case WM_CHAR:                                                   //增加键盘输入消息处理
    {
        c2 = (wParam >= 'a' && wParam <= 'z') ? wParam + 'A' - 'a' : wParam;   //按键
        c3 = wParam;
        HDC hdc = GetDC(hWnd);
        Fire(hdc, left + 5 + (c2 - 'A') * 9 + 4, top, bottom);      //显示射击效果
        ReleaseDC(hWnd, hdc);
        if (c3 == ' ' && tools > 0) frozen(hdc, hWnd);
        if ((c2 == c1 && k1 == 2))                                               //判断是否击中||zhandan luo di
        {
            c1 = rand() % 26 + 'A';                                     //产生新下落字母
            k1 = rand() % 4;
            if (k1 < 2 && iScoring >= 10) k1 += rand() % (2 - k1);      //增加难度
            x = left + 5 + (c1 - 'A') * 9;                           //计算下落初始x坐标
            y = top;                                                 //计算下落初始y坐标
            iScoring = iScoring + 1;                                 //分数+1
            if (iScoring % 5 == 0) tools++;                             //每得5分加一个道具
        }
        else if (c1 == c2 && k1 != 3) {
            ++k1;
        }
        else if (c1 == c2 && k1 == 3) gameover = 1;
        else
            iFail = iFail + 1;                                      //未击中++
    }
    break;
    case WM_COMMAND:                                                //处理键盘输入结束
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case ID_START:  
                //增加新菜单项的ID
                if (gameover == 1)
                    KillTimer(hWnd, 2);
                gameover = 0;
                iScoring = 0;
                iFail = 0;
<<<<<<< HEAD
                tools = 1;
=======
>>>>>>> 53a1b38b105686b023da51364a550b8b9fe79327
                c1 = rand() % 26 + 'A';
                k1 = rand() % 4;
                x = left + 5 + (c1 - 'A') * 9;
                y = top;
                SetTimer(hWnd, 1, 10, NULL);                        //启动字母下落定时器
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            DrawBk(hdc, left, top, right, bottom);                  //显示背景
            ShowScoring(hdc, right + 20, top + 50, iScoring, iFail);//显示分数
            if (gameover)                                           //判断游戏是否结束
                GameOver(hdc, left + 80, top + 130);                //显示游戏结束
            else   if(y<=bottom-40)                                               //显示游戏画面
            {
                char szTemp[8];
                sprintf_s(szTemp, "%c", c1);                        //将下落字母转为字符串
                switch(k1)
                {
                case 0:SetTextColor(hdc, Clr1);break;
                case 1:SetTextColor(hdc, Clr2);break;
                case 2:SetTextColor(hdc, Clr3);break;
                default:SetTextColor(hdc, Clr4);
                }                                                    //三种颜色字母+一个“炸弹”
                TextOutA(hdc, x, y, szTemp, strlen(szTemp));        //显示下落字母
            }
            else         if ((y > bottom - 40) && k1 == 3)
            {
                c1 = rand() % 26 + 'A';                                     //产生新下落字母
                k1 = rand() % 4;
                if (k1 < 2 && iScoring >= 10) k1 += rand() % (2 - k1);      //增加难度
                x = left + 5 + (c1 - 'A') * 9;                           //计算下落初始x坐标
                y = top;                                                 //计算下落初始y坐标
                iScoring = iScoring + 1;                                 //分数+1
                if (iScoring % 5 == 0) tools++;
            }

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}