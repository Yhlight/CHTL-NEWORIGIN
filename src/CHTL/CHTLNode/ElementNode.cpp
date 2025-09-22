#include "ElementNode.h"
#include <iostream>

namespace CHTL {

    ElementNode::ElementNode(const std::string& tagName) : tagName(tagName) {}

    const std::string& ElementNode::getTagName() const {
        return tagName;
    }

    void ElementNode::addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void ElementNode::print(int indent) const {
        std::string indentStr;
        for (int i = 0; i < indent; ++i) {
            indentStr += "  ";
        }
        std::cout << indentStr << "ElementNode: <" << tagName << ">" << std::endl;
        for (const auto& child : children) {
            if (child) {
                child->print(indent + 1);
            }
        }
    }

} // namespace CHTL
