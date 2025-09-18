#pragma once

#include "../CHTLNode/ASTVisitor.h"
#include "../CHTLNode/BaseNode.h"
#include <string>

namespace CHTL {

class CHTLGenerator : public ASTVisitor {
public:
    // The main entry point for the generator.
    std::string generate(NodePtr& ast);

    // Visitor methods for each node type.
    std::string visit(ElementNode& node) override;
    std::string visit(TextNode& node) override;
    std::string visit(CommentNode& node) override;
    std::string visit(AttributeNode& node) override;
    std::string visit(StyleNode& node) override;
};

} // namespace CHTL
