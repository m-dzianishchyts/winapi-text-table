#include "WinApplication.h"

#include <windowsx.h>

#include "framework.h"
#include "resource.h"
#include "WinPrintfUtil.h"

// Forward Functions:
INT_PTR CALLBACK About(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam);

WinApplication& WinApplication::GetInstance() {
    static TextTable textTable(TABLE_MARGIN, TABLE_CELL_MARGIN);
    static WinApplication winApplication(textTable);
    return winApplication;
}

WinApplication::WinApplication(TextTable &textTable) :
    instance(NULL),
    _clientAreaWidth(0),
    _clientAreaHeight(0),
    _textTable(textTable) {
}

LRESULT WinApplication::HandleWinMessage(HWND window, UINT messageType, WPARAM wParam, LPARAM lParam) {
    switch (messageType) {
        HANDLE_MSG(window, WM_DESTROY, HandleQuit);
        HANDLE_MSG(window, WM_GETMINMAXINFO, HandleGetMinMaxInfo);
        HANDLE_MSG(window, WM_SIZE, HandleResize);
        HANDLE_MSG(window, WM_PAINT, HandlePaint);
        HANDLE_MSG(window, WM_COMMAND, HandleCommand);
        default:
            return DefWindowProc(window, messageType, wParam, lParam);
    }
}

LRESULT WinApplication::HandleCommand(HWND window, int id, HWND controlWindow, UINT notificationCode) const {
    switch (id) {
        case IDM_ABOUT:
            return DialogBoxW(instance, MAKEINTRESOURCE(IDD_ABOUTBOX), window, About);
        case IDM_EXIT:
            return DestroyWindow(window);
        default:
            return DefWindowProc(window, WM_PAINT, notificationCode << 16 | id,
                                 reinterpret_cast<LPARAM>(controlWindow));
    }
}

void WinApplication::HandleGetMinMaxInfo(HWND window, LPMINMAXINFO minMaxInfo) {
    minMaxInfo->ptMinTrackSize.x = SM_CXMINTRACK + 250;
    minMaxInfo->ptMinTrackSize.y = SM_CYMINTRACK + 50;
}

void WinApplication::HandleQuit(HWND window) {
    Util::Debug(L"QUIT!\n");
    PostQuitMessage(0);
}

void WinApplication::HandleResize(HWND window, UINT state, LONG width, LONG height) {
    Util::Debug(L"RESIZE!\n");
    _clientAreaWidth = width;
    _clientAreaHeight = height;
    InvalidateRect(window, NULL, TRUE);
}

void WinApplication::HandlePaint(HWND window) const {
    Util::Debug(L"PAINT!\n");
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(window, &ps);
    HDC bufferDeviceContext = CreateCompatibleDC(hdc);
    HBITMAP bufferBitMap = CreateCompatibleBitmap(hdc, _clientAreaWidth, _clientAreaHeight);
    SelectObject(bufferDeviceContext, bufferBitMap);
    BitBlt(bufferDeviceContext, 0, 0, _clientAreaWidth, _clientAreaHeight, NULL, 0, 0, WHITENESS);
    _textTable.Draw(window, bufferDeviceContext);
    BitBlt(hdc, 0, 0, _clientAreaWidth, _clientAreaHeight, bufferDeviceContext, 0, 0, SRCCOPY);
    DeleteObject(bufferBitMap);
    DeleteDC(bufferDeviceContext);
    EndPaint(window, &ps);
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(dialog, LOWORD(wParam));
                return TRUE;
            }
            break;
        default:
            return FALSE;
    }
    return FALSE;
}
