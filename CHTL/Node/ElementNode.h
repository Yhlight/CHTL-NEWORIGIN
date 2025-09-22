#pragma once

#include "Node.h"
#include "Attribute.h"
#include "StyleProperty.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include "CHTL/AstVisitor.h"

class ElementNode : public Node {
public:
    ElementNode(std::string tag_name, AttributeList attributes, StylePropertyList styles, NodeList children)
        : tag_name(std::move(tag_name)),
          attributes(std::move(attributes)),
          styles(std::move(styles)),
          children(std::move(children)) {}

    void accept(AstVisitor& visitor) const override {
        visitor.visit(*this);
    }

    std::string to_string(int indent = 0) const override {
        std::stringstream ss;
        std::string indentation(indent * 2, ' ');
        ss << indentation << "<" << tag_name << ">" << std::endl;
        for (const auto& attr : attributes) {
            ss << indentation << "  " << attr.key << "=\"" << attr.value << "\"" << std::endl;
        }
        for (const auto& style : styles) {
            // Cannot easily print expression AST here, so we skip it for debugging.
            // ss << indentation << "  " << "style: " << style.key << ": " << style.value << std::endl;
            ss << indentation << "  " << "style: " << style.key << ": [Expression]" << std::endl;
        }
        for (const auto& child : children) {
            ss << child->to_string(indent + 1);
        }
        return ss.str();
    }

    std::string tag_name;
    AttributeList attributes;
    StylePropertyList styles;
    NodeList children;
};
