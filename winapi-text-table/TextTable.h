#pragma once

#include <string>
#include <vector>
#include <Windows.h>

class TextTable {
public:
    LONG tableMarginLeft;
    LONG tableMarginTop;
    LONG tableMarginRight;
    LONG tableMarginBottom;
    LONG cellMarginLeft;
    LONG cellMarginTop;
    LONG cellMarginRight;
    LONG cellMarginBottom;
    WORD rowsAmount;
    WORD columnsAmount;

    explicit TextTable(LONG tableMargin, LONG cellMargin);

    [[nodiscard]] std::vector<std::wstring> GetData() const;
    void Draw(HWND window, HDC deviceContext);

private:
    std::vector<std::wstring> _data;
    LOGFONT _font;

    void LoadTableData();
    void DrawInternalVerticalBorders(HDC deviceContext, RECT tableArea) const;
    void DrawInternalHorizontalBorders(HDC deviceContext, RECT tableArea);
    void DrawData(HDC deviceContext, RECT tableArea);

    BOOL TextFitsIntoTable(HDC deviceContext, RECT tableArea);
    void CalculateRowTextHeights(HDC deviceContext, LONG cellContentWidth, std::vector<LONG> &rowTextHeights);
    void MaximizeFontSize(HDC deviceContext, RECT tableArea);
    void NormalizeFontSize(HDC deviceContext, RECT tableArea);

    static void DrawExternalBorders(HDC deviceContext, RECT tableArea);
};
