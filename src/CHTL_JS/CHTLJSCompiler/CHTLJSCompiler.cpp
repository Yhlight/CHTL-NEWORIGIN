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
    // Separate Vir declarations from code-generation nodes.
    std::vector<std::unique_ptr<CHTLJSNode>> generator_ast;
    for (auto& node : ast) {
        if (node && node->getType() == CHTLJSNodeType::VirtualObject) {
            auto virNode = std::unique_ptr<VirtualObjectNode>(static_cast<VirtualObjectNode*>(node.release()));
            virtualObjectManager.registerVirtualObject(virNode->name, std::move(virNode));
        } else if (node) {
            generator_ast.push_back(std::move(node));
        }
    }

    // The generator now receives the AST and the virtual object manager.
    return generator.generate(generator_ast, virtualObjectManager);
}

CHTLJSLexer CHTLJSCompiler::createLexer(const std::string& source) {
    return CHTLJSLexer(source);
}

CHTLJSParser CHTLJSCompiler::createParser(std::vector<CHTLJSToken> tokens) {
    return CHTLJSParser(tokens);
}