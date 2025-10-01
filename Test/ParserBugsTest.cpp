#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"

TEST_CASE("Parse p tag with attribute and unquoted text", "[parser][bugs]")
{
    std::string input = R"(
        p {
            class: "paragraph";
            text {
                This is a paragraph with unquoted text.
            }
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    CHTL::CHTLParser parser(tokens);
    parser.parse();
    auto rootNodes = parser.getAST();
    REQUIRE(rootNodes.size() == 1);
    std::unique_ptr<CHTL::BaseNode> rootNode = std::move(rootNodes[0]);

    REQUIRE(rootNode != nullptr);
    auto *pNode = dynamic_cast<CHTL::ElementNode *>(rootNode.get());
    REQUIRE(pNode != nullptr);
    REQUIRE(pNode->getTagName() == "p");
    REQUIRE(pNode->getAttribute("class") == "paragraph");
    REQUIRE(pNode->getChildren().size() == 1);

    auto *textNode = dynamic_cast<CHTL::TextNode *>(pNode->getChildren()[0].get());
    REQUIRE(textNode != nullptr);
    REQUIRE(textNode->getValue() == "This is a paragraph with unquoted text.");
}