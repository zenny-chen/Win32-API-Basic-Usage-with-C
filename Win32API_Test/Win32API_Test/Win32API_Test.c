// Win32API_Test.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Win32API_Test.h"
#include <CommCtrl.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_LOADSTRING 100

// 全局变量:
static HINSTANCE hInst;             // 当前实例
static HWND sMainWindow;            // main window handler
static HWND sClickMeButton;         // `click me` button handler
static HWND sCheckBox;              // check box control
static HWND sStaticText;            // static control
static HWND sEditText;              // edit control
static HWND sComboBox;              // combo box control
static HWND sGroupBox;              // group box control
static HWND sRadioButtons[3];       // radio button controls
static HWND sListBox;               // list box control
static HWND sListView;              // list view control
static HBRUSH sBackgroundBrush;     // background brush

static WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
static WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

static LONG sWindowWidth, sWindowHeight;

static const WCHAR* const sRadioButtonNames[ARRAYSIZE(sRadioButtons)] = {
    L"Radio 1", L"Radio 2", L"Radio 3"
};

static const WCHAR* const sListBoxItems[3] = {
    L"list box item 1",
    L"list box item 2",
    L"list box item 3",
};

static enum WindowColorStyle {
    WindowColorStyle_NONE,
    WindowColorStyle_GRAY,
    WindowColorStyle_DEFAULT
} sWindowColorStyle = WindowColorStyle_NONE;

static int GetControlWidth(int width)
{
    return (int)((float)sWindowWidth / 640.0f * (float)width + 0.5f);
}

static int GetControlHeight(int height)
{
    return (int)((float)sWindowHeight / 480.0f * (float)height + 0.5f);
}

