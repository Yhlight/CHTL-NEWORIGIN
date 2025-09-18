#include <gtest/gtest.h>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLContext.h"

TEST(ParserTest, ParsesSimpleElement) {
    std::string source = "html {}";
    CHTL::CHTLLexer lexer(source);
    auto context = std::make_shared<CHTL::CHTLContext>();
    CHTL::CHTLParser parser(lexer.getAllTokens(), context);
    auto root = parser.parse();

    ASSERT_NE(root, nullptr);
    ASSERT_EQ(root->children.size(), 1);

    auto* htmlNode = dynamic_cast<CHTL::ElementNode*>(root->children[0].get());
    ASSERT_NE(htmlNode, nullptr);
    EXPECT_EQ(htmlNode->tagName, "html");
    EXPECT_EQ(htmlNode->children.size(), 0);
    EXPECT_EQ(htmlNode->attributes.size(), 0);
}

TEST(ParserTest, ParsesNestedElements) {
    std::string source = "div { span {} }";
    CHTL::CHTLLexer lexer(source);
    auto context = std::make_shared<CHTL::CHTLContext>();
    CHTL::CHTLParser parser(lexer.getAllTokens(), context);
    auto root = parser.parse();

    ASSERT_NE(root, nullptr);
    ASSERT_EQ(root->children.size(), 1);
    auto* divNode = dynamic_cast<CHTL::ElementNode*>(root->children[0].get());
    ASSERT_NE(divNode, nullptr);
    ASSERT_EQ(divNode->children.size(), 1);

    auto* spanNode = dynamic_cast<CHTL::ElementNode*>(divNode->children[0].get());
    ASSERT_NE(spanNode, nullptr);
    EXPECT_EQ(spanNode->tagName, "span");
}

TEST(ParserTest, ParsesAttributes) {
    std::string source = "a { href: \"/index.html\"; }";
    CHTL::CHTLLexer lexer(source);
    auto context = std::make_shared<CHTL::CHTLContext>();
    CHTL::CHTLParser parser(lexer.getAllTokens(), context);
    auto root = parser.parse();

    ASSERT_NE(root, nullptr);
    ASSERT_EQ(root->children.size(), 1);
    auto* aNode = dynamic_cast<CHTL::ElementNode*>(root->children[0].get());
    ASSERT_NE(aNode, nullptr);
    ASSERT_EQ(aNode->attributes.size(), 1);
    EXPECT_EQ(aNode->attributes[0].key, "href");
    EXPECT_EQ(aNode->attributes[0].value, "/index.html");
}

TEST(ParserTest, ThrowsOnMalformedInput) {
    std::string source = "div {"; // Missing closing brace
    CHTL::CHTLLexer lexer(source);
    auto context = std::make_shared<CHTL::CHTLContext>();
    CHTL::CHTLParser parser(lexer.getAllTokens(), context);

    EXPECT_THROW(parser.parse(), std::runtime_error);
}
