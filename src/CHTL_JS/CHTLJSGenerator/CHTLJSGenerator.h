#pragma once

#include "../CHTLJSNode/CHTLJSNode.h"
#include "../CHTLJSNode/EventBindingNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/ScriptLoaderNode.h"
#include "../CHTLJSManage/VirtualObjectManager.h"
#include "../CHTLJSNode/RouterNode.h"
#include "../Util/Placeholder.h" // Correct path to Placeholder
#include <string>
#include <vector>
#include <memory>

class CHTLJSGenerator {
public:
    // The generate method now accepts a Placeholder manager to resolve placeholders.
    std::string generate(const std::vector<std::unique_ptr<CHTLJSNode>>& ast, const VirtualObjectManager& vom, const Placeholder& placeholder_manager);

private:
    bool scriptLoaderInjected = false;
    bool routerInjected = false;
    const VirtualObjectManager* vom = nullptr;
    const Placeholder* placeholder_manager = nullptr; // Store a pointer to the placeholder manager

    // This is the new private member function to resolve placeholders.
    std::string resolve_handler(const std::string& handler_code) const;

    std::string generateNode(const CHTLJSNode* node);
    std::string generateScriptLoader(const ScriptLoaderNode* node);
    std::string generateRouter(const RouterNode* node);
};