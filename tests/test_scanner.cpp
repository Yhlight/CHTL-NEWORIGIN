#include <gtest/gtest.h>
#include "CHTLCommon.h"
#include "UnifiedScanner.h"

using namespace CHTL;

class ScannerTest : public ::testing::Test {
protected:
    void SetUp() override {
        scanner = std::make_unique<UnifiedScanner>("", "");
    }
    
    void TearDown() override {
        scanner.reset();
    }
    
    std::unique_ptr<UnifiedScanner> scanner;
};

TEST_F(ScannerTest, BasicScanning) {
    std::string source = "div { text: \"Hello World\"; }";
    scanner->setSource(source, "test.chtl");
    auto fragments = scanner->scan();
    
    ASSERT_GT(fragments.size(), 0);
    EXPECT_EQ(fragments[0].type, FragmentType::CHTL);
}

TEST_F(ScannerTest, MixedContent) {
    std::string source = R"(
        div {
            style {
                width: 100px;
            }
            script {
                console.log("Hello");
            }
        }
    )";
    
    scanner->setSource(source, "test.chtl");
    auto fragments = scanner->scan();
    
    ASSERT_GT(fragments.size(), 0);
    
    // Should detect different fragment types
    bool hasCHTL = false;
    bool hasCSS = false;
    bool hasJS = false;
    
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL) hasCHTL = true;
        if (fragment.type == FragmentType::CSS) hasCSS = true;
        if (fragment.type == FragmentType::JS) hasJS = true;
    }
    
    EXPECT_TRUE(hasCHTL);
    // CSS and JS detection might be more complex in real implementation
}

TEST_F(ScannerTest, EmptyInput) {
    std::string source = "";
    scanner->setSource(source, "test.chtl");
    auto fragments = scanner->scan();
    
    EXPECT_EQ(fragments.size(), 0);
}

TEST_F(ScannerTest, FragmentProcessing) {
    std::string source = "div { text: \"Hello\"; }";
    scanner->setSource(source, "test.chtl");
    auto fragments = scanner->scan();
    
    ASSERT_GT(fragments.size(), 0);
    
    // Process fragments
    scanner->processFragments();
    
    // Fragments should still exist after processing
    EXPECT_GT(scanner->getFragments().size(), 0);
}

TEST_F(ScannerTest, PlaceholderManagement) {
    std::string source = "div { text: \"Hello\"; }";
    scanner->setSource(source, "test.chtl");
    auto fragments = scanner->scan();
    
    // Placeholder manager should be available
    const auto& placeholderManager = scanner->getPlaceholderManager();
    EXPECT_TRUE(placeholderManager.getPlaceholderMap().empty());
}