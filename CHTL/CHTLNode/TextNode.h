#ifndef TEXT_NODE_H
#define TEXT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    explicit TextNode(const std::string& text) : BaseNode(NodeType::TEXT), text(text) {}
    void print(int indent = 0) const override;
    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<TextNode>(text);
    }
    const std::string& getValue() const;

private:
    std::string text;
};

} // namespace CHTL

#endif // TEXT_NODE_H