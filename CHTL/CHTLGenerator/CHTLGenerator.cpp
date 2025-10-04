#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/ConfigurationNode.h"
#include "../CHTLNode/PropertyNode.h"
#include "../CHTLNode/RuleNode.h"
#include "../CHTLNode/UseNode.h"
#include "../CHTLNode/IfNode.h"
#include <stdexcept>

namespace CHTL {

void CHTLGenerator::generate(const std::shared_ptr<BaseNode>& node, const GenerationContext& context) {
    this->context = &context;
    if (node) {
        visit(node);
    }
}

void CHTLGenerator::visit(const std::shared_ptr<BaseNode>& node) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::NODE_ELEMENT:
            visit(std::dynamic_pointer_cast<ElementNode>(node));
            break;
        case NodeType::NODE_TEXT:
            visit(std::dynamic_pointer_cast<TextNode>(node));
            break;
        case NodeType::NODE_STYLE:
            visit(std::dynamic_pointer_cast<StyleNode>(node));
            break;
        case NodeType::NODE_TEMPLATE:
            visit(std::dynamic_pointer_cast<TemplateNode>(node));
            break;
        case NodeType::NODE_TEMPLATE_USAGE:
            visit(std::dynamic_pointer_cast<TemplateUsageNode>(node));
            break;
        case NodeType::NODE_CUSTOM:
            visit(std::dynamic_pointer_cast<CustomNode>(node));
            break;
        case NodeType::NODE_ORIGIN:
            visit(std::dynamic_pointer_cast<OriginNode>(node));
            break;
        case NodeType::NODE_IMPORT:
            visit(std::dynamic_pointer_cast<ImportNode>(node));
            break;
        case NodeType::NODE_NAMESPACE:
            visit(std::dynamic_pointer_cast<NamespaceNode>(node));
            break;
        case NodeType::NODE_CONFIG:
            visit(std::dynamic_pointer_cast<ConfigurationNode>(node));
            break;
        case NodeType::NODE_PROPERTY:
             visit(std::dynamic_pointer_cast<PropertyNode>(node));
            break;
        case NodeType::NODE_RULE:
             visit(std::dynamic_pointer_cast<RuleNode>(node));
            break;
        case NodeType::NODE_USE:
            visit(std::dynamic_pointer_cast<UseNode>(node));
            break;
        case NodeType::NODE_IF:
            visit(std::dynamic_pointer_cast<IfNode>(node));
            break;
        default:
            break;
    }
}

