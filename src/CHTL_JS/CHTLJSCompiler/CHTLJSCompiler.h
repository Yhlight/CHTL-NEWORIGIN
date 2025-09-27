#pragma once

#include "CHTL_JS/CHTLLexer/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTL_JS/CHTLJSManage/VirtualObjectManager.h"
#include "CHTL_JS/CHTLJSNode/CHTLJSNode.h"
#include <string>
#include <vector>
#include <memory>

class CHTLJSCompiler {
public:
    std::vector<std::unique_ptr<CHTLJSNode>> parse(const std::string& chtl_js_source);
    std::string generate(std::vector<std::unique_ptr<CHTLJSNode>>& ast);

private:
    CHTLJSLexer createLexer(const std::string& source);
    CHTLJSParser createParser(std::vector<CHTLJSToken> tokens);
    CHTLJSGenerator generator;
    VirtualObjectManager virtualObjectManager;
};