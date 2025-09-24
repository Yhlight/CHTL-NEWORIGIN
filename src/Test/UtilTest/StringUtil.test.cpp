#include <gtest/gtest.h>
#include "Util/StringUtil/StringUtil.h"

// Test fixture for StringUtil tests
class StringUtilTest : public ::testing::Test {
protected:
    using StringUtil = CHTL::Util::StringUtil;
};

TEST_F(StringUtilTest, TrimFunction) {
    EXPECT_EQ(StringUtil::trim("  hello world  "), "hello world");
    EXPECT_EQ(StringUtil::trim("hello world  "), "hello world");
    EXPECT_EQ(StringUtil::trim("  hello world"), "hello world");
    EXPECT_EQ(StringUtil::trim("hello world"), "hello world");
    EXPECT_EQ(StringUtil::trim("\t\n hello \t\n"), "hello");
    EXPECT_EQ(StringUtil::trim(""), "");
    EXPECT_EQ(StringUtil::trim("   "), "");
}

TEST_F(StringUtilTest, SplitFunction) {
    std::vector<std::string> expected1 = {"hello", "world"};
    EXPECT_EQ(StringUtil::split("hello,world", ','), expected1);

    std::vector<std::string> expected2 = {"one", "two", "three"};
    EXPECT_EQ(StringUtil::split("one two three", ' '), expected2);

    std::vector<std::string> expected3 = {"single"};
    EXPECT_EQ(StringUtil::split("single", ','), expected3);

    std::vector<std::string> expected4 = {"", "a", ""};
    EXPECT_EQ(StringUtil::split(",a,", ','), expected4);

    // FIX: Update test case to reflect new behavior for empty strings.
    std::vector<std::string> expected5 = {""};
    EXPECT_EQ(StringUtil::split("", ','), expected5);
}
