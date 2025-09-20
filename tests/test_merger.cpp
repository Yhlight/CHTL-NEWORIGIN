#include <gtest/gtest.h>
#include "CHTLCommon.h"
#include "CodeMerger.h"

using namespace CHTL;

class MergerTest : public ::testing::Test {
protected:
    void SetUp() override {
        merger = std::make_unique<CodeMerger>();
    }
    
    void TearDown() override {
        merger.reset();
    }
    
    std::unique_ptr<CodeMerger> merger;
};

TEST_F(MergerTest, BasicMerging) {
    std::vector<CodeFragment> chtlFragments;
    std::vector<CodeFragment> cssFragments;
    std::vector<CodeFragment> jsFragments;
    std::vector<CodeFragment> chtljsFragments;
    
    chtlFragments.push_back(CodeFragment(FragmentType::CHTL, "div { text: \"Hello\"; }"));
    cssFragments.push_back(CodeFragment(FragmentType::CSS, "body { margin: 0; }"));
    jsFragments.push_back(CodeFragment(FragmentType::JS, "console.log('Hello');"));
    
    auto output = merger->mergeFragments(chtlFragments, chtljsFragments, cssFragments, jsFragments);
    
    EXPECT_FALSE(output.html.empty());
    EXPECT_FALSE(output.css.empty());
    EXPECT_FALSE(output.js.empty());
}

TEST_F(MergerTest, InlineCSS) {
    std::vector<CodeFragment> chtlFragments;
    std::vector<CodeFragment> cssFragments;
    std::vector<CodeFragment> jsFragments;
    std::vector<CodeFragment> chtljsFragments;
    
    chtlFragments.push_back(CodeFragment(FragmentType::CHTL, "<div></div>"));
    cssFragments.push_back(CodeFragment(FragmentType::CSS, "div { color: red; }"));
    
    merger->setInlineCSS(true);
    auto output = merger->mergeFragments(chtlFragments, chtljsFragments, cssFragments, jsFragments);
    
    EXPECT_FALSE(output.html.empty());
    EXPECT_TRUE(output.css.empty()); // Should be inlined
}

TEST_F(MergerTest, InlineJS) {
    std::vector<CodeFragment> chtlFragments;
    std::vector<CodeFragment> cssFragments;
    std::vector<CodeFragment> jsFragments;
    std::vector<CodeFragment> chtljsFragments;
    
    chtlFragments.push_back(CodeFragment(FragmentType::CHTL, "<div></div>"));
    jsFragments.push_back(CodeFragment(FragmentType::JS, "console.log('Hello');"));
    
    merger->setInlineJS(true);
    auto output = merger->mergeFragments(chtlFragments, chtljsFragments, cssFragments, jsFragments);
    
    EXPECT_FALSE(output.html.empty());
    EXPECT_TRUE(output.js.empty()); // Should be inlined
}

TEST_F(MergerTest, SeparateFilesOutput) {
    std::vector<CodeFragment> chtlFragments;
    std::vector<CodeFragment> cssFragments;
    std::vector<CodeFragment> jsFragments;
    std::vector<CodeFragment> chtljsFragments;
    
    chtlFragments.push_back(CodeFragment(FragmentType::CHTL, "div { text: \"Hello\"; }"));
    cssFragments.push_back(CodeFragment(FragmentType::CSS, "body { margin: 0; }"));
    jsFragments.push_back(CodeFragment(FragmentType::JS, "console.log('Hello');"));
    
    merger->setOutputType(OutputType::SEPARATE_FILES);
    auto output = merger->mergeFragments(chtlFragments, chtljsFragments, cssFragments, jsFragments);
    
    EXPECT_FALSE(output.html.empty());
    EXPECT_EQ(output.separateFiles.size(), 2);
    EXPECT_TRUE(output.separateFiles.find("style.css") != output.separateFiles.end());
    EXPECT_TRUE(output.separateFiles.find("script.js") != output.separateFiles.end());
}

TEST_F(MergerTest, EmptyInput) {
    std::vector<CodeFragment> emptyFragments;
    
    auto output = merger->mergeFragments(emptyFragments, emptyFragments, emptyFragments, emptyFragments);
    
    EXPECT_TRUE(output.isEmpty());
}

TEST_F(MergerTest, PlaceholderRegistration) {
    merger->registerPlaceholder("_TEST_", "Hello World");
    
    std::string content = "div { text: _TEST_; }";
    std::string resolved = merger->resolvePlaceholders(content);
    
    EXPECT_EQ(resolved, "div { text: Hello World; }");
}

TEST_F(MergerTest, ClearPlaceholders) {
    merger->registerPlaceholder("_TEST_", "Hello World");
    merger->clearPlaceholders();
    
    std::string content = "div { text: _TEST_; }";
    std::string resolved = merger->resolvePlaceholders(content);
    
    // Should remain unchanged since placeholder was cleared
    EXPECT_EQ(resolved, content);
}

TEST_F(MergerTest, MultiplePlaceholders) {
    merger->registerPlaceholder("_PLACEHOLDER_1_", "Hello");
    merger->registerPlaceholder("_PLACEHOLDER_2_", "World");
    
    std::string content = "div { text: _PLACEHOLDER_1_ _PLACEHOLDER_2_; }";
    std::string resolved = merger->resolvePlaceholders(content);
    
    EXPECT_EQ(resolved, "div { text: Hello World; }");
}

TEST_F(MergerTest, TemplateVariables) {
    std::string template_ = "{{TITLE}} - {{AUTHOR}}";
    std::map<std::string, std::string> variables = {
        {"TITLE", "CHTL"},
        {"AUTHOR", "CHTL Team"}
    };
    
    std::string result = merger->processTemplate(template_, variables);
    EXPECT_EQ(result, "CHTL - CHTL Team");
}

TEST_F(MergerTest, TemplateExtraction) {
    std::string template_ = "Hello {{NAME}}, welcome to {{SITE}}!";
    auto variables = merger->extractTemplateVariables(template_);
    
    EXPECT_EQ(variables.size(), 2);
    EXPECT_TRUE(variables.find("NAME") != variables.end());
    EXPECT_TRUE(variables.find("SITE") != variables.end());
}