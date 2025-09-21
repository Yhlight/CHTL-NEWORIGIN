#pragma once

#include "../CHTLNode/Nodes.h"
#include <string>
#include <sstream>
#include <memory>

namespace CHTL {

class Generator {
public:
    Generator(const std::vector<NodePtr>& nodes);
    std::string generate();

private:
    void visit(const NodePtr& node);
    void visitElementNode(const std::shared_ptr<ElementNode>& node);
    void visitTextNode(const std::shared_ptr<TextNode>& node);

    void collectCssRules(const NodePtr& node);
    void generateCss();

    const std::vector<NodePtr>& rootNodes;
    std::stringstream output;
    std::vector<std::shared_ptr<CssRuleNode>> globalCssRules;
    int indentLevel = 0;

    void indent();
};

} // namespace CHTL
