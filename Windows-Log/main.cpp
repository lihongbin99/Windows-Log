#ifndef UNICODE
#define UNICODE
#endif 

#define ID_OUTPUT 1
#define ID_INPUT  2

#include <windows.h>
#include "resource.h"

TCHAR buf[1024];

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HINSTANCE hInstance;
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    ::hInstance = hInstance;
    TCHAR CLASS_NAME[] = TEXT("Windows-Log");

    WNDCLASS wc;
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = WindowProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(0x00FFFFFF);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc)) {
        return 0;
    }

    HWND hwnd = CreateWindowEx(0,
        CLASS_NAME,
        CLASS_NAME,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    HACCEL hAccel = LoadAccelerators(hInstance, CLASS_NAME);
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        if (!TranslateAccelerator(hwnd, hAccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return msg.wParam;
}

TCHAR* GetModuleName() {
    DWORD len = GetModuleFileName(NULL, buf, sizeof(buf) / sizeof(TCHAR));
    if (len) {
        for (; len > 0; --len) {
            if (buf[len] == TEXT(".")[0]) {
                buf[len] = 0;
                break;
            }
        }
        for (; len > 0; --len) {
            if (buf[len] == TEXT("\\")[0]) {
                ++len;
                break;
            }
        }
        return buf+len;
    }
    return 0;
}

void AppendText(HWND hEditWnd, LPCTSTR Text)
{
    int idx = GetWindowTextLength(hEditWnd);
    SendMessage(hEditWnd, EM_SETSEL, (WPARAM)idx, (LPARAM)idx);
    SendMessage(hEditWnd, EM_REPLACESEL, 0, (LPARAM)Text);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static TCHAR* moduleName;
    static HWND hwndOutput;
    static HWND hwndInput;
    static HFONT hFont;
    static int fontHeight = 20;
    switch (uMsg) {
    case WM_CREATE:
        // 获取当前文件名来设置窗口名称
        moduleName = GetModuleName();
        if (moduleName) {
            SetWindowText(hwnd, moduleName);
        }
        // 创建输出框
        hwndOutput = CreateWindow(TEXT("edit"), NULL,
            WS_CHILD | WS_VISIBLE /* | ES_READONLY*/ | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
            0, 0, 0, 0, hwnd, (HMENU)ID_OUTPUT, hInstance, NULL
        );
        // 创建输入框
        hwndInput = CreateWindow(TEXT("edit"), NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
            0, 0, 0, 0, hwnd, (HMENU)ID_INPUT, hInstance, NULL
        );
        //创建字体
        hFont = CreateFont(fontHeight, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT("微软雅黑"));
        //设置编辑框的字体
        SendMessage(hwndOutput, WM_SETFONT, (WPARAM)hFont, NULL);
        SendMessage(hwndInput, WM_SETFONT, (WPARAM)hFont, NULL);
        // 设置输出框最长字符串
        SendMessage(hwndOutput, EM_LIMITTEXT, -1, 0);
        return 0;
    case WM_CTLCOLOREDIT:
        if (hwndOutput == (HWND)lParam) {
            SetTextColor((HDC)wParam, RGB(0xFF, 0xFF, 0xFF));
            SetBkColor((HDC)wParam, RGB(0, 0, 0));
            return (INT_PTR)CreateSolidBrush(RGB(0, 0, 0));
        }
        break;
    case WM_SETFOCUS:
        SetFocus(hwndInput);
        return 0;
    case WM_SIZE:
        MoveWindow(hwndOutput, 0, 0, LOWORD(lParam), HIWORD(lParam) - fontHeight, TRUE);
        MoveWindow(hwndInput, 0, HIWORD(lParam) - fontHeight, LOWORD(lParam), fontHeight, TRUE);
        return 0;
    case WM_COMMAND:
        if (lParam) {
            if (LOWORD(wParam) == ID_OUTPUT) {
                if (HIWORD(wParam) == EN_SETFOCUS) {
                    // SetFocus(hwndInput);
                }
            }
        } else {
            switch (LOWORD(wParam)) {
            case IDM_SELECT_ALL:
                SendMessage(hwndOutput, EM_SETSEL, 0, -1);
                return 0;
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}