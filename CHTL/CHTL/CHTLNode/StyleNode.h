#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include <string>
#include <vector>

// This node holds the raw string content of a local style { ... } block.
// It is not part of the standard AST child hierarchy, but rather a property
// of an ElementNode. The generator will process this node when it visits the ElementNode.
class StyleNode {
public:
    StyleNode(const std::string& content) : rawContent(content) {}
    const std::string& getContent() const { return rawContent; }
private:
    std::string rawContent;
};

#endif // CHTL_STYLE_NODE_H
