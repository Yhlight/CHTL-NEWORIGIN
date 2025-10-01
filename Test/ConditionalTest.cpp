#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/BaseNode.h"
#include <memory>
#include <string>

// Helper to get the generated string from a CHTL snippet
std::string generate_from_string_cond(const std::string& chtl_input) {
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(chtl_input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> root = parser.parse();
    CHTL::CHTLGenerator generator;
    return generator.generate(root.get());
}

TEST_CASE("Conditional rendering with a simple true 'if' statement", "[conditional]") {
    std::string input = R"(
        div {
            style {
                width: 100px;
            }
            if {
                condition: 1 == 1;
                display: block;
            }
        }
    )";
    std::string result = generate_from_string_cond(input);
    REQUIRE(result == R"(<div style="display: block; width: 100px; "></div>)");
}

TEST_CASE("Conditional rendering with a simple false 'if' statement", "[conditional]") {
    std::string input = R"(
        div {
            style {
                width: 100px;
            }
            if {
                condition: 1 == 0;
                display: none;
            }
        }
    )";
    std::string result = generate_from_string_cond(input);
    REQUIRE(result == R"(<div style="width: 100px; "></div>)");
}

TEST_CASE("Conditional rendering with an if-else chain (if branch taken)", "[conditional]") {
    std::string input = R"(
        div {
            if {
                condition: 10 > 5;
                color: red;
            } else {
                color: blue;
            }
        }
    )";
    std::string result = generate_from_string_cond(input);
    REQUIRE(result == R"(<div style="color: red; "></div>)");
}

TEST_CASE("Conditional rendering with an if-else chain (else branch taken)", "[conditional]") {
    std::string input = R"(
        div {
            if {
                condition: 10 < 5;
                color: red;
            } else {
                color: blue;
            }
        }
    )";
    std::string result = generate_from_string_cond(input);
    REQUIRE(result == R"(<div style="color: blue; "></div>)");
}

TEST_CASE("Conditional rendering with an if-else if-else chain", "[conditional]") {
    std::string input = R"(
        div {
            style {
                width: 10px;
            }
            if {
                condition: width == 5px;
                height: 10px;
            } else if {
                condition: width == 10px;
                height: 20px;
            }
            else {
                height: 30px;
            }
        }
    )";
    std::string result = generate_from_string_cond(input);
    REQUIRE(result == R"(<div style="height: 20px; width: 10px; "></div>)");
}

TEST_CASE("Conditional rendering with logical operators", "[conditional]") {
    std::string input = R"(
        div {
            style {
                width: 100px;
                height: 50px;
            }
            if {
                condition: width > 50px && height < 100px;
                background-color: green;
            }
        }
    )";
    std::string result = generate_from_string_cond(input);
    REQUIRE(result == R"(<div style="background-color: green; height: 50px; width: 100px; "></div>)");
}

TEST_CASE("Conditional rendering with property reference", "[conditional]") {
    std::string input = R"(
        body {
            div {
                id: box1;
                style {
                    width: 200px;
                }
            }
            div {
                id: box2;
                if {
                    condition: #box1.width > 150px;
                    height: 300px;
                }
            }
        }
    )";
    std::string result = generate_from_string_cond(input);
    REQUIRE(result == R"(<body><div id="box1" style="width: 200px; "></div><div id="box2" style="height: 300px; "></div></body>)");
}