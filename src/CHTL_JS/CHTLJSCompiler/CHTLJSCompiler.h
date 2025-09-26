#pragma once

#include "CHTL_JS/CHTLLexer/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTL_JS/CHTLJSManage/VirtualObjectManager.h"
#include <string>

class CHTLJSCompiler {
public:
    std::string compile(const std::string& chtl_js_source);

private:
    CHTLJSLexer createLexer(const std::string& source);
    CHTLJSParser createParser(std::vector<CHTLJSToken> tokens);
    CHTLJSGenerator generator;
    VirtualObjectManager virtualObjectManager;
};