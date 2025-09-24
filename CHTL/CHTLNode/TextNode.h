#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "Node.h"
#include <string>

namespace CHTL {

class TextNode : public Node {
public:
    explicit TextNode(const std::string& text);

    void Accept(AstVisitor& visitor) override;

    std::string text;
};

} // namespace CHTL

#endif // CHTL_TEXT_NODE_H
