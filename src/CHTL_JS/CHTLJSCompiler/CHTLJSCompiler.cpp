#include "CHTLJSCompiler.h"
#include "../CHTLJSNode/CHTLJSNode.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"

// The compile method is simplified to match the actual Parser and Generator interfaces.
// The previous implementation was based on an outdated, non-existent API.
std::string CHTLJSCompiler::compile(const std::string& chtl_js_source, CJMODManager& cjmodManager) {
    // This is a simplified proof-of-concept implementation. A real implementation
    // would involve the parser creating a specific CJMOD node, and the generator
    // calling the manager. For now, we'll do a simple text-based replacement
    // to prove the dynamic loading and code generation mechanism works.

    CHTLJSLexer lexer(chtl_js_source);
    auto tokens = lexer.tokenize();
    std::stringstream final_js;

    for (const auto& token : tokens) {
        if (token.type == CHTLJSTokenType::Identifier && cjmodManager.has_transformer(token.value)) {
            // Found a CJMOD function call. Execute the transformer.
            Arg temp_args; // In a real system, the parser would create this.
            final_js << cjmodManager.execute_transformer(token.value, temp_args);
        }
    }

    // For this PoC, we assume the CJMOD is the only thing in the script.
    // A real implementation would need to handle regular CHTL JS alongside it.
    if (!final_js.str().empty()) {
        return final_js.str();
    }

    // Fallback to the original AST-based compilation for non-CJMOD code.
    CHTLJSParser parser;
    std::unique_ptr<CHTLJSBaseNode> ast = parser.parse(chtl_js_source);
    if (!ast) {
        return "";
    }
    return generator.generate(*ast);
}