#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <string>
#include <sstream>
#include <memory>
#include <unordered_set>

class CHTLContext; // Forward declaration

class CHTLGenerator {
public:
    explicit CHTLGenerator(std::shared_ptr<ElementNode> root, CHTLContext& context);
    std::string generate();

private:
    CHTLContext& context;
    void visit(const std::shared_ptr<BaseNode>& node);
    void visitElement(const std::shared_ptr<ElementNode>& node);
    void visitText(const std::shared_ptr<TextNode>& node);
    void visitComment(const std::shared_ptr<CommentNode>& node);

    std::shared_ptr<ElementNode> rootNode;
    std::stringstream output;
    int indentLevel = 0;

    const std::unordered_set<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };

    void indent();
};
