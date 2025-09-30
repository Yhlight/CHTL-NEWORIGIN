#ifndef TEXT_NODE_H
#define TEXT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    TextNode(const std::string& text);
    void print(int indent = 0) const override;
    const std::string& getValue() const;

private:
    std::string text;
};

} // namespace CHTL

#endif // TEXT_NODE_H