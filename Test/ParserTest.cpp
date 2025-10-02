#include <gtest/gtest.h>
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ScriptNode.h"

TEST(ParserTest, ParsesSingleEmptyElement) {
    std::string input = "div {}";
    CHTLParser parser(input);
    parser.parse();

    BaseNode* root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(root->getChildren().size(), 1);

    BaseNode* element = root->getChildren()[0].get();
    ASSERT_EQ(element->getType(), NodeType::Element);

    auto* elementNode = static_cast<ElementNode*>(element);
    EXPECT_EQ(elementNode->getTagName(), "div");
}

TEST(ParserTest, ParsesStyleBlock) {
    std::string input = "div { style { color: red; } }";
    CHTLParser parser(input);
    parser.parse();

    BaseNode* root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(root->getChildren().size(), 1);

    BaseNode* divNode = root->getChildren()[0].get();
    ASSERT_EQ(divNode->getType(), NodeType::Element);
    ASSERT_EQ(divNode->getChildren().size(), 1);

    BaseNode* styleNodeBase = divNode->getChildren()[0].get();
    ASSERT_EQ(styleNodeBase->getType(), NodeType::Style);

    auto* styleNode = static_cast<StyleNode*>(styleNodeBase);
    EXPECT_EQ(styleNode->getStyle(), " color: red; ");
}

TEST(ParserTest, ParsesScriptBlock) {
    std::string input = "div { script { console.log(1); } }";
    CHTLParser parser(input);
    parser.parse();

    BaseNode* root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(root->getChildren().size(), 1);

    BaseNode* divNode = root->getChildren()[0].get();
    ASSERT_EQ(divNode->getType(), NodeType::Element);
    ASSERT_EQ(divNode->getChildren().size(), 1);

    BaseNode* scriptNodeBase = divNode->getChildren()[0].get();
    ASSERT_EQ(scriptNodeBase->getType(), NodeType::Script);

    auto* scriptNode = static_cast<ScriptNode*>(scriptNodeBase);
    EXPECT_EQ(scriptNode->getScript(), " console.log(1); ");
}