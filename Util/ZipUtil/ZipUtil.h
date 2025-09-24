#pragma once

#include <string>

namespace CHTL {
namespace ZipUtil {

// This will be a wrapper around the miniz library.
// For now, these are just placeholder declarations.

// Compresses a single file into a zip archive.
bool compressFile(const std::string& sourceFilePath, const std::string& zipArchivePath);

// Decompresses a single file from a zip archive.
bool decompressFile(const std::string& zipArchivePath, const std::string& outputFilePath);

} // namespace ZipUtil
} // namespace CHTL
