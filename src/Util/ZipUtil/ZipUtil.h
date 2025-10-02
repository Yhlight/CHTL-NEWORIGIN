/**
 * @file ZipUtil.h
 * @brief ZIP utilities for CMOD/CJMOD
 */

#ifndef ZIP_UTIL_H
#define ZIP_UTIL_H

#include <string>
#include <vector>

namespace Util {

class ZipUtil {
public:
    static bool compress(const std::string& sourcePath, const std::string& zipPath);
    static bool decompress(const std::string& zipPath, const std::string& destPath);
    static std::vector<std::string> listFiles(const std::string& zipPath);
};

} // namespace Util

#endif // ZIP_UTIL_H
