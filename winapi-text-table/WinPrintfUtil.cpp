#include "WinPrintfUtil.h"

namespace Util {

    static constexpr WORD BUFFER_SIZE = 1000;

    void Debug(STRSAFE_LPCWSTR formatString, ...) {
        va_list args;
        va_start(args, formatString);
        TCHAR buffer[BUFFER_SIZE];
        StringCchVPrintfW(buffer, BUFFER_SIZE, formatString, args);
        OutputDebugStringW(buffer);
        va_end(args);
    }
}
