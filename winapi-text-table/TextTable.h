#pragma once

#include <string>
#include <vector>
#include <Windows.h>

class TextTable {
public:
    LONG paddingTop;
    LONG paddingBottom;
    LONG paddingLeft;
    LONG paddingRight;
    WORD rowsAmount;
    WORD columnsAmount;

    explicit TextTable(LONG padding);

    [[nodiscard]] std::vector<std::wstring> GetData() const;
    void Draw(HWND window, HDC deviceContext) const;

private:
    std::vector<std::wstring> _data;

    void LoadTableData();
    void DrawInternalBorders(HDC deviceContext, LONG top, LONG bottom, LONG left, LONG right) const;

    static void DrawExternalBorders(HDC deviceContext, LONG top, LONG bottom, LONG left, LONG right);
};
