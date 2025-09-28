#pragma once

#include "CHTL_JS/CHTLLexer/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTL_JS/CHTLJSManage/VirtualObjectManager.h"
#include "CHTL_JS/CHTLJSNode/CHTLJSNode.h"
#include "CHTL_JS/Util/Placeholder.h" // Correct path to the new Placeholder class
#include <string>
#include <vector>
#include <memory>

class CHTLJSCompiler {
public:
    // Parse now handles the granular scan and populates the placeholder manager
    std::vector<std::unique_ptr<CHTLJSNode>> parse(const std::string& chtl_js_source);
    // Generate will now use the internal placeholder manager to resolve placeholders
    std::string generate(std::vector<std::unique_ptr<CHTLJSNode>>& ast);

private:
    // This new method will perform the granular scan of script content.
    std::string granular_scan(const std::string& content);

    CHTLJSLexer createLexer(const std::string& source);

    CHTLJSGenerator generator;
    VirtualObjectManager virtualObjectManager;
    Placeholder placeholder_manager; // The placeholder manager is now a member
};