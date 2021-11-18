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
    static constexpr LONG TABLE_MARGIN = 10;
    static constexpr LONG TABLE_CELL_MARGIN = 5;

    static constexpr LONG MIN_TRACK_WIDTH = 250;
    static constexpr LONG MIN_TRACK_HEIGHT = 50;

    LRESULT HandleCommand(HWND window, int id, HWND controlWindow, UINT notificationCode) const;
    static void HandleQuit(HWND window);
    void HandleResize(HWND window, UINT state, LONG width, LONG height);
    void HandlePaint(HWND window) const;

    static void HandleGetMinMaxInfo(HWND window, LPMINMAXINFO minMaxInfo);
};
