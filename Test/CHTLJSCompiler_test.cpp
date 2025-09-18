#include <gtest/gtest.h>
#include "CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include "CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"

// Helper function to remove all whitespace for robust comparison.
static std::string removeWhitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

TEST(CHTLJSCompilerTest, TranslatesEnhancedSelector) {
    std::string source = "const my_div = {{.my-class}};";

    // Expected JS output
    std::string expected_js = "const my_div = document.querySelector('.my-class');";

    // --- Run the CHTL JS pipeline ---
    CHTL_JS::CHTLJSLexer lexer(source);
    CHTL_JS::CHTLJSParser parser(lexer.getAllTokens());
    auto ast = parser.parse();
    CHTL_JS::CHTLJSGenerator generator;
    std::string result_js = generator.generate(ast);

    EXPECT_EQ(removeWhitespace(result_js), removeWhitespace(expected_js));
}

TEST(CHTLJSCompilerTest, PreservesRawJS) {
    std::string source = "let x = 10; if (x > 5) { console.log('hello'); }";

    // --- Run the CHTL JS pipeline ---
    CHTL_JS::CHTLJSLexer lexer(source);
    CHTL_JS::CHTLJSParser parser(lexer.getAllTokens());
    auto ast = parser.parse();
    CHTL_JS::CHTLJSGenerator generator;
    std::string result_js = generator.generate(ast);

    EXPECT_EQ(removeWhitespace(result_js), removeWhitespace(source));
}
