#define CATCH_CONFIG_MAIN
#include "third-party/catch.hpp"
#include "chtl-compiler/CHTL/CHTLLexer/Lexer.h"
#include "chtl-compiler/CHTL/CHTLParser/Parser.h"
#include "chtl-compiler/CHTL/CHTLNode/ElementNode.h"
#include "chtl-compiler/CHTL/CHTLNode/TextNode.h"
#include "chtl-compiler/CHTL/CHTLNode/CommentNode.h"
#include "chtl-compiler/CHTL/CHTLGenerator/Generator.h"
#include "chtl-compiler/CHTL/Loader/Loader.h"
#include "chtl-compiler/CHTL/Expression/ExpressionParser.h"
#include "chtl-compiler/CHTL/Expression/ExprNode.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// Helper function to create a parser instance for tests that don't need file loading.
Parser create_test_parser(Lexer& lexer, Loader& loader) {
    return Parser(lexer, loader, "test.chtl");
}

TEST_CASE("New Compiler - Lexer", "[new_compiler_lexer]") {
    // Lexer tests are fine and don't need changes.
    SECTION("Tokenizes a simple structure") {
        std::string source = "div { text { \"Hello\" } }";
        Lexer lexer(source);
        REQUIRE(lexer.getNextToken().type == TokenType::Identifier);
        REQUIRE(lexer.getNextToken().type == TokenType::LeftBrace);
        REQUIRE(lexer.getNextToken().type == TokenType::Identifier);
        REQUIRE(lexer.getNextToken().type == TokenType::LeftBrace);
        REQUIRE(lexer.getNextToken().type == TokenType::StringLiteral);
        REQUIRE(lexer.getNextToken().type == TokenType::RightBrace);
        REQUIRE(lexer.getNextToken().type == TokenType::RightBrace);
        REQUIRE(lexer.getNextToken().type == TokenType::EndOfFile);
    }
}

TEST_CASE("New Compiler - Parser", "[new_compiler_parser]") {
    SECTION("Parses an element with attributes") {
        std::string source = "div { id = \"main\"; class: box; }";
        Lexer lexer(source);
        Loader loader;
        Parser parser = create_test_parser(lexer, loader);
        NodeList nodes = parser.parse();
        REQUIRE(nodes.size() == 1);
        auto element = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(element->attributes["id"] == "main");
        REQUIRE(element->attributes["class"] == "box");
    }
}

TEST_CASE("New Compiler - Style Blocks and Expressions", "[new_compiler_style_expressions]") {
    SECTION("Parses a style block with expressions") {
        std::string source = R"(
            div { style { width: 100px + 50px; color: red; } }
        )";
        Lexer lexer(source);
        Loader loader;
        Parser parser = create_test_parser(lexer, loader);
        NodeList nodes = parser.parse();
        REQUIRE(nodes.size() == 1);
        auto element = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        const std::string& style = element->attributes["style"];
        REQUIRE(style.find("width:150px;") != std::string::npos);
        REQUIRE(style.find("color:red;") != std::string::npos);
    }

    SECTION("Parses and evaluates a conditional style property") {
        std::string source = R"(
            div {
                style {
                    width: 100px;
                    height: width * 2;
                    background-color: width > 50px ? "red" : "blue";
                }
            }
        )";
        Lexer lexer(source);
        Loader loader;
        Parser parser = create_test_parser(lexer, loader);
        NodeList nodes = parser.parse();
        REQUIRE(nodes.size() == 1);
        auto element = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        const std::string& style = element->attributes["style"];
        REQUIRE(style.find("width:100px;") != std::string::npos);
        REQUIRE(style.find("height:200px;") != std::string::npos);
        REQUIRE(style.find("background-color:red;") != std::string::npos);
    }
}

TEST_CASE("New Compiler - Templates (Style and Element)", "[new_compiler_templates]") {
    SECTION("Specializes a custom style template with valueless properties and deletion") {
        std::string source = R"(
            [Custom] @Style BaseButton {
                padding: 10px 15px;
                background-color;
                color: white;
            }
            button {
                style {
                    @Style BaseButton {
                        background-color: blue;
                        delete color;
                    }
                }
            }
        )";
        Lexer lexer(source);
        Loader loader;
        Parser parser = create_test_parser(lexer, loader);
        NodeList nodes = parser.parse();
        REQUIRE(nodes.size() == 1);
        auto btn = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        const std::string& style = btn->attributes["style"];
        REQUIRE(style.find("background-color:blue;") != std::string::npos);
        REQUIRE(style.find("padding:10px 15px;") != std::string::npos);
        REQUIRE(style.find("color:white;") == std::string::npos);
    }

    SECTION("Specializes an element template with styling, insertion, and deletion") {
        std::string source = R"(
            [Template] @Element MyComponent {
                header { }
                main { }
                footer { }
            }
            body {
                @Element MyComponent {
                    header { style { color: red; } }
                    delete footer;
                    insert after main { p { } }
                }
            }
        )";
        Lexer lexer(source);
        Loader loader;
        Parser parser = create_test_parser(lexer, loader);
        NodeList nodes = parser.parse();
        REQUIRE(nodes.size() == 1);
        auto body = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(body->children.size() == 3);
        auto header = std::dynamic_pointer_cast<ElementNode>(body->children[0]);
        REQUIRE(header->attributes["style"] == "color:red;");
    }
}


TEST_CASE("New Compiler - Import System", "[new_compiler_import]") {
    SECTION("Successfully imports and uses templates from another file") {
        std::string base_path = TOSTRING(TEST_SOURCE_DIR);
        if (base_path.front() == '"' && base_path.back() == '"') {
            base_path = base_path.substr(1, base_path.length() - 2);
        }
        Loader loader(base_path);
        std::string entrypoint = "chtl-compiler/Test/test_files/main.chtl";
        std::string source = loader.loadFile(entrypoint);
        Lexer lexer(source);
        Parser parser(lexer, loader, entrypoint);
        NodeList nodes = parser.parse();
        REQUIRE(nodes.size() == 1);
        auto body = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(body->children.size() == 2);
        auto p = std::dynamic_pointer_cast<ElementNode>(body->children[1]);
        REQUIRE(p->attributes["style"].find("font-family:Arial;") != std::string::npos);
    }

    SECTION("Detects and throws an error for circular imports") {
        std::string base_path = TOSTRING(TEST_SOURCE_DIR);
        if (base_path.front() == '"' && base_path.back() == '"') {
            base_path = base_path.substr(1, base_path.length() - 2);
        }
        Loader loader(base_path);
        std::string entrypoint = "chtl-compiler/Test/test_files/circular_a.chtl";
        REQUIRE_THROWS_WITH(
            [&]() {
                std::string source = loader.loadFile(entrypoint);
                Lexer lexer(source);
                Parser parser(lexer, loader, entrypoint);
                parser.parse();
            }(),
            Catch::Matchers::Contains("Circular dependency detected")
        );
    }
}