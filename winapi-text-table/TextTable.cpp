#include "TextTable.h"

#include <algorithm>
#include <cwctype>
#include <numeric>
#include <wingdi.h>
#include <WinUser.h>

#include "resource.h"
#include "StringSplitUtil.h"
#include "WinPrintfUtil.h"

static constexpr WCHAR TABLE_DATA_RESOURCE_TYPE[] = L"PLAINTEXT";
static constexpr WCHAR WINDOWS_NEW_LINE[] = L"\r\n";
static constexpr WORD TABLE_DATA_ROWS_AMOUNT_POS = 0;
static constexpr WORD TABLE_DATA_COLUMNS_AMOUNT_POS = 1;

static constexpr COLORREF BLACK = 0x000000;
static constexpr WORD TABLE_EXTERNAL_BORDER_WIDTH = 2;
static constexpr WORD TABLE_INTERNAL_BORDER_WIDTH = 1;

TextTable::TextTable(const LONG tableMargin, LONG cellMargin) :
    tableMarginLeft(tableMargin),
    tableMarginTop(tableMargin),
    tableMarginRight(tableMargin),
    tableMarginBottom(tableMargin),
    cellMarginLeft(cellMargin),
    cellMarginTop(cellMargin),
    cellMarginRight(cellMargin),
    cellMarginBottom(cellMargin),
    rowsAmount(0),
    columnsAmount(0) {
    _font.lfHeight = 20;
    LoadTableData();
}

void TextTable::LoadTableData() {
    if (HRSRC resource = FindResource(NULL, MAKEINTRESOURCE(IDR_TABLE_DATA_1), TABLE_DATA_RESOURCE_TYPE);
        NULL != resource) {
        if (HGLOBAL resourceData = LoadResource(NULL, resource);
            NULL != resourceData) {
            const DWORD dataSize = SizeofResource(NULL, resource);
            auto *strData = static_cast<wchar_t*>(LockResource(resourceData));
            strData += 1; // Skip UTF-16 BOM
            std::wstring str;
            str.assign(strData, dataSize / sizeof(WCHAR));
            std::vector<std::wstring> tokens = Util::Split(str, std::wstring(WINDOWS_NEW_LINE));
            rowsAmount = static_cast<WORD>(std::stoi(tokens[TABLE_DATA_ROWS_AMOUNT_POS]));
            columnsAmount = static_cast<WORD>(std::stoi(tokens[TABLE_DATA_COLUMNS_AMOUNT_POS]));
            _data = std::vector(tokens.begin() + TABLE_DATA_COLUMNS_AMOUNT_POS + 1, tokens.end());
        } else {
            Util::Debug(L"Cannot load resource!\r\n");
        }
    } else {
        Util::Debug(L"File is missing!\r\n");
    }
}

std::vector<std::wstring> TextTable::GetData() const {
    return _data;
}

void TextTable::Draw(HWND window, HDC deviceContext) {
    RECT clientArea;
    GetClientRect(window, &clientArea);
    RECT tableArea = clientArea;
    tableArea.left += tableMarginLeft;
    tableArea.top += tableMarginTop;
    tableArea.right -= tableMarginRight;
    tableArea.bottom -= tableMarginBottom;

    if (AllStringsFitToCells(deviceContext, tableArea)) {
        MaximizeFontSize(deviceContext, tableArea);
    } else {
        NormalizeFontSize(deviceContext, tableArea);
    }
    DrawExternalBorders(deviceContext, tableArea);
    DrawInternalVerticalBorders(deviceContext, tableArea);
    DrawInternalHorizontalBorders(deviceContext, tableArea);
    DrawData(deviceContext, tableArea);
}

void TextTable::DrawExternalBorders(HDC deviceContext, const RECT tableArea) {
    HPEN pen = CreatePen(PS_SOLID, TABLE_EXTERNAL_BORDER_WIDTH, BLACK);
    Rectangle(deviceContext, tableArea.left, tableArea.top, tableArea.right, tableArea.bottom);
    HGDIOBJ originalPen = SelectObject(deviceContext, pen);
    SelectObject(deviceContext, originalPen);
    DeleteObject(pen);
}

void TextTable::DrawInternalVerticalBorders(HDC deviceContext, const RECT tableArea) const {
    HPEN pen = CreatePen(PS_SOLID, TABLE_INTERNAL_BORDER_WIDTH, BLACK);
    HGDIOBJ originalPen = SelectObject(deviceContext, pen);
    LONG tableWidth = tableArea.right - tableArea.left;
    LONG columnWidth = tableWidth / columnsAmount;
    LONG x = tableArea.left + columnWidth;
    for (WORD columnIndex = 1; columnIndex < columnsAmount; ++columnIndex) {
        MoveToEx(deviceContext, x, tableArea.top, NULL);
        LineTo(deviceContext, x, tableArea.bottom);
        x += columnWidth;
    }
    SelectObject(deviceContext, originalPen);
    DeleteObject(pen);
}

