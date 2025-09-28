#include <cassert>
#include <string>
#include <vector>
#include <memory>

#include "CHTL_JS/CHTLLexer/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTL_JS/CHTLJSNode/CHTLJSNode.h"
#include "CHTL_JS/CHTLJSNode/ListenNode.h"
#include "CHTL_JS/CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "CHTL_JS/CHTLJSManage/VirtualObjectManager.h"

// Helper function to remove all whitespace from a string
extern std::string remove_whitespace(std::string str);

void test_placeholder_based_parsing_and_generation() {
    // This test simulates the output of an ideal Unified Scanner,
    // where raw JavaScript has been replaced by a placeholder token.
    std::vector<CHTLJSToken> tokens = {
        {CHTLJSTokenType::OpenDoubleBrace, "{{"},
        {CHTLJSTokenType::Identifier, ".my-button"},
        {CHTLJSTokenType::CloseDoubleBrace, "}}"},
        {CHTLJSTokenType::Arrow, "->"},
        {CHTLJSTokenType::Identifier, "Listen"},
        {CHTLJSTokenType::OpenBrace, "{"},
        {CHTLJSTokenType::Identifier, "click"},
        {CHTLJSTokenType::Colon, ":"},
        {CHTLJSTokenType::Placeholder, "console.log('clicked');"},
        {CHTLJSTokenType::CloseBrace, "}"},
        {CHTLJSTokenType::EndOfFile, ""}
    };

    CHTLJSParser parser(tokens);
    auto ast = parser.parse();

    CHTLJSGenerator generator;
    VirtualObjectManager vom;
    std::string generated_js = generator.generate(ast, vom);

    std::string expected_js = R"(
        document.querySelector('.my-button').addEventListener('click', () => {
            console.log('clicked');
        });
    )";

    assert(remove_whitespace(generated_js) == remove_whitespace(expected_js));
}