#pragma once

#include "../CHTLContext/CHTLContext.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include <memory>
#include <vector>

class CHTLResolver {
public:
    CHTLResolver(CHTLContext& context);
    std::shared_ptr<ElementNode> resolve(const std::shared_ptr<ElementNode>& root, const std::string& currentFilePath);

private:
    CHTLContext& context;
    void processImports(const std::shared_ptr<ElementNode>& root, const std::string& currentFilePath);
    void resolveInheritance();
    void validateNodeConstraints(const std::shared_ptr<BaseNode>& node);

    std::vector<std::shared_ptr<BaseNode>> resolveNode(const std::shared_ptr<BaseNode>& node);
    std::shared_ptr<ElementNode> resolveElement(const std::shared_ptr<ElementNode>& node);
    std::vector<std::shared_ptr<BaseNode>> resolveChildren(const std::vector<std::shared_ptr<BaseNode>>& children);
};
