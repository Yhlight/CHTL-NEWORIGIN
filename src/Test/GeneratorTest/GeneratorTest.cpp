#include "gtest/gtest.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/CommentNode.h"
#include <vector>
#include <memory>

TEST(GeneratorTest, GeneratesBasicElement) {
    std::vector<std::unique_ptr<Node>> ast;
    ast.push_back(std::make_unique<ElementNode>("p"));

    CHTLGenerator generator(ast);
    std::string result = generator.generate();

    EXPECT_EQ(result, "<p></p>");
}

TEST(GeneratorTest, GeneratesElementWithAttributes) {
    std::vector<std::unique_ptr<Node>> ast;
    auto element = std::make_unique<ElementNode>("div");
    element->setAttribute("id", "\"main\"");
    element->setAttribute("class", "\"container\"");
    ast.push_back(std::move(element));

    CHTLGenerator generator(ast);
    std::string result = generator.generate();

    EXPECT_EQ(result, "<div class=\"container\" id=\"main\"></div>");
}

TEST(GeneratorTest, GeneratesTextNode) {
    std::vector<std::unique_ptr<Node>> ast;
    ast.push_back(std::make_unique<TextNode>("Hello CHTL"));

    CHTLGenerator generator(ast);
    std::string result = generator.generate();

    EXPECT_EQ(result, "Hello CHTL");
}

TEST(GeneratorTest, GeneratesQuotedTextNode) {
    std::vector<std::unique_ptr<Node>> ast;
    ast.push_back(std::make_unique<TextNode>("\"Hello Quoted CHTL\""));

    CHTLGenerator generator(ast);
    std::string result = generator.generate();

    EXPECT_EQ(result, "Hello Quoted CHTL");
}

TEST(GeneratorTest, GeneratesGeneratorComment) {
    std::vector<std::unique_ptr<Node>> ast;
    ast.push_back(std::make_unique<CommentNode>("# a generator comment"));

    CHTLGenerator generator(ast);
    std::string result = generator.generate();

    EXPECT_EQ(result, "<!-- a generator comment -->");
}

TEST(GeneratorTest, IgnoresOtherComments) {
    std::vector<std::unique_ptr<Node>> ast;
    ast.push_back(std::make_unique<CommentNode>("// a single line comment"));
    ast.push_back(std::make_unique<CommentNode>("/* a multi-line comment */"));

    CHTLGenerator generator(ast);
    std::string result = generator.generate();

    EXPECT_EQ(result, "");
}

TEST(GeneratorTest, GeneratesNestedStructure) {
    std::vector<std::unique_ptr<Node>> ast;
    auto div = std::make_unique<ElementNode>("div");
    div->addChild(std::make_unique<ElementNode>("p"));
    auto p = std::make_unique<ElementNode>("p");
    p->addChild(std::make_unique<TextNode>("Hello"));
    div->addChild(std::move(p));
    ast.push_back(std::move(div));

    CHTLGenerator generator(ast);
    std::string result = generator.generate();

    EXPECT_EQ(result, "<div><p></p><p>Hello</p></div>");
}