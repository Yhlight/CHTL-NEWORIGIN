#include "ElementNode.h"
#include <iostream>

namespace CHTL {

void ElementNode::addChild(std::unique_ptr<BaseNode> child) {
    children.push_back(std::move(child));
}

void ElementNode::addAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

void ElementNode::setStyle(std::unique_ptr<StyleNode> styleNode) {
    style = std::move(styleNode);
}

void ElementNode::setScript(std::unique_ptr<ScriptNode> scriptNode) {
    script = std::move(scriptNode);
}

void ElementNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "<" << tagName;
    for (const auto& attr : attributes) {
        std::cout << " " << attr.first << "=\"" << attr.second << "\"";
    }
    std::cout << ">" << std::endl;

    if (style) {
        style->print(indent + 1);
    }
    if (script) {
        script->print(indent + 1);
    }

    for (const auto& child : children) {
        child->print(indent + 1);
    }

    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "</" << tagName << ">" << std::endl;
}

const std::string& ElementNode::getTagName() const {
    return tagName;
}

const std::vector<std::unique_ptr<BaseNode>>& ElementNode::getChildren() const {
    return children;
}

const std::map<std::string, std::string>& ElementNode::getAttributes() const {
    return attributes;
}

std::string ElementNode::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    if (it != attributes.end()) {
        return it->second;
    }
    return "";
}

const StyleNode* ElementNode::getStyle() const {
    return style.get();
}

const ScriptNode* ElementNode::getScript() const {
    return script.get();
}

std::unique_ptr<BaseNode> ElementNode::clone() const {
    auto clonedNode = std::make_unique<ElementNode>(tagName);
    for (const auto& attr : attributes) {
        clonedNode->addAttribute(attr.first, attr.second);
    }
    if (style) {
        clonedNode->setStyle(std::unique_ptr<StyleNode>(static_cast<StyleNode*>(style->clone().release())));
    }
    if (script) {
        clonedNode->setScript(std::unique_ptr<ScriptNode>(static_cast<ScriptNode*>(script->clone().release())));
    }
    for (const auto& child : children) {
        clonedNode->addChild(child->clone());
    }
    return clonedNode;
}

} // namespace CHTL