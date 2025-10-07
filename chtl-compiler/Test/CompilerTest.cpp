#define CATCH_CONFIG_MAIN
#include "third-party/catch.hpp"
#include "chtl-compiler/CHTL/CHTLLexer/Lexer.h"
#include "chtl-compiler/CHTL/CHTLParser/Parser.h"
#include "chtl-compiler/CHTL/CHTLNode/ElementNode.h"
#include "chtl-compiler/CHTL/CHTLNode/TextNode.h"
#include "chtl-compiler/CHTL/CHTLGenerator/Generator.h"

TEST_CASE("New Compiler - Lexer", "[new_compiler_lexer]") {
    SECTION("Tokenizes a simple structure") {
        std::string source = "div { text { \"Hello\" } }";
        Lexer lexer(source);

        Token t1 = lexer.getNextToken();
        REQUIRE(t1.type == TokenType::Identifier);
        REQUIRE(t1.value == "div");

        Token t2 = lexer.getNextToken();
        REQUIRE(t2.type == TokenType::LeftBrace);

        Token t3 = lexer.getNextToken();
        REQUIRE(t3.type == TokenType::Identifier);
        REQUIRE(t3.value == "text");

        Token t4 = lexer.getNextToken();
        REQUIRE(t4.type == TokenType::LeftBrace);

        Token t5 = lexer.getNextToken();
        REQUIRE(t5.type == TokenType::StringLiteral);
        REQUIRE(t5.value == "Hello");

        Token t6 = lexer.getNextToken();
        REQUIRE(t6.type == TokenType::RightBrace);

        Token t7 = lexer.getNextToken();
        REQUIRE(t7.type == TokenType::RightBrace);

        Token t8 = lexer.getNextToken();
        REQUIRE(t8.type == TokenType::EndOfFile);
    }

    SECTION("Tokenizes comments") {
        std::string source = "// line\n/* block */\n# generator";
        Lexer lexer(source);

        Token t1 = lexer.getNextToken();
        REQUIRE(t1.type == TokenType::LineComment);
        REQUIRE(t1.value == " line");

        Token t2 = lexer.getNextToken();
        REQUIRE(t2.type == TokenType::BlockComment);
        REQUIRE(t2.value == " block ");

        Token t3 = lexer.getNextToken();
        REQUIRE(t3.type == TokenType::GeneratorComment);
        REQUIRE(t3.value == "generator");
    }
}

TEST_CASE("New Compiler - Parser", "[new_compiler_parser]") {
    SECTION("Parses a simple element") {
        std::string source = "div { }";
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList nodes = parser.parse();

        REQUIRE(nodes.size() == 1);
        auto element = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(element != nullptr);
        REQUIRE(element->tagName == "div");
        REQUIRE(element->children.empty());
    }

    SECTION("Parses a nested element") {
        std::string source = "div { p { } }";
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList nodes = parser.parse();

        REQUIRE(nodes.size() == 1);
        auto parent = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(parent != nullptr);
        REQUIRE(parent->tagName == "div");
        REQUIRE(parent->children.size() == 1);

        auto child = std::dynamic_pointer_cast<ElementNode>(parent->children[0]);
        REQUIRE(child != nullptr);
        REQUIRE(child->tagName == "p");
    }

    SECTION("Parses an element with text") {
        std::string source = "p { text { \"hello\" } }";
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList nodes = parser.parse();

        REQUIRE(nodes.size() == 1);
        auto parent = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(parent != nullptr);
        REQUIRE(parent->tagName == "p");
        REQUIRE(parent->children.size() == 1);

        auto child = std::dynamic_pointer_cast<TextNode>(parent->children[0]);
        REQUIRE(child != nullptr);
        REQUIRE(child->content == "hello");
    }

    SECTION("Parses multiple root elements") {
        std::string source = "p { } div { }";
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList nodes = parser.parse();

        REQUIRE(nodes.size() == 2);
        auto p_element = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(p_element != nullptr);
        REQUIRE(p_element->tagName == "p");

        auto div_element = std::dynamic_pointer_cast<ElementNode>(nodes[1]);
        REQUIRE(div_element != nullptr);
        REQUIRE(div_element->tagName == "div");
    }

    SECTION("Parses an element with attributes") {
        std::string source = "div { id = \"main\"; class: box; }";
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList nodes = parser.parse();

        REQUIRE(nodes.size() == 1);
        auto element = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(element != nullptr);
        REQUIRE(element->tagName == "div");
        REQUIRE(element->attributes.size() == 2);
        REQUIRE(element->attributes["id"] == "main");
        REQUIRE(element->attributes["class"] == "box");
    }

    SECTION("Parses and handles comments") {
        std::string source = R"(
            // This should be ignored
            div {
                # This is a generator comment
                p { text { "hello" } } // Another ignored comment
            }
            /* This block
               should also
               be ignored */
        )";
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList nodes = parser.parse();

        REQUIRE(nodes.size() == 1);
        auto div = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(div != nullptr);
        REQUIRE(div->tagName == "div");
        REQUIRE(div->children.size() == 2);

        auto comment = std::dynamic_pointer_cast<CommentNode>(div->children[0]);
        REQUIRE(comment != nullptr);
        REQUIRE(comment->content == "This is a generator comment");

        auto p = std::dynamic_pointer_cast<ElementNode>(div->children[1]);
        REQUIRE(p != nullptr);
        REQUIRE(p->tagName == "p");
    }
}

