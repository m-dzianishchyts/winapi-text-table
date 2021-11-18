#pragma once

#include "Windows.h"
#include "strsafe.h"

namespace Util {
    void Debug(STRSAFE_LPCWSTR formatString, ...);
}