void CHTLGenerator::visit(const std::shared_ptr<ElementNode>& node) {
    // Handle the special 'text' tag by only visiting its children
    if (node->getTagName() == "text") {
        for (const auto& child : node->getChildren()) {
            visit(child);
        }
        return;
    }

    if (node->getTagName() != "root") {
        element_stack.push_back(node);
        html_out << "<" << node->getTagName();

        for (const auto& attr : node->getAttributes()) {
            html_out << " " << attr.first << "=\"" << attr.second << "\"";
        }

        std::stringstream style_ss;
        for (const auto& child : node->getChildren()) {
            if (child->getType() == NodeType::NODE_STYLE) {
                auto styleNode = std::dynamic_pointer_cast<StyleNode>(child);
                for (const auto& styleChild : styleNode->getChildren()) {
                    if (styleChild->getType() == NodeType::NODE_PROPERTY) {
                        auto prop = std::dynamic_pointer_cast<PropertyNode>(styleChild);
                        if (prop) {
                            if (!prop->getChildren().empty() && prop->getChildren()[0]->getType() == NodeType::NODE_TEMPLATE_USAGE) {
                                auto usageNode = std::dynamic_pointer_cast<TemplateUsageNode>(prop->getChildren()[0]);
                                if (usageNode && usageNode->getUsageType() == TemplateUsageType::VAR) {
                                    auto templateNode = context->getTemplate(usageNode->getName());
                                    if (templateNode) {
                                        for (const auto& varChild : templateNode->getChildren()) {
                                            if (varChild->getType() == NodeType::NODE_PROPERTY) {
                                                auto varProp = std::dynamic_pointer_cast<PropertyNode>(varChild);
                                                if (varProp && varProp->getKey() == usageNode->getVariableName()) {
                                                    style_ss << prop->getKey() << ":" << varProp->getValue() << ";";
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            } else {
                                style_ss << prop->getKey() << ":" << prop->getValue() << ";";
                            }
                        }
                    } else if (styleChild->getType() == NodeType::NODE_TEMPLATE_USAGE) {
                        auto usageNode = std::dynamic_pointer_cast<TemplateUsageNode>(styleChild);
                        if (usageNode && usageNode->getUsageType() == TemplateUsageType::STYLE) {
                             auto templateNode = context->getTemplate(usageNode->getName());
                             if(templateNode) {
                                 for(const auto& templateChild : templateNode->getChildren()) {
                                     if(templateChild->getType() == NodeType::NODE_PROPERTY) {
                                         auto prop = std::dynamic_pointer_cast<PropertyNode>(templateChild);
                                         if (prop) {
                                            style_ss << prop->getKey() << ":" << prop->getValue() << ";";
                                         }
                                     }
                                 }
                             }
                        }
                    } else if (styleChild->getType() == NodeType::NODE_RULE) {
                        visit(styleChild);
                    }
                }
            } else if (child->getType() == NodeType::NODE_IF) {
                visit(child);
            }
        }

        if (style_ss.rdbuf()->in_avail() > 0) {
            html_out << " style=\"" << style_ss.str() << "\"";
        }

        html_out << ">";
    }

    for (const auto& child : node->getChildren()) {
        if (child->getType() != NodeType::NODE_STYLE && child->getType() != NodeType::NODE_IF) {
            visit(child);
        }
    }

    if (node->getTagName() != "root") {
        html_out << "</" << node->getTagName() << ">";
        element_stack.pop_back();
    }
}

void CHTLGenerator::visit(const std::shared_ptr<TextNode>& node) {
    html_out << node->getContent();
}

void CHTLGenerator::visit(const std::shared_ptr<StyleNode>& node) {
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::NODE_RULE) {
            visit(child);
        }
    }
}

void CHTLGenerator::visit(const std::shared_ptr<TemplateNode>& node) {
}

void CHTLGenerator::visit(const std::shared_ptr<TemplateUsageNode>& node) {
    auto templateNode = context->getTemplate(node->getName());
    if (templateNode) {
        for (const auto& child : templateNode->getChildren()) {
            visit(child);
        }
    }
}

void CHTLGenerator::visit(const std::shared_ptr<CustomNode>& node) {
}

void CHTLGenerator::visit(const std::shared_ptr<OriginNode>& node) {
    if (node->getOriginType() == "@Html") {
        html_out << node->getContent();
    } else if (node->getOriginType() == "@Style") {
        css_out << node->getContent();
    } else if (node->getOriginType() == "@JavaScript") {
        html_out << node->getContent();
    }
}

void CHTLGenerator::visit(const std::shared_ptr<ImportNode>& node) {
}

void CHTLGenerator::visit(const std::shared_ptr<NamespaceNode>& node) {
    for (const auto& child : node->getChildren()) {
        visit(child);
    }
}

void CHTLGenerator::visit(const std::shared_ptr<ConfigurationNode>& node) {
}

void CHTLGenerator::visit(const std::shared_ptr<PropertyNode>& node) {
}

void CHTLGenerator::visit(const std::shared_ptr<RuleNode>& node) {
    css_out << node->getSelector() << "{";
    for (const auto& prop : node->getProperties()) {
        if (prop) {
             css_out << prop->getKey() << ":" << prop->getValue() << ";";
        }
    }
    css_out << "}";
}

void CHTLGenerator::visit(const std::shared_ptr<UseNode>& node) {
    if (node->getUseType() == "html5") {
        html_out << "<!DOCTYPE html>";
    }
}

void CHTLGenerator::visit(const std::shared_ptr<IfNode>& node) {
    if (element_stack.empty()) {
        return; // or throw an error, as `if` should be inside an element
    }
    auto parentElement = element_stack.back();
    std::string selector;

    if (parentElement->hasAttribute("id")) {
        selector = "#" + parentElement->getAttributes().at("id");
    } else if (parentElement->hasAttribute("class")) {
        // Using the first class if multiple are present
        std::string classes = parentElement->getAttributes().at("class");
        selector = "." + classes.substr(0, classes.find(' '));
    } else {
        selector = parentElement->getTagName();
    }

    // This is a simplified condition parser. A real implementation would need a more robust parser.
    std::string condition = node->condition;
    std::string media_query = "@media screen and (" + condition + ")";

    css_out << media_query << " {";
    css_out << selector << " {";

    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::NODE_PROPERTY) {
            auto prop = std::dynamic_pointer_cast<PropertyNode>(child);
            if (prop) {
                css_out << prop->getKey() << ":" << prop->getValue() << ";";
            }
        }
    }

    css_out << "}";
    css_out << "}";
}

}