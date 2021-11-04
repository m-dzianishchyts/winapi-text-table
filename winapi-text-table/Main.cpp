#include "framework.h"
#include "winapi-text-table.h"
#include "WinApplication.h"

constexpr auto MAX_LOAD_STRING = 100;

// Global Variables:
WinApplication *winApplication;
WCHAR windowTitle[MAX_LOAD_STRING];
WCHAR windowClassName[MAX_LOAD_STRING];

// Forward Functions:
ATOM RegisterClassLocal(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// ReSharper disable once CppInconsistentNaming
int APIENTRY wWinMain(_In_ HINSTANCE       hInstance,
                      _In_opt_ HINSTANCE   hPrevInstance,
                      _In_ LPWSTR          lpCmdLine,
                      _In_ int       nShowCmd) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    winApplication = &WinApplication::GetInstance();

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, windowTitle, MAX_LOAD_STRING);
    LoadStringW(hInstance, IDC_WINAPITEXTTABLE, windowClassName, MAX_LOAD_STRING);
    RegisterClassLocal(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nShowCmd)) {
        return FALSE;
    }

    HACCEL acceleratorsTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPITEXTTABLE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, acceleratorsTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return static_cast<int>(msg.wParam);
}

ATOM RegisterClassLocal(HINSTANCE hInstance) {
    WNDCLASSEXW windowClass;

    windowClass.cbSize = sizeof(WNDCLASSEX);

    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WndProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINAPITEXTTABLE));
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    windowClass.lpszMenuName = MAKEINTRESOURCEW(IDC_WINAPITEXTTABLE);
    windowClass.lpszClassName = windowClassName;
    windowClass.hIconSm = LoadIcon(windowClass.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&windowClass);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    winApplication->instance = hInstance;
    HWND hWnd = CreateWindowW(windowClassName, windowTitle, WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) {
        return FALSE;
    }
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = winApplication->HandleWinMessage(hWnd, message, wParam, lParam);
    return result;
}
