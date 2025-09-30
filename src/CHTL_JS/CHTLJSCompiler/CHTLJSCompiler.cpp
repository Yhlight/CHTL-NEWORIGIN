#include "CHTLJSCompiler.h"
#include "../CHTLJSParser/CHTLJSParser.h"
#include "../CHTLJSGenerator/CHTLJSGenerator.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"

// The compile method now uses the new token-based parser.
std::string CHTLJSCompiler::compile(const std::string& chtl_js_source, CJMODManager& cjmodManager) {
    // 1. Instantiate the refactored parser
    CHTLJSParser parser;

    // 2. Parse the source to get the AST. The parser now handles the lexer internally.
    std::unique_ptr<CHTLJSBaseNode> ast = parser.parse(chtl_js_source);

    // If parsing fails or produces no output, return an empty string.
    if (!ast) {
        return "";
    }

    // 3. Instantiate the generator and generate the final JavaScript.
    CHTLJSGenerator generator;
    return generator.generate(*ast);
}