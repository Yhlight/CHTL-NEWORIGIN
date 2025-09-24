#pragma once

#include <string>
#include <vector>

namespace CHTL::Util {

/**
 * @class StringUtil
 * @brief Provides a collection of static utility functions for string manipulation.
 */
class StringUtil {
public:
    /**
     * @brief Trims leading and trailing whitespace from a string.
     * @param str The string to trim.
     * @return The trimmed string.
     */
    static std::string trim(const std::string& str);

    /**
     * @brief Splits a string into a vector of substrings based on a delimiter.
     * @param str The string to split.
     * @param delimiter The character to split the string by.
     * @return A vector of substrings.
     */
    static std::vector<std::string> split(const std::string& str, char delimiter);
};

} // namespace CHTL::Util
