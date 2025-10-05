#pragma once

#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ConstraintNode.h"
#include "CHTLContext/GenerationContext.h"
#include "Common/Value.h"
#include <memory>
#include <vector>

namespace CHTL {

// Forward declarations for node types
class ElementNode;
class StyleNode;
class TemplateUsageNode;
class IfNode;
class PropertyNode;
class BinaryOpNode;
class NumericLiteralNode;
class ReferenceNode;

class SemanticAnalyzer {
public:
    void analyze(std::shared_ptr<BaseNode>& root, const GenerationContext& context);

private:
    const GenerationContext* context = nullptr;
    std::shared_ptr<BaseNode> root = nullptr;
    void visit(std::shared_ptr<BaseNode>& node, const std::vector<Constraint>& active_constraints, std::shared_ptr<ElementNode> parent);
    void visitElement(std::shared_ptr<ElementNode> node, const std::vector<Constraint>& active_constraints);
    void visitStyleNode(std::shared_ptr<StyleNode> node, std::shared_ptr<ElementNode> parent);
    bool evaluateCondition(const std::shared_ptr<BaseNode>& condition, const std::shared_ptr<ElementNode>& scope);
    void checkNodeAgainstConstraints(const std::shared_ptr<BaseNode>& node, const std::vector<Constraint>& constraints);
    Value evaluateExpression(const std::shared_ptr<BaseNode>& node, const std::shared_ptr<ElementNode>& scope);
};

}