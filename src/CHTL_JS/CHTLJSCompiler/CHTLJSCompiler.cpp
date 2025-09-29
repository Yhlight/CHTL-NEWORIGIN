#include "CHTLJSCompiler.h"
#include "../CHTLJSNode/CHTLJSNode.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"

// The compile method is simplified to match the actual Parser and Generator interfaces.
// The previous implementation was based on an outdated, non-existent API.
std::string CHTLJSCompiler::compile(const std::string& chtl_js_source) {
    // 1. Create a parser.
    CHTLJSParser parser;

    // 2. Parse the source to get the AST. The parser's interface returns a single root node.
    std::unique_ptr<CHTLJSBaseNode> ast = parser.parse(chtl_js_source);

    // If parsing fails or the source is empty, return an empty string.
    if (!ast) {
        return "";
    }

    // 3. The generator takes the single root node and returns the compiled JavaScript.
    return generator.generate(*ast);
}