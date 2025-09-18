#pragma once

#include "../CHTLNode/AstVisitor.h"
#include "../CHTLNode/BaseNode.h"
#include <vector>
#include <string>
#include <memory>
#include <sstream>

namespace CHTL
{

    // HtmlGenerator traverses the AST and generates an HTML string.
    // It implements the AstVisitor interface.
    class HtmlGenerator : public AstVisitor
    {
    public:
        // Generates HTML from a vector of root AST nodes.
        std::string generate(const std::vector<std::unique_ptr<BaseNode>> &nodes);

        // Visitor methods for each node type.
        void visit(ElementNode &node) override;
        void visit(TextNode &node) override;
        void visit(CommentNode &node) override;
        void visit(StyleNode &node) override;

        // Expression nodes are not visited directly by the generator,
        // but the methods must be implemented to make the class concrete.
        void visit(LiteralExprNode &node) override;
        void visit(BinaryOpExprNode &node) override;
        void visit(PropertyAccessExprNode &node) override;
        void visit(GroupExprNode &node) override;

    private:
        std::stringstream output;
    };

}
