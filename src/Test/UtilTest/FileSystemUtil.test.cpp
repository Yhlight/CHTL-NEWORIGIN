#include <gtest/gtest.h>
#include "Util/FileSystem/FileSystemUtil.h"
#include <filesystem>
#include <string>

// Test fixture for FileSystemUtil tests
class FileSystemUtilTest : public ::testing::Test {
protected:
    using FileSystemUtil = CHTL::Util::FileSystemUtil;
    const std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "chtl_tests";

    void SetUp() override {
        // Create a temporary directory for test files
        std::filesystem::create_directories(tempDir);
    }

    void TearDown() override {
        // Clean up the temporary directory
        std::filesystem::remove_all(tempDir);
    }
};

TEST_F(FileSystemUtilTest, WriteAndReadFile) {
    const std::filesystem::path testFile = tempDir / "test.txt";
    const std::string content = "Hello, CHTL!\nThis is a test file.";

    // Test writing to the file
    ASSERT_NO_THROW(FileSystemUtil::writeFile(testFile, content));
    ASSERT_TRUE(std::filesystem::exists(testFile));

    // Test reading from the file
    std::string readContent;
    ASSERT_NO_THROW(readContent = FileSystemUtil::readFile(testFile));

    // Verify the content is the same
    EXPECT_EQ(content, readContent);
}

TEST_F(FileSystemUtilTest, ReadNonExistentFile) {
    const std::filesystem::path nonExistentFile = tempDir / "non_existent_file.txt";

    // Verify that reading a non-existent file throws an exception
    EXPECT_THROW(FileSystemUtil::readFile(nonExistentFile), std::runtime_error);
}

TEST_F(FileSystemUtilTest, WriteFileCreatesDirectory) {
    const std::filesystem::path deepTestFile = tempDir / "subdir" / "test.txt";
    const std::string content = "Testing directory creation.";

    // The "subdir" does not exist yet. writeFile should create it.
    ASSERT_FALSE(std::filesystem::exists(deepTestFile.parent_path()));
    ASSERT_NO_THROW(FileSystemUtil::writeFile(deepTestFile, content));
    ASSERT_TRUE(std::filesystem::exists(deepTestFile));

    // Verify the content
    std::string readContent = FileSystemUtil::readFile(deepTestFile);
    EXPECT_EQ(content, readContent);
}
