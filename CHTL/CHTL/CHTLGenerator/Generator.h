#pragma once

#include "AstVisitor.h"
#include "../CHTLNode/BaseNode.h"
#include <string>
#include <sstream>

namespace CHTL {

class HtmlGenerator : public AstVisitor {
public:
    std::string generate(BaseNode* root);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(AttributeNode& node) override;
    void visit(StyleBlockNode& node) override;
    void visit(CssPropertyNode& node) override;

private:
    std::stringstream output;
};

} // namespace CHTL
