#include "StringUtil.h"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace CHTL {
namespace Util {

String StringUtil::trim(const String& str) {
    return trimLeft(trimRight(str));
}

String StringUtil::trimLeft(const String& str) {
    auto it = std::find_if(str.begin(), str.end(), [](char c) {
        return !isWhitespace(c);
    });
    return String(it, str.end());
}

String StringUtil::trimRight(const String& str) {
    auto it = std::find_if(str.rbegin(), str.rend(), [](char c) {
        return !isWhitespace(c);
    });
    return String(str.begin(), it.base());
}

Vector<String> StringUtil::split(const String& str, char delimiter) {
    Vector<String> result;
    std::stringstream ss(str);
    String item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

Vector<String> StringUtil::split(const String& str, const String& delimiter) {
    Vector<String> result;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != String::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    
    result.push_back(str.substr(start));
    return result;
}

String StringUtil::join(const Vector<String>& strings, const String& separator) {
    if (strings.empty()) {
        return "";
    }
    
    String result = strings[0];
    for (size_t i = 1; i < strings.size(); ++i) {
        result += separator + strings[i];
    }
    return result;
}

String StringUtil::toUpper(const String& str) {
    String result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

String StringUtil::toLower(const String& str) {
    String result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool StringUtil::startsWith(const String& str, const String& prefix) {
    if (str.length() < prefix.length()) {
        return false;
    }
    return str.compare(0, prefix.length(), prefix) == 0;
}

bool StringUtil::endsWith(const String& str, const String& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool StringUtil::contains(const String& str, const String& substr) {
    return str.find(substr) != String::npos;
}

String StringUtil::replace(const String& str, const String& from, const String& to) {
    size_t pos = str.find(from);
    if (pos == String::npos) {
        return str;
    }
    
    String result = str;
    result.replace(pos, from.length(), to);
    return result;
}

String StringUtil::replaceAll(const String& str, const String& from, const String& to) {
    if (from.empty()) {
        return str;
    }
    
    String result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != String::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

bool StringUtil::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool StringUtil::isAllWhitespace(const String& str) {
    return std::all_of(str.begin(), str.end(), isWhitespace);
}

bool StringUtil::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool StringUtil::isNumber(const String& str) {
    if (str.empty()) {
        return false;
    }
    
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') {
        start = 1;
    }
    
    bool hasDigit = false;
    bool hasDot = false;
    
    for (size_t i = start; i < str.length(); ++i) {
        if (isDigit(str[i])) {
            hasDigit = true;
        } else if (str[i] == '.' && !hasDot) {
            hasDot = true;
        } else {
            return false;
        }
    }
    
    return hasDigit;
}

bool StringUtil::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool StringUtil::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

bool StringUtil::isIdentifierStart(char c) {
    return isAlpha(c) || c == '_' || c == '$';
}

bool StringUtil::isIdentifierChar(char c) {
    return isAlphaNumeric(c) || c == '_' || c == '$';
}

String StringUtil::escape(const String& str) {
    String result;
    result.reserve(str.length());
    
    for (char c : str) {
        switch (c) {
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\'': result += "\\\'"; break;
            default: result += c; break;
        }
    }
    
    return result;
}

String StringUtil::unescape(const String& str) {
    String result;
    result.reserve(str.length());
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case 'n': result += '\n'; i++; break;
                case 'r': result += '\r'; i++; break;
                case 't': result += '\t'; i++; break;
                case '\\': result += '\\'; i++; break;
                case '\"': result += '\"'; i++; break;
                case '\'': result += '\''; i++; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    
    return result;
}

} // namespace Util
} // namespace CHTL
