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

    // Separate Vir declarations from code-generation nodes.
    std::vector<std::unique_ptr<CHTLJSNode>> generator_ast;
    for (auto& node : ast) {
        if (node && node->getType() == CHTLJSNodeType::VirtualObject) {
            // Cast and move the node to the manager.
            // This transfers ownership of the node from the AST to the manager.
            auto virNode = std::unique_ptr<VirtualObjectNode>(static_cast<VirtualObjectNode*>(node.release()));
            virtualObjectManager.registerVirtualObject(virNode->name, std::move(virNode));
        } else if (node) {
            // Move all other nodes to the generator's AST.
            generator_ast.push_back(std::move(node));
        }
    }

    // The generator now only receives nodes that should result in output.
    return generator.generate(generator_ast);
}

CHTLJSLexer CHTLJSCompiler::createLexer(const std::string& source) {
    return CHTLJSLexer(source);
}

CHTLJSParser CHTLJSCompiler::createParser(std::vector<CHTLJSToken> tokens) {
    return CHTLJSParser(tokens);
}