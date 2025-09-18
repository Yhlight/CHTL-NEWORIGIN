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

    private:
        std::stringstream output;
    };

}
