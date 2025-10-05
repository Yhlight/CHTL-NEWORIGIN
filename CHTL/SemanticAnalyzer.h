#pragma once

#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ConstraintNode.h"
#include "CHTLContext/GenerationContext.h"
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

class SemanticAnalyzer {
public:
    void analyze(const std::shared_ptr<BaseNode>& root, const GenerationContext& context);

private:
    const GenerationContext* context = nullptr;
    void visit(const std::shared_ptr<BaseNode>& node, const std::vector<Constraint>& active_constraints, const std::shared_ptr<ElementNode>& parent);
    void visitElement(const std::shared_ptr<ElementNode>& node, const std::vector<Constraint>& active_constraints);
    void visitStyleNode(const std::shared_ptr<StyleNode>& node, const std::shared_ptr<ElementNode>& parent);
    bool evaluateCondition(const std::string& condition, const std::shared_ptr<ElementNode>& scope);
    void checkNodeAgainstConstraints(const std::shared_ptr<BaseNode>& node, const std::vector<Constraint>& constraints);
    std::shared_ptr<NumericLiteralNode> evaluateExpression(const std::shared_ptr<BaseNode>& node);
};

}