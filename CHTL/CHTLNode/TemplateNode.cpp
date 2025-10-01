#include "TemplateNode.h"
#include <iostream>

namespace CHTL {

TemplateNode::TemplateNode(const std::string& type, const std::string& name)
    : templateType(type), templateName(name) {}

void TemplateNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "TemplateDefinition: @" << templateType << " " << templateName << " {\n";

    // Print properties for Style Templates
    for (const auto& prop : properties) {
        prop->print(indent + 1);
    }

    // Print children for Element Templates
    for (const auto& child : children) {
        child->print(indent + 1);
    }

    // Print inherited templates
    for (const auto& usage : templateUsages) {
        usage->print(indent + 1);
    }

    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "}\n";
}

std::string TemplateNode::getTemplateType() const {
    return templateType;
}

std::string TemplateNode::getTemplateName() const {
    return templateName;
}

const std::vector<std::unique_ptr<StylePropertyNode>>& TemplateNode::getProperties() const {
    return properties;
}

void TemplateNode::addProperty(std::unique_ptr<StylePropertyNode> property) {
    properties.push_back(std::move(property));
}

const std::vector<std::unique_ptr<BaseNode>>& TemplateNode::getChildren() const {
    return children;
}

void TemplateNode::addChild(std::unique_ptr<BaseNode> child) {
    children.push_back(std::move(child));
}

const std::vector<std::unique_ptr<TemplateUsageNode>>& TemplateNode::getTemplateUsages() const {
    return templateUsages;
}

void TemplateNode::addTemplateUsage(std::unique_ptr<TemplateUsageNode> usage) {
    templateUsages.push_back(std::move(usage));
}

} // namespace CHTL