#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLUtil/DocumentTraverser.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include <memory>

TEST_CASE("DocumentTraverser finds elements by selector", "[traverser]") {
    std::string input = R"(
        body {
            div {
                class: "container";
                p {
                    id: "main-text";
                    text { "hello" }
                }
            }
            footer { }
        }
    )";

    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> root = parser.parse();

    SECTION("Find by tag name") {
        const CHTL::ElementNode* footer = CHTL::DocumentTraverser::find(root.get(), "footer");
        REQUIRE(footer != nullptr);
        REQUIRE(footer->getTagName() == "footer");
    }

    SECTION("Find by class name") {
        const CHTL::ElementNode* container = CHTL::DocumentTraverser::find(root.get(), ".container");
        REQUIRE(container != nullptr);
        REQUIRE(container->getAttribute("class") == "container");
    }

    SECTION("Find by ID") {
        const CHTL::ElementNode* main_text = CHTL::DocumentTraverser::find(root.get(), "#main-text");
        REQUIRE(main_text != nullptr);
        REQUIRE(main_text->getAttribute("id") == "main-text");
    }

    SECTION("Return nullptr for non-existent element") {
        const CHTL::ElementNode* not_found = CHTL::DocumentTraverser::find(root.get(), "#not-found");
        REQUIRE(not_found == nullptr);
    }
}