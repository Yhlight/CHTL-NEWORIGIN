#pragma once

#include "BaseNode.h"
#include "AstVisitor.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL
{
    // Represents an element node in the CHTL document, e.g., div { ... }
    class ElementNode : public BaseNode
    {
    public:
        std::string tagName;
        std::map<std::string, std::string> attributes;
        std::vector<std::unique_ptr<BaseNode>> children;

        explicit ElementNode(const std::string& name) : tagName(name) {}

        void addChild(std::unique_ptr<BaseNode> child) {
            children.push_back(std::move(child));
        }

        void addAttribute(const std::string& key, const std::string& value) {
            attributes[key] = value;
        }

        void accept(AstVisitor &visitor) override
        {
            visitor.visit(*this);
        }
    };
}
