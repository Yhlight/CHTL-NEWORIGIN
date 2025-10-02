#pragma once

#include "BaseNode.h"
#include <string>
#include <map>

namespace CHTL {

    class ElementNode : public BaseNode {
    public:
        ElementNode(const std::string& tagName) : tagName(tagName) {
            type = NodeType::NODE_ELEMENT;
        }

        const std::string& getTagName() const { return tagName; }
        const std::map<std::string, std::string>& getAttributes() const { return attributes; }
        void setAttribute(const std::string& key, const std::string& value) { attributes[key] = value; }

    private:
        std::string tagName;
        std::map<std::string, std::string> attributes;
    };

}