TEST_CASE("New Compiler - Generator", "[new_compiler_generator]") {
    SECTION("Generates an element with attributes") {
        std::string source = "a { href=\"https://www.example.com\"; text { \"Click me\" } }";
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList nodes = parser.parse();
        Generator generator;
        std::string html = generator.generate(nodes);

        REQUIRE(html == "<a href=\"https://www.example.com\">Click me</a>");
    }

    SECTION("Generates HTML with comments") {
        std::string source = "div { #this is a comment\n }";
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList nodes = parser.parse();
        Generator generator;
        std::string html = generator.generate(nodes);

        REQUIRE(html == "<div><!--this is a comment--></div>");
    }
}

TEST_CASE("New Compiler - Style Blocks", "[new_compiler_style]") {
    SECTION("Parses a style block and adds it as an attribute") {
        std::string source = R"(
            div {
                style {
                    width: 100px + 50px;
                    color: red;
                }
            }
        )";
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList nodes = parser.parse();

        REQUIRE(nodes.size() == 1);
        auto element = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(element != nullptr);
        REQUIRE(element->attributes.count("style") == 1);
        REQUIRE(element->attributes["style"] == "width:150px;color:red;");
    }
}

TEST_CASE("New Compiler - Style Templates", "[new_compiler_style_template]") {
    SECTION("Defines and uses a style template") {
        std::string source = R"(
            [Template] @Style DefaultText {
                font-size: 16px;
                color: black;
            }

            p {
                style {
                    @Style DefaultText;
                    font-weight: bold;
                }
            }
        )";
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList nodes = parser.parse();

        REQUIRE(nodes.size() == 1);
        auto p_element = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(p_element != nullptr);
        REQUIRE(p_element->attributes.count("style") == 1);

        const std::string& style = p_element->attributes["style"];
        REQUIRE(style.find("font-size:16px;") != std::string::npos);
        REQUIRE(style.find("color:black;") != std::string::npos);
        REQUIRE(style.find("font-weight:bold;") != std::string::npos);
    }

    SECTION("Specializes a custom style template with valueless properties and deletion") {
        std::string source = R"(
            [Custom] @Style BaseButton {
                padding: 10px 15px;
                border-radius: 5px;
                background-color; // Valueless property
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
        Parser parser(lexer);
        NodeList nodes = parser.parse();

        REQUIRE(nodes.size() == 1);
        auto btn = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(btn != nullptr);
        REQUIRE(btn->attributes.count("style") == 1);

        const std::string& style = btn->attributes["style"];
        REQUIRE(style.find("background-color:blue;") != std::string::npos);
        REQUIRE(style.find("border-radius:5px;") != std::string::npos);
        REQUIRE(style.find("padding:10px 15px;") != std::string::npos);
        REQUIRE(style.find("color:white;") == std::string::npos);
    }

    SECTION("Throws an error for unspecialized valueless property") {
        std::string source = R"(
            [Custom] @Style BadButton {
                color;
            }
            button { style { @Style BadButton; } }
        )";
        Lexer lexer(source);
        Parser parser(lexer);
        REQUIRE_THROWS(parser.parse());
    }
}

TEST_CASE("New Compiler - Expression Evaluator", "[new_compiler_expression]") {
    SECTION("Evaluates simple arithmetic expressions") {
        REQUIRE(ExpressionEvaluator::evaluate("10px", "+", "5px") == "15px");
        REQUIRE(ExpressionEvaluator::evaluate("20", "-", "5") == "15");
        REQUIRE(ExpressionEvaluator::evaluate("10", "*", "2.5") == "25");
        REQUIRE(ExpressionEvaluator::evaluate("100%", "/", "2") == "50%");
    }

    SECTION("Handles mixed units correctly") {
        REQUIRE(ExpressionEvaluator::evaluate("10.5em", "+", "2") == "12.5em");
        REQUIRE_THROWS(ExpressionEvaluator::evaluate("10px", "+", "5em"));
    }
}