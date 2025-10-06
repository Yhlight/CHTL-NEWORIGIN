#include "../third-party/catch.hpp"
#include "../CHTLJS/CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"
#include <memory>
#include <vector>
#include <string>

std::string generate_cjs_output(const std::string& input) {
    CHTLJS::CHTLJSLexer lexer(input);
    std::vector<CHTLJS::Token> tokens;
    CHTLJS::Token token = lexer.getNextToken();
    while (token.type != CHTLJS::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }

    CHTLJS::CHTLJSParser parser(tokens);
    auto root = parser.parse();

    CHTLJS::CHTLJSGenerator generator;
    return generator.generate(root);
}

TEST_CASE("CHTLJS ScriptLoader with single script", "[chtljs][scriptloader]") {
    std::string input = R"(
        ScriptLoader {
            load: "./module.js"
        }
    )";
    std::string expected_output = "require(\"./module.js\");\n";
    REQUIRE(generate_cjs_output(input) == expected_output);
}

TEST_CASE("CHTLJS ScriptLoader with multiple load statements", "[chtljs][scriptloader]") {
    std::string input = R"(
        ScriptLoader {
            load: "./module1.js",
            load: "./module2.js"
        }
    )";
    std::string expected_output = "require(\"./module1.js\");\nrequire(\"./module2.js\");\n";
    REQUIRE(generate_cjs_output(input) == expected_output);
}

TEST_CASE("CHTLJS ScriptLoader with multiple comma-separated paths", "[chtljs][scriptloader]") {
    std::string input = R"(
        ScriptLoader {
            load: ./module1.js, ./module2.js, ./module3.js
        }
    )";
    std::string expected_output = "require(\"./module1.js\");\nrequire(\"./module2.js\");\nrequire(\"./module3.js\");\n";
    REQUIRE(generate_cjs_output(input) == expected_output);
}

TEST_CASE("CHTLJS ScriptLoader with unquoted literals", "[chtljs][scriptloader]") {
    std::string input = R"(
        ScriptLoader {
            load: ./module.js
        }
    )";
    std::string expected_output = "require(\"./module.js\");\n";
    REQUIRE(generate_cjs_output(input) == expected_output);
}

TEST_CASE("CHTLJS ScriptLoader with mixed literals", "[chtljs][scriptloader]") {
    std::string input = R"(
        ScriptLoader {
            load: ./module1.js, "./module2.js"
        }
    )";
    std::string expected_output = "require(\"./module1.js\");\nrequire(\"./module2.js\");\n";
    REQUIRE(generate_cjs_output(input) == expected_output);
}