#include <gtest/gtest.h>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include <string>
#include <algorithm>
#include <cctype>

// Helper function to remove all whitespace for robust comparison.
std::string removeWhitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}


TEST(IntegrationTest, FullCompilationSimple) {
    std::string source = R"(
        div {
            id: "main";
            p {
                text { "Hello" }
            }
            # generator comment
        }
    )";

    std::string expected_html = R"(
        <div id="main">
          <p>
            Hello
          </p>
          <!-- generator comment -->
        </div>
    )";

    CHTL::CHTLLexer lexer(source);
    CHTL::CHTLParser parser(lexer.getAllTokens());
    auto ast = parser.parse();

    ASSERT_NE(ast, nullptr);

    CHTL::CHTLGenerator generator;
    std::string result_html = generator.generate(*ast);

    EXPECT_EQ(removeWhitespace(result_html), removeWhitespace(expected_html));
}

TEST(IntegrationTest, HandlesVoidElements) {
    std::string source = R"(
        div {
            img { src: "image.jpg"; }
            br {}
        }
    )";

    std::string expected_html = R"(
        <div>
            <img src="image.jpg">
            <br>
        </div>
    )";

    CHTL::CHTLLexer lexer(source);
    CHTL::CHTLParser parser(lexer.getAllTokens());
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result_html = generator.generate(*ast);

    EXPECT_EQ(removeWhitespace(result_html), removeWhitespace(expected_html));
}
