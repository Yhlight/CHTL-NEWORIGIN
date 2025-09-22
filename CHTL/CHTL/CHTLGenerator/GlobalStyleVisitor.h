#pragma once

#include "AstVisitor.h"
#include "../CHTLNode/CssRuleNode.h"
#include <vector>

namespace CHTL {

class GlobalStyleVisitor : public AstVisitor {
public:
    void collect(BaseNode* root);
    const std::vector<CssRuleNode*>& getCollectedRules() const;

    // Visitor overrides
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(AttributeNode& node) override;
    void visit(StyleBlockNode& node) override;
    void visit(CssPropertyNode& node) override;
    void visit(CssRuleNode& node) override;

private:
    std::vector<CssRuleNode*> collectedRules;
};

} // namespace CHTL
