#include "TextTable.h"

#include <cwctype>
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


TextTable::TextTable(const LONG padding) :
    paddingTop(padding), paddingBottom(padding),
    paddingLeft(padding), paddingRight(padding),
    rowsAmount(0), columnsAmount(0) {
    LoadTableData();
}

void TextTable::LoadTableData()
{
    if (HRSRC resource = FindResource(NULL, MAKEINTRESOURCE(IDR_TABLE_DATA_1), TABLE_DATA_RESOURCE_TYPE);
        NULL != resource) {
        if (HGLOBAL resourceData = LoadResource(NULL, resource);
            NULL != resourceData) {
            const DWORD dataSize = SizeofResource(NULL, resource);
            auto *strData = static_cast<wchar_t *>(LockResource(resourceData));
            strData += 1; // Skip UTF-16 BOM
            std::wstring str;
            str.assign(strData, dataSize);
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

std::vector<std::wstring> TextTable::GetData() const
{
    return _data;
}

void TextTable::Draw(HWND window, HDC deviceContext) const {
    RECT clientArea;
    GetClientRect(window, &clientArea);
    LONG top = clientArea.top + paddingTop;
    LONG bottom = clientArea.bottom - paddingBottom;
    LONG left = clientArea.top + paddingLeft;
    LONG right = clientArea.right - paddingRight;
    DrawExternalBorders(deviceContext, top, bottom, left, right);
    DrawInternalBorders(deviceContext, top, bottom, left, right);
}


void TextTable::DrawExternalBorders(HDC deviceContext, LONG top, LONG bottom, LONG left, LONG right)
{
    HPEN pen = CreatePen(PS_SOLID, TABLE_EXTERNAL_BORDER_WIDTH, BLACK);
    Rectangle(deviceContext, left, top, right, bottom);
    HGDIOBJ originalPen = SelectObject(deviceContext, pen);
    SelectObject(deviceContext, originalPen);
    DeleteObject(pen);
}

void TextTable::DrawInternalBorders(HDC deviceContext, LONG top, LONG bottom, LONG left, LONG right) const
{
    HPEN pen = CreatePen(PS_SOLID, TABLE_INTERNAL_BORDER_WIDTH, BLACK);
    HGDIOBJ originalPen = SelectObject(deviceContext, pen);
    LONG tableWidth = right - left;
    LONG tableHeight = bottom - top;
    DOUBLE rowHeight = static_cast<DOUBLE>(tableHeight) / rowsAmount;
    DOUBLE columnWidth = static_cast<DOUBLE>(tableWidth) / columnsAmount;
    for (WORD i = 1; i < rowsAmount; i++) {
        LONG y = top + static_cast<LONG>(rowHeight * i);
        MoveToEx(deviceContext, left, y, NULL);
        LineTo(deviceContext, right, y);
    }
    for (WORD i = 1; i < columnsAmount; i++) {
        LONG x = left + static_cast<LONG>(columnWidth * i);
        MoveToEx(deviceContext, x, top, NULL);
        LineTo(deviceContext, x, bottom);
    }
    SelectObject(deviceContext, originalPen);
    DeleteObject(pen);
}
