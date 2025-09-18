#pragma once

#include <string>
#include <filesystem>

namespace CHTL {

// NOTE: This class is a placeholder. A third-party library (e.g., minizip, libzip)
// will be needed to implement the actual zip/unzip functionality for the CMOD system.

class ZipUtil {
public:
    // Compresses a directory into a .cmod file.
    static bool compressDirectory(const std::filesystem::path& directoryPath, const std::filesystem::path& zipFilePath);

    // Decompresses a .cmod file into a directory.
    static bool decompressToDirectory(const std::filesystem::path& zipFilePath, const std::filesystem::path& directoryPath);
};

} // namespace CHTL
