#pragma once

#include <Windows.h>

#include "TextTable.h"

class WinApplication {
public:
    HINSTANCE instance;
    LRESULT HandleWinMessage(HWND window, UINT messageType, WPARAM wParam, LPARAM lParam);
    WinApplication(WinApplication &other) = delete;
    static WinApplication& GetInstance();

protected:
    explicit WinApplication(TextTable &textTable);

private:
    LONG _clientAreaWidth;
    LONG _clientAreaHeight;

    TextTable &_textTable;
    static constexpr LONG TABLE_PADDING = 10;

    LRESULT HandleCommand(HWND window, int id, HWND controlWindow, UINT notificationCode) const;
    static void HandleQuit(HWND window);
    void HandleResize(HWND window, UINT state, LONG width, LONG height);
    void HandlePaint(HWND window) const;

    static void HandleGetMinMaxInfo(HWND window, LPMINMAXINFO minMaxInfo);
};
