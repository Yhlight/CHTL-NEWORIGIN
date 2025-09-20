#include <gtest/gtest.h>
#include "CHTLCommon.h"
#include "CodeMerger.h"

using namespace CHTL;

class GeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        merger = std::make_unique<CodeMerger>();
    }
    
    void TearDown() override {
        merger.reset();
    }
    
    std::unique_ptr<CodeMerger> merger;
};

TEST_F(GeneratorTest, BasicCodeMerger) {
    std::vector<CodeFragment> fragments;
    
    CodeFragment fragment(FragmentType::CHTL, "div { text: \"Hello\"; }");
    fragments.push_back(fragment);
    
    auto output = merger->merge(fragments);
    
    EXPECT_FALSE(output.isEmpty());
    // Basic structure should be present
    EXPECT_FALSE(output.html.empty());
}

TEST_F(GeneratorTest, FragmentTypeSeparation) {
    std::vector<CodeFragment> fragments;
    
    CodeFragment chtlFragment(FragmentType::CHTL, "div { text: \"Hello\"; }");
    CodeFragment cssFragment(FragmentType::CSS, "body { margin: 0; }");
    CodeFragment jsFragment(FragmentType::JS, "console.log('Hello');");
    
    fragments.push_back(chtlFragment);
    fragments.push_back(cssFragment);
    fragments.push_back(jsFragment);
    
    auto output = merger->merge(fragments);
    
    EXPECT_FALSE(output.html.empty());
    EXPECT_FALSE(output.css.empty());
    EXPECT_FALSE(output.js.empty());
}

TEST_F(GeneratorTest, InlineOptions) {
    std::vector<CodeFragment> fragments;
    
    CodeFragment chtlFragment(FragmentType::CHTL, "div { text: \"Hello\"; }");
    CodeFragment cssFragment(FragmentType::CSS, "body { margin: 0; }");
    
    fragments.push_back(chtlFragment);
    fragments.push_back(cssFragment);
    
    merger->setInlineCSS(true);
    auto output = merger->merge(fragments);
    
    // CSS should be inlined into HTML
    EXPECT_FALSE(output.html.empty());
    EXPECT_TRUE(output.css.empty()); // Should be inlined
}

TEST_F(GeneratorTest, PlaceholderManagement) {
    merger->registerPlaceholder("_PLACEHOLDER_1_", "Hello World");
    
    std::string content = "div { text: _PLACEHOLDER_1_; }";
    std::string resolved = merger->resolvePlaceholders(content);
    
    EXPECT_EQ(resolved, "div { text: Hello World; }");
}

TEST_F(GeneratorTest, TemplateProcessing) {
    std::string template_ = "Hello {{NAME}}, welcome to {{SITE}}!";
    std::map<std::string, std::string> variables = {
        {"NAME", "John"},
        {"SITE", "CHTL"}
    };
    
    std::string result = merger->processTemplate(template_, variables);
    EXPECT_EQ(result, "Hello John, welcome to CHTL!");
}

TEST_F(GeneratorTest, OutputFormatting) {
    std::string html = "<div><span>Hello</span></div>";
    std::string formatted = merger->formatHTML(html);
    
    // Formatting might add indentation or other formatting
    EXPECT_FALSE(formatted.empty());
}

TEST_F(GeneratorTest, EmptyFragments) {
    std::vector<CodeFragment> emptyFragments;
    
    auto output = merger->merge(emptyFragments);
    
    // Should handle empty input gracefully
    EXPECT_TRUE(output.isEmpty());
}

TEST_F(GeneratorTest, UnknownFragmentType) {
    std::vector<CodeFragment> fragments;
    
    CodeFragment unknownFragment(FragmentType::UNKNOWN, "some content");
    fragments.push_back(unknownFragment);
    
    auto output = merger->merge(fragments);
    
    // Should handle unknown fragment types gracefully
    EXPECT_TRUE(output.isEmpty());
}

TEST_F(GeneratorTest, DefaultTemplates) {
    std::string htmlTemplate = merger->getDefaultHTMLTemplate();
    std::string cssTemplate = merger->getDefaultCSSTemplate();
    std::string jsTemplate = merger->getDefaultJSTemplate();
    
    EXPECT_FALSE(htmlTemplate.empty());
    EXPECT_FALSE(cssTemplate.empty());
    EXPECT_FALSE(jsTemplate.empty());
    
    // Templates should contain placeholder markers
    EXPECT_TRUE(htmlTemplate.find("{{") != std::string::npos);
    EXPECT_TRUE(cssTemplate.find("{{") != std::string::npos);
    EXPECT_TRUE(jsTemplate.find("{{") != std::string::npos);
}