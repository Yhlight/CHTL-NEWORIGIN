#pragma once

#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ConstraintNode.h"
#include <memory>
#include <vector>

namespace CHTL {

// Forward declarations for node types
class ElementNode;
class TemplateUsageNode;

class SemanticAnalyzer {
public:
    void analyze(const std::shared_ptr<BaseNode>& root);

private:
    void visit(const std::shared_ptr<BaseNode>& node, const std::vector<Constraint>& active_constraints);
    void visitElement(const std::shared_ptr<ElementNode>& node, const std::vector<Constraint>& active_constraints);
};

}