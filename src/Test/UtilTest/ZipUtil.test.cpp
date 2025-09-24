#include <gtest/gtest.h>
#include "Util/ZipUtil/ZipUtil.h"
#include "Util/FileSystem/FileSystemUtil.h"
#include <filesystem>
#include <string>
#include <vector>

// Test fixture for ZipUtil tests
class ZipUtilTest : public ::testing::Test {
protected:
    using ZipUtil = CHTL::Util::ZipUtil;
    using FileSystemUtil = CHTL::Util::FileSystemUtil;

    const std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "chtl_zip_tests";
    const std::filesystem::path sourceDir = tempDir / "source";
    const std::filesystem::path destDir = tempDir / "destination";
    const std::filesystem::path zipFile = tempDir / "archive.zip";

    void SetUp() override {
        TearDown(); // Clean up from previous runs if any
        std::filesystem::create_directories(sourceDir);
        std::filesystem::create_directories(destDir);
    }

    void TearDown() override {
        std::filesystem::remove_all(tempDir);
    }

    // Helper to compare two directories
    bool are_dirs_equal(const std::filesystem::path& p1, const std::filesystem::path& p2) {
        auto it1 = std::filesystem::recursive_directory_iterator(p1);
        auto it2 = std::filesystem::recursive_directory_iterator(p2);

        while (it1 != std::filesystem::recursive_directory_iterator() && it2 != std::filesystem::recursive_directory_iterator()) {
            if (it1->is_directory() && it2->is_directory()) {
                if (std::filesystem::relative(it1->path(), p1) != std::filesystem::relative(it2->path(), p2)) return false;
            } else if (it1->is_regular_file() && it2->is_regular_file()) {
                if (std::filesystem::relative(it1->path(), p1) != std::filesystem::relative(it2->path(), p2)) return false;
                if (FileSystemUtil::readFile(it1->path()) != FileSystemUtil::readFile(it2->path())) return false;
            } else {
                return false; // Mismatch in item types
            }
            ++it1;
            ++it2;
        }
        return it1 == std::filesystem::recursive_directory_iterator() && it2 == std::filesystem::recursive_directory_iterator();
    }
};

TEST_F(ZipUtilTest, ZipAndUnzipRoundtrip) {
    // 1. Create a dummy directory structure to zip
    FileSystemUtil::writeFile(sourceDir / "root.txt", "root file");
    std::filesystem::create_directory(sourceDir / "subdir1");
    FileSystemUtil::writeFile(sourceDir / "subdir1" / "file1.txt", "file one");
    std::filesystem::create_directory(sourceDir / "subdir2");
    FileSystemUtil::writeFile(sourceDir / "subdir2" / "file2.log", "a log file");

    // 2. Zip the directory
    ASSERT_TRUE(ZipUtil::zip(sourceDir, zipFile));
    ASSERT_TRUE(std::filesystem::exists(zipFile));

    // 3. Unzip the directory to a new location
    ASSERT_TRUE(ZipUtil::unzip(zipFile, destDir));

    // 4. Compare the original and unzipped directories
    EXPECT_TRUE(are_dirs_equal(sourceDir, destDir));
}

TEST_F(ZipUtilTest, ZipNonExistentSource) {
    EXPECT_FALSE(ZipUtil::zip(tempDir / "nonexistent", zipFile));
}

TEST_F(ZipUtilTest, UnzipNonExistentArchive) {
    EXPECT_FALSE(ZipUtil::unzip(tempDir / "nonexistent.zip", destDir));
}
