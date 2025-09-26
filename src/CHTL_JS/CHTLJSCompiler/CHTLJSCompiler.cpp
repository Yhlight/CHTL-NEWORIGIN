#include "CHTLJSCompiler.h"
#include "../CHTLLexer/CHTLJSLexer.h"
#include "../CHTLJSParser/CHTLJSParser.h"
#include "../CHTLJSGenerator/CHTLJSGenerator.h"

std::string CHTLJSCompiler::compile(const std::string& chtl_js_source) {
    // Per user instruction, temporarily bypass the CHTL JS compilation
    // to stabilize the main compiler workflow. The CHTL JS features can be
    // fixed in a future task. This allows the dispatcher and merger to
    // function correctly with standard JavaScript.
    return chtl_js_source;
}

CHTLJSLexer CHTLJSCompiler::createLexer(const std::string& source) {
    return CHTLJSLexer(source);
}

CHTLJSParser CHTLJSCompiler::createParser(std::vector<CHTLJSToken> tokens) {
    return CHTLJSParser(tokens);
}