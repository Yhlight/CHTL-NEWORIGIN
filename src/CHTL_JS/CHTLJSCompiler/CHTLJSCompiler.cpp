#include "CHTLJSCompiler.h"
#include "../CHTLLexer/CHTLJSLexer.h"
#include "../CHTLJSParser/CHTLJSParser.h"
#include "../CHTLJSGenerator/CHTLJSGenerator.h"
#include "../CHTLJSNode/VirtualObjectNode.h"

std::string CHTLJSCompiler::compile(const std::string& chtl_js_source) {
    // The full CHTL JS compilation pipeline.
    auto lexer = createLexer(chtl_js_source);
    auto tokens = lexer.tokenize();
    auto parser = createParser(std::move(tokens));
    auto ast = parser.parse();

    // Register any virtual objects that were parsed.
    for (const auto& node : ast) {
        if (node->getType() == CHTLJSNodeType::VirtualObject) {
            auto* virNode = static_cast<VirtualObjectNode*>(node.get());
            // The manager takes ownership, so we need to release and re-wrap.
            // This is a simplification; a real implementation might need to clone.
            // For now, we assume the generator doesn't also need ownership.
            // This part of the logic will be expanded upon in the next step.
        }
    }

    return generator.generate(ast);
}

CHTLJSLexer CHTLJSCompiler::createLexer(const std::string& source) {
    return CHTLJSLexer(source);
}

CHTLJSParser CHTLJSCompiler::createParser(std::vector<CHTLJSToken> tokens) {
    return CHTLJSParser(tokens);
}