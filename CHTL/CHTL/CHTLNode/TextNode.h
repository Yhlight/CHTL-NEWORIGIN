#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"
#include "../CHTLGenerator/Generator.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    std::string value;

    std::string toString() const override {
        return "TextNode { Value: \"" + value + "\" }";
    }

    void accept(Generator& visitor) const override {
        visitor.visit(*this);
    }
};

} // namespace CHTL

#endif // CHTL_TEXT_NODE_H
