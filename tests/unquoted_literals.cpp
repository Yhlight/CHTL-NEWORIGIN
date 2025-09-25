#include <iostream>
#include <string>
#include <vector>
#include <cassert>

// Include all necessary CHTL headers
#include "../src/CHTL/CHTLLexer/Lexer.h"
#include "../src/CHTL/CHTLParser/Parser.h"
#include "../src/CHTL/CHTLGenerator/Generator.h"

// Test for basic unquoted literal in a text block
void test_unquoted_in_text_block() {
    std::string source = R"(
        div {
            text { Hello world this is unquoted }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("Hello world this is unquoted") != std::string::npos);
}

// Test for unquoted literal in an element attribute
void test_unquoted_in_attribute() {
    std::string source = R"(
        div {
            class: my-class-name;
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("class=\"my-class-name\"") != std::string::npos);
}

// Test for unquoted literal in a style block
void test_unquoted_in_style_block() {
    std::string source = R"(
        div {
            style {
                font-family: Arial Black;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("font-family: Arial Black;") != std::string::npos);
}

// Test for unquoted literal with numbers and hyphens
void test_unquoted_with_numbers_and_hyphens() {
    std::string source = R"(
        div {
            style {
                border: 1px solid black;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("border: 1px solid black;") != std::string::npos);
}

// Test that unquoted literal parsing stops correctly at a semicolon
void test_unquoted_stops_at_semicolon() {
    std::string source = R"(
        div {
            style {
                color: red;
                background-color: blue;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("color: red;") != std::string::npos);
    assert(result.find("background-color: blue;") != std::string::npos);
}

// Test unquoted literal followed by an arithmetic expression
void test_unquoted_with_arithmetic() {
    std::string source = R"(
        div {
            style {
                padding: 10px + 5px;
                margin: 10px;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("padding: 15px;") != std::string::npos);
    assert(result.find("margin: 10px;") != std::string::npos);
}

// Test for the power operator (**)
void test_power_operator() {
    std::string source = R"(
        div {
            style {
                width: 2**8px;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("width: 256px;") != std::string::npos);
}

// Test for the modulo operator (%)
void test_modulo_operator() {
    std::string source = R"(
        div {
            style {
                width: 10 % 3px;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("width: 1px;") != std::string::npos);
}

// Test for operator precedence
void test_operator_precedence() {
    std::string source = R"(
        div {
            style {
                width: 2 + 3 * 2 ** 3px;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    // 2 + 3 * 8 = 2 + 24 = 26
    assert(result.find("width: 26px;") != std::string::npos);
}