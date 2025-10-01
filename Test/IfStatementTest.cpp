#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/IfNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include "CHTLNode/LiteralNode.h"
#include "CHTLNode/BinaryOpNode.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLEvaluator/ExpressionEvaluator.h"

TEST_CASE("Parse If Statement", "[parser][if]") {
    std::string input = R"(
        div {
            if {
                condition: 1 > 0;
                color: red;
            }
        }
    )";

    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto* elementNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(elementNode != nullptr);
    REQUIRE(elementNode->getTagName() == "div");

    const auto& if_blocks = elementNode->getIfBlocks();
    REQUIRE(if_blocks.size() == 1);

    const auto* ifNode = if_blocks[0].get();
    REQUIRE(ifNode != nullptr);

    const auto* condition = ifNode->getCondition();
    REQUIRE(condition != nullptr);
    REQUIRE(condition->getType() == CHTL::ExpressionType::BINARY_OP);

    const auto* binOpNode = dynamic_cast<const CHTL::BinaryOpNode*>(condition);
    REQUIRE(binOpNode != nullptr);
    REQUIRE(binOpNode->getOperator().type == CHTL::TokenType::GREATER);

    const auto& body = ifNode->getBody();
    REQUIRE(body.size() == 1);
    REQUIRE(body[0]->getKey() == "color");
}

TEST_CASE("Parse If-Else Statement", "[parser][if]") {
    std::string input = R"(
        div {
            if {
                condition: 1 > 2;
                color: red;
            } else {
                color: blue;
            }
        }
    )";

    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto* elementNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(elementNode != nullptr);

    const auto& if_blocks = elementNode->getIfBlocks();
    REQUIRE(if_blocks.size() == 1);

    const auto* ifNode = if_blocks[0].get();
    REQUIRE(ifNode != nullptr);

    const auto* elseNode = ifNode->getElseBranch();
    REQUIRE(elseNode != nullptr);
    REQUIRE(elseNode->getCondition() == nullptr);

    const auto& elseBody = elseNode->getBody();
    REQUIRE(elseBody.size() == 1);
    REQUIRE(elseBody[0]->getKey() == "color");
}

TEST_CASE("Parse If-Else-If Statement", "[parser][if]") {
    std::string input = R"(
        div {
            if {
                condition: 1 > 2;
                color: red;
            } else if {
                condition: 1 < 2;
                color: green;
            } else {
                color: blue;
            }
        }
    )";

    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto* elementNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(elementNode != nullptr);

    const auto& if_blocks = elementNode->getIfBlocks();
    REQUIRE(if_blocks.size() == 1);

    const auto* ifNode = if_blocks[0].get();
    REQUIRE(ifNode != nullptr);

    const auto* elseIfNode = ifNode->getElseBranch();
    REQUIRE(elseIfNode != nullptr);
    REQUIRE(elseIfNode->getCondition() != nullptr);

    const auto& elseIfBody = elseIfNode->getBody();
    REQUIRE(elseIfBody.size() == 1);
    REQUIRE(elseIfBody[0]->getKey() == "color");

    const auto* elseNode = elseIfNode->getElseBranch();
    REQUIRE(elseNode != nullptr);
    REQUIRE(elseNode->getCondition() == nullptr);
}

TEST_CASE("Generate If Statement - True Condition", "[generator][if]") {
    std::string input = R"(
        div {
            if {
                condition: 1 > 0;
                color: red;
            }
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    auto root = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(root.get());
    REQUIRE(result.find("style=\"color: red;\"") != std::string::npos);
}

TEST_CASE("Generate If-Else Statement - Else Branch", "[generator][if]") {
    std::string input = R"(
        div {
            if {
                condition: 1 > 2;
                color: red;
            } else {
                color: blue;
            }
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    auto root = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(root.get());
    REQUIRE(result.find("style=\"color: blue;\"") != std::string::npos);
}

TEST_CASE("Generate If-Else-If-Else Statement - Else If Branch", "[generator][if]") {
    std::string input = R"(
        div {
            if {
                condition: 1 > 2;
                color: red;
            } else if {
                condition: 2 > 1;
                color: green;
            } else {
                color: blue;
            }
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    auto root = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(root.get());
    REQUIRE(result.find("style=\"color: green;\"") != std::string::npos);
}