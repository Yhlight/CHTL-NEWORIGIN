/**
 * @file StringUtil.h
 * @brief String utilities
 */

#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <vector>

namespace Util {

class StringUtil {
public:
    static std::string trim(const std::string& str);
    static std::string trimLeft(const std::string& str);
    static std::string trimRight(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& parts, const std::string& delimiter);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
};

} // namespace Util

#endif // STRING_UTIL_H