void TextTable::DrawInternalHorizontalBorders(HDC deviceContext, const RECT tableArea) {
    HPEN pen = CreatePen(PS_SOLID, TABLE_INTERNAL_BORDER_WIDTH, BLACK);
    HGDIOBJ originalPen = SelectObject(deviceContext, pen);

    auto [left, top, right, bottom] = tableArea;
    LONG tableWidth = right - left;
    LONG columnWidth = tableWidth / columnsAmount;
    LONG cellContentWidth = columnWidth - cellMarginLeft - cellMarginRight;

    std::vector<LONG> rowHeights;
    CalculateRowTextHeights(deviceContext, cellContentWidth, rowHeights);
    rowHeights.pop_back();
    for (long &rowHeight : rowHeights) {
        rowHeight += cellMarginTop + cellMarginBottom;
    }
    LONG posY = top;
    for (LONG rowHeight : rowHeights) {
        posY += rowHeight;
        MoveToEx(deviceContext, tableArea.left, posY, NULL);
        LineTo(deviceContext, tableArea.right, posY);
    }
    SelectObject(deviceContext, originalPen);
    DeleteObject(pen);
}

void TextTable::CalculateRowTextHeights(HDC deviceContext, LONG cellContentWidth, std::vector<LONG> &rowTextHeights) {
    HFONT currentFont = CreateFontIndirect(&_font);
    auto originalFont = static_cast<HFONT>(SelectObject(deviceContext, currentFont));

    for (int rowIndex = 0; rowIndex < rowsAmount; ++rowIndex) {
        std::vector<LONG> cellTextHeights;
        for (int columnIndex = 0; columnIndex < columnsAmount; ++columnIndex) {
            LONG cellIndex = columnsAmount * rowIndex + columnIndex;
            std::wstring cellString = _data[cellIndex];
            RECT realTextRect {0, 0, cellContentWidth, 0};
            DrawTextW(deviceContext, cellString.data(), static_cast<int>(cellString.length()), &realTextRect,
                      DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT);
            LONG realTextHeight = realTextRect.bottom - realTextRect.top;
            cellTextHeights.push_back(realTextHeight);
        }
        LONG maxCellTextHeight = *std::max_element(cellTextHeights.begin(), cellTextHeights.end());
        rowTextHeights.push_back(maxCellTextHeight);
    }
    SelectObject(deviceContext, originalFont);
    DeleteObject(currentFont);
}

BOOL TextTable::AllStringsFitToCells(HDC deviceContext, RECT tableArea) {
    LONG tableWidth = tableArea.right - tableArea.left;
    LONG contentHeight = tableArea.bottom - tableArea.top;
    DOUBLE columnWidth = static_cast<DOUBLE>(tableWidth) / columnsAmount;
    DOUBLE cellContentWidth = columnWidth - cellMarginLeft - cellMarginRight;

    std::vector<LONG> rowTextHeights;
    CalculateRowTextHeights(deviceContext, static_cast<LONG>(cellContentWidth), rowTextHeights);

    LONG rowsContentTotalHeight = std::accumulate(rowTextHeights.begin(), rowTextHeights.end(), 0);
    LONG rowsTotalHeight = rowsContentTotalHeight + rowsAmount * (cellMarginTop + cellMarginBottom);
    Util::Debug(L"Fit to height: %d\n", rowsTotalHeight <= contentHeight);
    return rowsTotalHeight <= contentHeight;
}

void TextTable::MaximizeFontSize(HDC deviceContext, RECT tableArea) {
    do {
        _font.lfHeight++;
    } while (AllStringsFitToCells(deviceContext, tableArea));
    _font.lfHeight--;
}

void TextTable::NormalizeFontSize(HDC deviceContext, RECT tableArea) {
    do {
        _font.lfHeight--;
    } while (_font.lfHeight > 0 && !AllStringsFitToCells(deviceContext, tableArea));
}

void TextTable::DrawData(HDC deviceContext, const RECT tableArea) {
    HFONT currentFont = CreateFontIndirect(&_font);
    auto originalFont = static_cast<HFONT>(SelectObject(deviceContext, currentFont));

    LONG contentWidth = tableArea.right - tableArea.left;
    DOUBLE columnWidth = static_cast<DOUBLE>(contentWidth) / columnsAmount;
    LONG cellContentWidth = static_cast<LONG>(columnWidth) - cellMarginLeft - cellMarginRight;
    std::vector<LONG> rowTextHeights;
    CalculateRowTextHeights(deviceContext, cellContentWidth, rowTextHeights);
    LONG posY = tableArea.top + cellMarginTop;
    RECT cellArea;
    cellArea.top = posY;
    for (int rowIndex = 0; rowIndex < rowsAmount; ++rowIndex) {
        cellArea.left = tableArea.left + cellMarginLeft;
        cellArea.bottom = cellArea.top + rowTextHeights[rowIndex];
        for (int columnIndex = 0; columnIndex < columnsAmount; ++columnIndex) {
            LONG cellIndex = columnsAmount * rowIndex + columnIndex;
            cellArea.right = cellArea.left + cellContentWidth;
            std::wstring cellString = _data[cellIndex];
            WCHAR *cellStringData = cellString.data();
            DrawTextW(deviceContext, cellStringData, static_cast<int>(cellString.length()), &cellArea,
                      DT_WORDBREAK | DT_EDITCONTROL);
            cellArea.left = cellArea.right + cellMarginRight + cellMarginLeft;
        }
        cellArea.top = cellArea.bottom + cellMarginBottom + cellMarginTop;

    }
    SelectObject(deviceContext, originalFont);
    DeleteObject(currentFont);
}
