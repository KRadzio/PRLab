// HelloWindowsDesktop.cpp
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

// Global variables

#define ID_CLEAR 1
#define ID_COPY 2
#define ID_CUT 3
#define ID_PASTE 4
#define ID_EDIT 5
#define EDIT_SIZE 24

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
        500, 150,
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

    HWND copyButton = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"COPY",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        120,         // x position 
        10,         // y position 
        100,        // Button width
        30,        // Button height
        hWnd,     // Parent window
        (HMENU)ID_COPY,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    HWND cutButton = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"CUT",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        230,         // x position 
        10,         // y position 
        100,        // Button width
        30,        // Button height
        hWnd,     // Parent window
        (HMENU)ID_CUT,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    HWND pasteButton = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"PASTE",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        340,         // x position 
        10,         // y position 
        100,        // Button width
        30,        // Button height
        hWnd,     // Parent window
        (HMENU)ID_PASTE,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    HWND text = CreateWindow(
        L"Edit",  // Predefined class; Unicode assumed 
        NULL,      // Button text 
        WS_CHILD | WS_VISIBLE | WS_BORDER,  // Styles 
        10,         // x position 
        50,         // y position 
        200,        // Button width
        30,        // Button height
        hWnd,     // Parent window
        (HMENU)ID_EDIT,       // No menu.
        NULL,
        NULL);      // Pointer not needed.

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Windows Desktop Guided Tour"),
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
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

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
    char buffer[24];
    HGLOBAL hGlMem; 
    LPVOID lpGlMem;

    HGLOBAL hglb;
    LPVOID cpMem;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_CLEAR:
            SetDlgItemTextA(hWnd,ID_EDIT,"");
            break;
        case ID_COPY:
            GetDlgItemTextA(hWnd, ID_EDIT, buffer, EDIT_SIZE);
            hGlMem = GlobalAlloc(GHND, (DWORD)EDIT_SIZE + 1);
            lpGlMem = GlobalLock(hGlMem);
            memcpy(lpGlMem, buffer, EDIT_SIZE + 1);
            GlobalUnlock(hGlMem);
            OpenClipboard(hWnd);
            EmptyClipboard();
            SetClipboardData(CF_TEXT, hGlMem);
            CloseClipboard();
            break;
        case ID_CUT:
            GetDlgItemTextA(hWnd, ID_EDIT, buffer, EDIT_SIZE);
            hGlMem = GlobalAlloc(GHND, (DWORD)EDIT_SIZE + 1);
            lpGlMem = GlobalLock(hGlMem);
            memcpy(lpGlMem, buffer, EDIT_SIZE + 1);
            GlobalUnlock(hGlMem);   
            OpenClipboard(hWnd);
            EmptyClipboard();
            SetClipboardData(CF_TEXT, hGlMem);
            CloseClipboard();
            SetDlgItemTextA(hWnd, ID_EDIT, "");
            break;
        case ID_PASTE:
            if (!IsClipboardFormatAvailable(CF_TEXT))
                break;
            SetDlgItemTextA(hWnd, ID_EDIT, "");
            OpenClipboard(hWnd);
            hglb = GetClipboardData(CF_TEXT);
            cpMem = GlobalLock(hglb);
            memcpy(buffer, cpMem, EDIT_SIZE);
            GlobalUnlock(hglb);
            CloseClipboard();
            SetDlgItemTextA(hWnd, ID_EDIT, buffer);
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