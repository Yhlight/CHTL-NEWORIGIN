#include "catch.hpp"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "TestUtil.h"
#include <memory>
#include <vector>

// Helper to get the generated string from a CHTL snippet
std::string generate_from_string_for_conditionals(const std::string& chtl_input) {
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(chtl_input);
    CHTL::CHTLParser parser(tokens);
    parser.parse();
    auto ast_nodes = parser.getAST();
    CHTL::CHTLGenerator generator;
    return remove_whitespace(generator.generate(ast_nodes));
}

TEST_CASE("Generator handles a simple 'if' block (true condition)", "[generator][conditional]") {
    std::string input = R"(
        div {
            if {
                condition: 1,
                p { text { "This should be rendered." } }
            }
        }
    )";
    std::string expected = "<html><head><style></style></head><body><div><p>Thisshouldberendered.</p></div></body></html>";
    std::string result = generate_from_string_for_conditionals(input);
    REQUIRE(result == expected);
}

TEST_CASE("Generator handles a simple 'if' block (false condition)", "[generator][conditional]") {
    std::string input = R"(
        div {
            if {
                condition: 0,
                p { text { "This should NOT be rendered." } }
            }
        }
    )";
    std::string expected = "<html><head><style></style></head><body><div></div></body></html>";
    std::string result = generate_from_string_for_conditionals(input);
    REQUIRE(result == expected);
}

TEST_CASE("Generator handles an 'if-else' block (if branch taken)", "[generator][conditional]") {
    std::string input = R"(
        div {
            if {
                condition: 1,
                p { text { "IF" } }
            } else {
                p { text { "ELSE" } }
            }
        }
    )";
    std::string expected = "<html><head><style></style></head><body><div><p>IF</p></div></body></html>";
    std::string result = generate_from_string_for_conditionals(input);
    REQUIRE(result == expected);
}

TEST_CASE("Generator handles an 'if-else' block (else branch taken)", "[generator][conditional]") {
    std::string input = R"(
        div {
            if {
                condition: 0,
                p { text { "IF" } }
            } else {
                p { text { "ELSE" } }
            }
        }
    )";
    std::string expected = "<html><head><style></style></head><body><div><p>ELSE</p></div></body></html>";
    std::string result = generate_from_string_for_conditionals(input);
    REQUIRE(result == expected);
}

TEST_CASE("Generator handles an 'if-else if-else' chain", "[generator][conditional]") {
    std::string input = R"(
        div {
            if {
                condition: 0,
                p { text { "IF" } }
            } else if {
                condition: 1,
                p { text { "ELSE IF" } }
            } else {
                p { text { "ELSE" } }
            }
        }
    )";
    std::string expected = "<html><head><style></style></head><body><div><p>ELSEIF</p></div></body></html>";
    std::string result = generate_from_string_for_conditionals(input);
    REQUIRE(result == expected);
}

TEST_CASE("Generator handles nested conditional blocks", "[generator][conditional]") {
    std::string input = R"(
        div {
            if {
                condition: 1,
                if {
                    condition: 0,
                    p { text { "INNER IF" } }
                } else {
                    p { text { "INNER ELSE" } }
                }
            }
        }
    )";
    std::string expected = "<html><head><style></style></head><body><div><p>INNERELSE</p></div></body></html>";
    std::string result = generate_from_string_for_conditionals(input);
    REQUIRE(result == expected);
}