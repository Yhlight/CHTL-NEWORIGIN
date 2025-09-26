#include "CHTLJSCompiler.h"

std::string CHTLJSCompiler::compile(const std::string& chtl_js_source) {
    CHTLJSLexer lexer(chtl_js_source);
    auto tokens = lexer.tokenize();

    CHTLJSParser parser(tokens);
    auto ast = parser.parse();

    return generator.generate(ast);
}

CHTLJSLexer CHTLJSCompiler::createLexer(const std::string& source) {
    return CHTLJSLexer(source);
}

CHTLJSParser CHTLJSCompiler::createParser(std::vector<CHTLJSToken> tokens) {
    return CHTLJSParser(tokens);
}