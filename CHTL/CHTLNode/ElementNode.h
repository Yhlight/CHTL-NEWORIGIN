#pragma once

#include "Node.h"
#include "AttributeNode.h"
#include "TextNode.h"
#include "StylePropertyNode.h"

class ElementNode : public Node {
public:
    ElementNode(const std::string& tagName) : tagName(tagName) {}

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    std::string getTagName() const { return tagName; }
    const AttributeList& getAttributes() const { return attributes; }
    const StyleStatementList& getStyleStatements() const { return styleStatements; }
    const NodeList& getChildren() const { return children; }

    void addAttribute(std::unique_ptr<AttributeNode> attr) {
        attributes.push_back(std::move(attr));
    }

    void addStyleStatement(std::unique_ptr<StyleStatementNode> style) {
        styleStatements.push_back(std::move(style));
    }

    void addChild(NodePtr child) {
        children.push_back(std::move(child));
    }

private:
    std::string tagName;
    AttributeList attributes;
    StyleStatementList styleStatements;
    NodeList children;
};
