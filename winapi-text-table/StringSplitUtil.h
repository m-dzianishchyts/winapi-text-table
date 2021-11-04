#pragma once

#include <string>
#include <vector>

namespace Util {
    template <typename C>
    std::vector<std::basic_string<C>> Split(std::basic_string<C> &str, std::basic_string<C> delimiter) {
        static_assert(std::is_same_v<C, char> || std::is_same_v<C, wchar_t>);
        const size_t delimiterLength = delimiter.length();
        size_t startPos = 0;
        size_t endPos;
        std::vector<std::basic_string<C>> tokens;

        while ((endPos = str.find(delimiter, startPos)) != std::wstring::npos) {
            size_t tokenLength = endPos - startPos;
            std::basic_string<C> token = str.substr(startPos, tokenLength);
            startPos = endPos + delimiterLength;
            tokens.push_back(token);
        }

        tokens.push_back(str.substr(startPos));
        return tokens;
    }
}
