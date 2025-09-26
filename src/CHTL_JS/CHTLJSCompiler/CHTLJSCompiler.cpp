#include "CHTLJSCompiler.h"
#include "../CHTLLexer/CHTLJSLexer.h"
#include "../CHTLJSParser/CHTLJSParser.h"
#include "../CHTLJSGenerator/CHTLJSGenerator.h"

std::string CHTLJSCompiler::compile(const std::string& chtl_js_source) {
    // The full CHTL JS compilation pipeline.
    auto lexer = createLexer(chtl_js_source);
    auto tokens = lexer.tokenize();
    auto parser = createParser(std::move(tokens));
    auto ast = parser.parse();
    return generator.generate(ast);
}

CHTLJSLexer CHTLJSCompiler::createLexer(const std::string& source) {
    return CHTLJSLexer(source);
}

CHTLJSParser CHTLJSCompiler::createParser(std::vector<CHTLJSToken> tokens) {
    return CHTLJSParser(tokens);
}