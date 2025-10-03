#include <gtest/gtest.h>
#include "Util/CssGenerator/CssGenerator.h"
#include "Util/GlobalStylesheet/GlobalStylesheet.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ContextSelectorNode.h"
#include "CHTLNode/StylePropertyNode.h"

TEST(CssGeneratorTest, GeneratesCssForContextualSelector) {
    // Manually construct the AST
    auto root = std::make_unique<ElementNode>("div");
    root->setAttribute("class", "my-div");
    auto styleNode = std::make_unique<StyleNode>();
    auto contextNode = std::make_unique<ContextSelectorNode>("&:hover");
    contextNode->addChild(std::make_unique<StylePropertyNode>("color", "red"));
    styleNode->addChild(std::move(contextNode));
    root->addChild(std::move(styleNode));

    // Run the generator
    GlobalStylesheet stylesheet;
    CssGenerator cssGenerator;
    cssGenerator.generate(root.get(), stylesheet);

    // Verify the result
    std::string expected_css = ".my-div:hover { color: red; }\n";
    EXPECT_EQ(stylesheet.getStylesheet(), expected_css);
}

TEST(CssGeneratorTest, HandlesIdForContext) {
    // Manually construct the AST
    auto root = std::make_unique<ElementNode>("div");
    root->setAttribute("id", "my-id");
    auto styleNode = std::make_unique<StyleNode>();
    auto contextNode = std::make_unique<ContextSelectorNode>("&::before");
    contextNode->addChild(std::make_unique<StylePropertyNode>("content", "\"hello\""));
    styleNode->addChild(std::move(contextNode));
    root->addChild(std::move(styleNode));

    // Run the generator
    GlobalStylesheet stylesheet;
    CssGenerator cssGenerator;
    cssGenerator.generate(root.get(), stylesheet);

    // Verify the result
    std::string expected_css = "#my-id::before { content: \"hello\"; }\n";
    EXPECT_EQ(stylesheet.getStylesheet(), expected_css);
}

TEST(CssGeneratorTest, HandlesMultipleClassesForContext) {
    // Manually construct the AST
    auto root = std::make_unique<ElementNode>("div");
    root->setAttribute("class", "class1 class2");
    auto styleNode = std::make_unique<StyleNode>();
    auto contextNode = std::make_unique<ContextSelectorNode>("&:active");
    contextNode->addChild(std::make_unique<StylePropertyNode>("font-weight", "bold"));
    styleNode->addChild(std::move(contextNode));
    root->addChild(std::move(styleNode));

    // Run the generator
    GlobalStylesheet stylesheet;
    CssGenerator cssGenerator;
    cssGenerator.generate(root.get(), stylesheet);

    // Verify the result
    std::string expected_css = ".class1.class2:active { font-weight: bold; }\n";
    EXPECT_EQ(stylesheet.getStylesheet(), expected_css);
}