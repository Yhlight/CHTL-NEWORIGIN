#pragma once

#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ConstraintNode.h"
#include <memory>
#include <vector>

namespace CHTL {

// Forward declarations for node types
class ElementNode;
class StyleNode;
class TemplateUsageNode;
class IfNode;

class SemanticAnalyzer {
public:
    void analyze(const std::shared_ptr<BaseNode>& root);

private:
    void visit(const std::shared_ptr<BaseNode>& node, const std::vector<Constraint>& active_constraints, const std::shared_ptr<ElementNode>& parent, const std::shared_ptr<BaseNode>& root);
    void visitElement(const std::shared_ptr<ElementNode>& node, const std::vector<Constraint>& active_constraints, const std::shared_ptr<BaseNode>& root);
    void visitStyleNode(const std::shared_ptr<StyleNode>& node, const std::shared_ptr<ElementNode>& parent, const std::shared_ptr<BaseNode>& root);
    bool evaluateCondition(const std::string& condition, const std::shared_ptr<ElementNode>& scope, const std::shared_ptr<BaseNode>& root);
    void checkNodeAgainstConstraints(const std::shared_ptr<BaseNode>& node, const std::vector<Constraint>& constraints);
};

}