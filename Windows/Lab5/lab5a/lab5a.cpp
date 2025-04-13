// HelloWindowsDesktop.cpp
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <stdio.h>


#define ID_CLEAR 1
#define ID_MOVE 2
#define ID_SIZE 3
#define ID_PAINT 4

// Global variables

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");

// Stored instance handle for use in Win32 API calls such as FindResource
HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
   
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    // Store instance handle in our global variable
    hInst = hInstance;

    // The parameters to CreateWindowEx explained:
    // WS_EX_OVERLAPPEDWINDOW : An optional extended window style.
    // szWindowClass: the name of the application
    // szTitle: the text that appears in the title bar
    // WS_OVERLAPPEDWINDOW: the type of window to create
    // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
    // 500, 100: initial size (width, length)
    // NULL: the parent of this window
    // NULL: this application does not have a menu bar
    // hInstance: the first parameter from WinMain
    // NULL: not used in this application
   HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 100,
        NULL,
        NULL,
        hInstance,
        NULL
    ); 

   HWND clearButton = CreateWindow(
       L"BUTTON",  // Predefined class; Unicode assumed 
       L"CLEAR",      // Button text 
       WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
       10,         // x position 
       10,         // y position 
       100,        // Button width
       30,        // Button height
       hWnd,     // Parent window
       (HMENU)ID_CLEAR,       // No menu.
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
       NULL);      // Pointer not needed.

   HWND checkBox1 = CreateWindow(
       L"BUTTON",  // Predefined class; Unicode assumed 
       L"Move",      // Button text 
       WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CHECKBOX,  // Styles 
       120,         // x position 
       10,         // y position 
       60,        // Button width
       30,        // Button height
       hWnd,     // Parent window
       (HMENU)ID_MOVE,       // No menu.
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
       NULL);      // Pointer not needed.

   HWND checkBox2 = CreateWindow(
       L"BUTTON",  // Predefined class; Unicode assumed 
       L"Size",      // Button text 
       WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CHECKBOX,  // Styles 
       200,         // x position 
       10,         // y position 
       60,        // Button width
       30,        // Button height
       hWnd,     // Parent window
       (HMENU)ID_SIZE,       // No menu.
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
       NULL);      // Pointer not needed.

   HWND checkBox3 = CreateWindow(
       L"BUTTON",  // Predefined class; Unicode assumed 
       L"Paint",      // Button text 
       WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CHECKBOX,  // Styles 
       280,         // x position 
       10,         // y position 
       60,        // Button width
       30,        // Button height
       hWnd,     // Parent window
       (HMENU)ID_PAINT,       // No menu.
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
       NULL);      // Pointer not needed.

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    if (!AllocConsole()) {
        MessageBox(NULL,
            _T("Call to CreateConsole failed!"),
            NULL,
            NULL);

        return 1;
    }

    // The parameters to ShowWindow explained:
    // hWnd: the value returned from CreateWindow
    // nCmdShow: the fourth parameter from WinMain
    ShowWindow(hWnd,
        nCmdShow);
    UpdateWindow(hWnd);

    // Main message loop:
    MSG msg;
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    const wchar_t* quitMessage = _T("Closing \n");

    DWORD out = 0;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED);
    WriteConsoleW(consoleHandle, quitMessage, wcslen(quitMessage), &out, NULL);

    Sleep(2000);
    FreeConsole();
    return (int)msg.wParam;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD out = 0;
    const wchar_t* moveCheckedMessage = _T("Move checkbox changed status \n");
    const wchar_t* sizeCheckedMessage = _T("Size checkbox changed status \n");
    const wchar_t* paintCheckedMessage = _T("Paint checkbox changed status \n");

    const wchar_t* moveMessage = _T("Window has moved \n");
    const wchar_t* sizeMessage = _T("Window has changed size \n");
    const wchar_t* paintMessage = _T("Window has been repainted \n");

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        if (IsDlgButtonChecked(hWnd, ID_PAINT))
            WriteConsoleW(consoleHandle, paintMessage, wcslen(paintMessage), &out, NULL);
        break;
    case WM_SIZE:
        if (IsDlgButtonChecked(hWnd, ID_SIZE))
            WriteConsoleW(consoleHandle, sizeMessage, wcslen(sizeMessage), &out, NULL);
        break;
    case WM_MOVE:
        if(IsDlgButtonChecked(hWnd, ID_MOVE))
            WriteConsoleW(consoleHandle, moveMessage, wcslen(moveMessage), &out, NULL);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_CLEAR:
            system("cls");
            break;
        case ID_MOVE:
            WriteConsoleW(consoleHandle, moveCheckedMessage, wcslen(moveCheckedMessage), &out, NULL);
            if (IsDlgButtonChecked(hWnd, ID_MOVE))
                CheckDlgButton(hWnd, ID_MOVE, BST_UNCHECKED);
            else 
                CheckDlgButton(hWnd, ID_MOVE, BST_CHECKED);
            break;
        case ID_SIZE:
            WriteConsoleW(consoleHandle, sizeCheckedMessage, wcslen(sizeCheckedMessage), &out, NULL);
            if (IsDlgButtonChecked(hWnd, ID_SIZE))
                CheckDlgButton(hWnd, ID_SIZE, BST_UNCHECKED);
            else
                CheckDlgButton(hWnd, ID_SIZE, BST_CHECKED);
            break;
        case ID_PAINT:
            WriteConsoleW(consoleHandle, paintCheckedMessage, wcslen(paintCheckedMessage), &out, NULL);
            if (IsDlgButtonChecked(hWnd, ID_PAINT))
                CheckDlgButton(hWnd, ID_PAINT, BST_UNCHECKED);
            else
                CheckDlgButton(hWnd, ID_PAINT, BST_CHECKED);
            break; 
        default:
            break;
        }    
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    return 0;
}