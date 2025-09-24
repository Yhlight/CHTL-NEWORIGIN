#pragma once

#include <filesystem>

namespace CHTL::Util {

/**
 * @class ZipUtil
 * @brief Provides a collection of static utility functions for zip archive operations.
 * This is a wrapper around the miniz library.
 */
class ZipUtil {
public:
    /**
     * @brief Creates a zip archive from a source directory.
     * @param sourceDir The directory to compress.
     * @param zipPath The path for the output zip file.
     * @return True if successful, false otherwise.
     */
    static bool zip(const std::filesystem::path& sourceDir, const std::filesystem::path& zipPath);

    /**
     * @brief Extracts a zip archive to a destination directory.
     * @param zipPath The path to the zip file.
     * @param destinationDir The directory to extract the contents to.
     * @return True if successful, false otherwise.
     */
    static bool unzip(const std::filesystem::path& zipPath, const std::filesystem::path& destinationDir);
};

} // namespace CHTL::Util
