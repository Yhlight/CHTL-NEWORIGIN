#pragma once

#include "../CHTLJSNode/CHTLJSNode.h"
#include "../CHTLJSNode/EventBindingNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/ScriptLoaderNode.h"
#include "../CHTLJSManage/VirtualObjectManager.h"
#include <string>
#include <vector>
#include <memory>

class CHTLJSGenerator {
public:
    std::string generate(const std::vector<std::unique_ptr<CHTLJSNode>>& ast, const VirtualObjectManager& vom);

private:
    bool scriptLoaderInjected = false;
    const VirtualObjectManager* vom = nullptr;
    std::string generateNode(const CHTLJSNode* node);
    std::string generateScriptLoader(const ScriptLoaderNode* node);
};