static HINSTANCE GetInstanceFromWindow(HWND hWnd)
{
    return (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
}

static void MyTimerProc(HWND hWnd, UINT arg1, UINT_PTR arg2, DWORD arg3)
{
    KillTimer(hWnd, 1);
}

// “关于”框的消息处理程序。
static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

static int GetSelectedRadioButtonIndex(void)
{
    int const radioButtonCount = (int)ARRAYSIZE(sRadioButtons);
    for (int i = 0; i < radioButtonCount; i++)
    {
        LRESULT const state = SendMessage(sRadioButtons[i], BM_GETSTATE, 0UL, 0UL);
        if (state == BST_CHECKED)
            return i;
    }

    return 0;
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
static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_RBUTTONDOWN:
        SetTimer(hWnd, 1, 10, MyTimerProc);
        sWindowColorStyle = WindowColorStyle_GRAY;
        RedrawWindow(hWnd, NULL, 0, RDW_INVALIDATE | RDW_UPDATENOW);
        break;

    case WM_LBUTTONDOWN:
        sWindowColorStyle = WindowColorStyle_DEFAULT;
        RedrawWindow(hWnd, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
        SetWindowText(sStaticText, L"This is a static control!");
        break;

    case WM_COMMAND:
    {
        DWORD wmId = LOWORD(wParam);

        if ((HWND)lParam == sClickMeButton)
        {
            DWORD notifCode = HIWORD(wParam);
            if (notifCode == BN_CLICKED)
            {
                WCHAR msgBuf[1024];

                WCHAR editContent[512];
                GetWindowText(sEditText, editContent, 512);

                LRESULT ItemIndex = SendMessage(sComboBox, CB_GETCURSEL, 0, 0);
                WCHAR itemContent[64];
                SendMessage(sComboBox, CB_GETLBTEXT, ItemIndex, (LPARAM)itemContent);

                LRESULT const checkState = SendMessage(sCheckBox, BM_GETSTATE, 0UL, 0UL);
                int const radioButtonIndex = GetSelectedRadioButtonIndex();

                wsprintf(msgBuf, L"Clicked Me!!\nThe edit control content is: %s.\nCombo box item content: %s.\nIs checked? %s\nSelected radio Button: %s\n", 
                    editContent, itemContent, checkState == BST_CHECKED ? L"YES" : L"NO", sRadioButtonNames[radioButtonIndex]);
                SetWindowText(sStaticText, L"Button clicked!");

                MessageBox(hWnd, msgBuf, L"Notice", MB_OK);
            }
            else
                return DefWindowProc(hWnd, message, wParam, lParam);
        }
        else if ((HWND)lParam == sListBox)
        {
            DWORD notifCode = HIWORD(wParam);
            if (notifCode == LBN_DBLCLK)
            {
                LRESULT selectedIndex = SendMessage(sListBox, LB_GETCURSEL, 0U, 0U);
                if (selectedIndex != LB_ERR)
                {
                    WCHAR strBuf[128];
                    wsprintf(strBuf, L"Selected list item: %s\n", sListBoxItems[selectedIndex]);
                    MessageBox(hWnd, strBuf, L"Notice", MB_OK);
                }
            }
            else
                return DefWindowProc(hWnd, message, wParam, lParam);
        }

        // 分析菜单选择:
        switch (wmId)
        {
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

    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, RGB(0, 0, 0));

        COLORREF bkgColor;
        if (sWindowColorStyle == WindowColorStyle_GRAY)
            bkgColor = RGB(109, 109, 109);
        else
            bkgColor = RGB(255, 255, 255);

        SetBkColor(hdcStatic, bkgColor);

        if (sBackgroundBrush != NULL)
            DeleteObject(sBackgroundBrush);

        sBackgroundBrush = CreateSolidBrush(bkgColor);

        // Because WM_PAINT message is ahead of WM_CTLCOLORSTATIC,
        // sWindowColorStyle should be cleared here
        sWindowColorStyle = WindowColorStyle_NONE;

        return (INT_PTR)sBackgroundBrush;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 在此处添加使用 hdc 的任何绘图代码...

        switch (sWindowColorStyle)
        {
        case WindowColorStyle_GRAY:
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_GRAYTEXT + 1));
            break;

        case WindowColorStyle_DEFAULT:
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            break;
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

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
static ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32APITEST));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WIN32APITEST);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 将实例句柄存储在全局变量中

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL)
        return FALSE;

    sMainWindow = hWnd;

    RECT windowRect = { 0 };
    if (GetWindowRect(hWnd, &windowRect))
    {
        sWindowWidth = windowRect.right - windowRect.left;
        sWindowHeight = windowRect.bottom - windowRect.top;
    }

    int x = 10, y = 10;
    int const clickMeWidth = GetControlWidth(70);
    int const controlHeight = GetControlHeight(25);

    sClickMeButton = CreateWindow(
        WC_BUTTON,          // Predefined class; Unicode assumed 
        L"Click Me",        // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        x,                  // x position 
        y,                  // y position 
        clickMeWidth,       // Button width
        controlHeight,      // Button height
        hWnd,               // Parent window
        NULL,               // No menu.
        hInstance,
        NULL);              // Pointer not needed.

    x += clickMeWidth + 20;

    sCheckBox = CreateWindow(
        WC_BUTTON,
        L"Check Box",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
        x,
        y,
        clickMeWidth,
        controlHeight,
        hWnd,
        NULL,
        hInstance,
        NULL);

    x += clickMeWidth + 20;

    int const staticTextWidth = GetControlWidth(100);
    sStaticText = CreateWindow(
        WC_STATIC,
        L"This is a static control!",
        WS_VISIBLE | WS_CHILD | SS_LEFTNOWORDWRAP,
        x,
        y,
        staticTextWidth,
        controlHeight,
        hWnd,
        NULL,
        hInstance,
        NULL);

    x += staticTextWidth + 20;

    int const editControlWidth = GetControlWidth(120);
    sEditText = CreateWindow(
        WC_EDIT,
        L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
        x,
        y,
        editControlWidth,
        controlHeight,
        hWnd,
        NULL,
        hInstance,
        NULL);

    x = 10;
    y += controlHeight + 20;

    int const comboBoxWidth = GetControlWidth(60);
    int const comboBoxHeight = GetControlHeight(200);

    sComboBox = CreateWindow(
        WC_COMBOBOX, 
        L"",
        WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS,
        x, 
        y,
        comboBoxWidth,
        comboBoxHeight,
        hWnd, 
        NULL, 
        hInstance,
        NULL);

    // Add string to combobox.
    SendMessage(sComboBox, CB_ADDSTRING, 0, (LPARAM)L"Item1");
    SendMessage(sComboBox, CB_ADDSTRING, 0, (LPARAM)L"Item2");
    SendMessage(sComboBox, CB_ADDSTRING, 0, (LPARAM)L"Item3");

    // Send the CB_SETCURSEL message to display an initial item in the selection field
    UINT const selectedItem = 0;
    SendMessage(sComboBox, CB_SETCURSEL, selectedItem, 0);

    x += comboBoxWidth + 20;

    int const groupBoxWidth = GetControlWidth(200);
    int const groupBoxHeight = GetControlHeight(50);

    sGroupBox = CreateWindow(
        WC_BUTTON,
        L"Radio group",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
        x,
        y,
        groupBoxWidth,
        groupBoxHeight,
        hWnd,
        NULL,
        hInstance,
        NULL);

    int const radioButtonCount = (int)ARRAYSIZE(sRadioButtons);
    int const radioButtonMargin = 10;
    int const radioButtonWidth = (groupBoxWidth - (radioButtonCount + 1) * radioButtonMargin) / radioButtonCount;
    int const radioButtonHeight = groupBoxHeight / 2;
    int radioX = radioButtonMargin;
    int const radioY = (groupBoxHeight - radioButtonHeight) / 2 + GetControlHeight(5);
    for (int i = 0; i < radioButtonCount; i++)
    {
        sRadioButtons[i] = CreateWindow(
            WC_BUTTON,
            sRadioButtonNames[i],
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            radioX,
            radioY,
            radioButtonWidth,
            radioButtonHeight,
            sGroupBox,
            NULL,
            hInstance,
            NULL);

        radioX += radioButtonWidth + radioButtonMargin;
    }
    // select radio 1 by default
    SendMessage(sRadioButtons[0], BM_SETCHECK, BST_CHECKED, 0UL);

    y += groupBoxHeight + 30;
    int const listControlMargin = GetControlWidth(20);
    x = listControlMargin;
    int const listBoxWidth = (sWindowWidth - 3 * listControlMargin) / 2;
    int const listBoxHeight = sWindowHeight - y - 100;

    sListBox = CreateWindow(
        WC_LISTBOX,
        NULL,
        WS_TABSTOP | WS_VISIBLE | WS_BORDER | WS_CHILD | LBS_USETABSTOPS | LBS_HASSTRINGS | LBS_NOTIFY | WS_VSCROLL,
        x,
        y,
        listBoxWidth,
        listBoxHeight,
        hWnd,
        NULL,
        hInstance,
        NULL);

    size_t listBoxItemCount = sizeof(sListBoxItems) / sizeof(sListBoxItems[0]);
    for(size_t i = 0; i < listBoxItemCount; i++)
        SendMessage(sListBox, LB_ADDSTRING, 0U, (LPARAM)sListBoxItems[i]);
    
    x += listBoxWidth + listControlMargin;

    sListView = CreateWindow(
        WC_LISTVIEW,
        NULL,
        WS_TABSTOP | WS_VISIBLE | WS_BORDER | WS_CHILD | LVS_OWNERDATA | WS_VSCROLL | LVS_ALIGNLEFT | LVS_LIST | LVS_SINGLESEL,
        x,
        y,
        listBoxWidth,
        listBoxHeight,
        hWnd,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32APITEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32APITEST));

    MSG msg;

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

