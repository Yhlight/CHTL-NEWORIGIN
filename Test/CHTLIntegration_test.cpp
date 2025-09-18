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

TEST(IntegrationTest, HandlesStyleBlocks) {
    std::string source = R"(
        div {
            style {
                color: red;
                font-size: 16px;
            }
        }
    )";

    // Note: The exact spacing inside the style attribute might differ,
    // so a robust comparison should ideally parse the style string.
    // For now, a string comparison after removing whitespace is sufficient.
    std::string expected_html = R"(
        <div style="color:red;font-size:16px;">
        </div>
    )";

    CHTL::CHTLLexer lexer(source);
    CHTL::CHTLParser parser(lexer.getAllTokens());
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result_html = generator.generate(*ast);

    EXPECT_EQ(removeWhitespace(result_html), removeWhitespace(expected_html));
}

TEST(IntegrationTest, HandlesPropertyArithmetic) {
    std::string source = R"(
        div {
            style {
                width: 100px + 50px;
                height: 2 * 25px;
            }
        }
    )";

    std::string expected_html = R"(
        <div style="width:150px; height:50px;">
        </div>
    )";

    CHTL::CHTLLexer lexer(source);
    CHTL::CHTLParser parser(lexer.getAllTokens());
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result_html = generator.generate(*ast);

    EXPECT_EQ(removeWhitespace(result_html), removeWhitespace(expected_html));
}

TEST(IntegrationTest, TextAttributeIsEquivalentToTextBlock) {
    std::string source_attr = R"(div { text: "hello"; })";
    std::string source_block = R"(div { text { "hello" } })";

    CHTL::CHTLLexer lexer_attr(source_attr);
    CHTL::CHTLParser parser_attr(lexer_attr.getAllTokens());
    auto ast_attr = parser_attr.parse();
    CHTL::CHTLGenerator generator;
    std::string result_attr = generator.generate(*ast_attr);

    CHTL::CHTLLexer lexer_block(source_block);
    CHTL::CHTLParser parser_block(lexer_block.getAllTokens());
    auto ast_block = parser_block.parse();
    std::string result_block = generator.generate(*ast_block);

    EXPECT_EQ(removeWhitespace(result_attr), removeWhitespace(result_block));
    EXPECT_EQ(removeWhitespace(result_attr), removeWhitespace("<div>hello</div>"));
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
