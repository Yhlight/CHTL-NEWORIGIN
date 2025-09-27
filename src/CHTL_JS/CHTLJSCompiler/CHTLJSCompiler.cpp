#include "CHTLJSCompiler.h"
#include "../CHTLLexer/CHTLJSLexer.h"
#include "../CHTLJSParser/CHTLJSParser.h"
#include "../CHTLJSGenerator/CHTLJSGenerator.h"
#include "../CHTLJSNode/VirtualObjectNode.h"

std::vector<std::unique_ptr<CHTLJSNode>> CHTLJSCompiler::parse(const std::string& chtl_js_source) {
    auto lexer = createLexer(chtl_js_source);
    auto tokens = lexer.tokenize();
    auto parser = createParser(std::move(tokens));
    return parser.parse();
}

std::string CHTLJSCompiler::generate(std::vector<std::unique_ptr<CHTLJSNode>>& ast) {
    // Clear the manager to prevent state from leaking between test runs.
    virtualObjectManager.clear();

    // A new vector to hold the nodes that will be passed to the generator.
    std::vector<std::unique_ptr<CHTLJSNode>> generator_ast;

    // First pass: find and register all virtual objects.
    for (auto& node : ast) {
        if (node && node->getType() == CHTLJSNodeType::VirtualObject) {
            // Safely downcast the unique_ptr from Base to Derived.
            auto virNode = std::unique_ptr<VirtualObjectNode>(static_cast<VirtualObjectNode*>(node.release()));
            virtualObjectManager.registerVirtualObject(virNode->name, std::move(virNode));
        }
    }

    // Second pass: move the remaining (non-null) nodes to the generator's AST.
    for (auto& node : ast) {
        if (node) {
            generator_ast.push_back(std::move(node));
        }
    }

    // The generator now receives an AST without the VirtualObject definitions,
    // but the manager is correctly populated.
    return generator.generate(generator_ast, virtualObjectManager);
}

CHTLJSLexer CHTLJSCompiler::createLexer(const std::string& source) {
    return CHTLJSLexer(source);
}

CHTLJSParser CHTLJSCompiler::createParser(std::vector<CHTLJSToken> tokens) {
    return CHTLJSParser(tokens);
}