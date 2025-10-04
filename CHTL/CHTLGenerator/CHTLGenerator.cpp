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
#include "../CHTLNode/DeleteNode.h"
#include "../CHTLNode/InsertNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../SharedCore/SaltBridge.h"
#include <stdexcept>
#include <set>
#include <map>

namespace CHTL {

void CHTLGenerator::generate(const std::shared_ptr<BaseNode>& node, const GenerationContext& context, SaltBridge* bridge) {
    this->context = &context;
    this->salt_bridge = bridge;
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
        case NodeType::NODE_SCRIPT:
            visit(std::dynamic_pointer_cast<ScriptNode>(node));
            break;
        // ... other cases from previous implementation
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
        case NodeType::NODE_DELETE:
            visit(std::dynamic_pointer_cast<DeleteNode>(node));
            break;
        case NodeType::NODE_INSERT:
            visit(std::dynamic_pointer_cast<InsertNode>(node));
            break;
        default:
            break;
    }
}

void CHTLGenerator::visit(const std::shared_ptr<ElementNode>& node) {
    if (node->getTagName() == "text") {
        for (const auto& child : node->getChildren()) {
            visit(child);
        }
        return;
    }

    if (node->getTagName() != "root" && node->getTagName() != "specialization-root") {
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

    if (node->getTagName() != "root" && node->getTagName() != "specialization-root") {
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
    if (!templateNode) return;

    auto specializationRoot = node->getSpecialization();
    if (!specializationRoot) {
        for (const auto& child : templateNode->getChildren()) {
            visit(child);
        }
        return;
    }

    std::set<std::string> deleted_selectors;
    std::vector<std::shared_ptr<InsertNode>> insert_rules;
    std::map<std::string, std::shared_ptr<ElementNode>> modification_rules;

    for (const auto& rule_node : specializationRoot->getChildren()) {
        if (auto delete_node = std::dynamic_pointer_cast<DeleteNode>(rule_node)) {
            for (const auto& target : delete_node->targets) {
                deleted_selectors.insert(target);
            }
        } else if (auto insert_node = std::dynamic_pointer_cast<InsertNode>(rule_node)) {
            insert_rules.push_back(insert_node);
        } else if (auto element_node = std::dynamic_pointer_cast<ElementNode>(rule_node)) {
            modification_rules[element_node->getTagName()] = element_node;
        }
    }

    auto find_inserts = [&](const std::string& selector, InsertPosition pos) {
        for(const auto& rule : insert_rules) {
            if(rule->position == pos && (rule->target_selector == selector || (pos == InsertPosition::AT_TOP || pos == InsertPosition::AT_BOTTOM))) {
                 for (const auto& content : rule->getChildren()) {
                    visit(content);
                }
            }
        }
    };

    find_inserts("", InsertPosition::AT_TOP);

    std::map<std::string, int> tag_counters;
    for (const auto& original_child : templateNode->getChildren()) {

        auto original_element = std::dynamic_pointer_cast<ElementNode>(original_child);
        if (!original_element) {
            visit(original_child);
            continue;
        }

        std::string tag_name = original_element->getTagName();
        int index = tag_counters[tag_name]++;
        std::string simple_selector = tag_name;
        std::string indexed_selector = tag_name + "[" + std::to_string(index) + "]";

        if (deleted_selectors.count(simple_selector) > 0 || deleted_selectors.count(indexed_selector) > 0) {
            continue;
        }

        find_inserts(simple_selector, InsertPosition::BEFORE);
        find_inserts(indexed_selector, InsertPosition::BEFORE);

        auto it = modification_rules.find(tag_name);
        if (it != modification_rules.end()) {
            auto modification_element = it->second;
            auto merged_element = std::make_shared<ElementNode>(original_element->getTagName());

            for(const auto& attr : original_element->getAttributes()) merged_element->setAttribute(attr.first, attr.second);
            for(const auto& attr : modification_element->getAttributes()) merged_element->setAttribute(attr.first, attr.second);

            std::shared_ptr<StyleNode> original_style = nullptr;
            for(const auto& child : original_element->getChildren()) if(child->getType() == NodeType::NODE_STYLE) original_style = std::dynamic_pointer_cast<StyleNode>(child);
            std::shared_ptr<StyleNode> modification_style = nullptr;
            for(const auto& child : modification_element->getChildren()) if(child->getType() == NodeType::NODE_STYLE) modification_style = std::dynamic_pointer_cast<StyleNode>(child);

            auto merged_style = std::make_shared<StyleNode>();
            if(original_style) for(const auto& prop : original_style->getChildren()) merged_style->addChild(prop);
            if(modification_style) for(const auto& prop : modification_style->getChildren()) merged_style->addChild(prop);

            if(!merged_style->getChildren().empty()) merged_element->addChild(merged_style);

            for(const auto& child : original_element->getChildren()) if(child->getType() != NodeType::NODE_STYLE) merged_element->addChild(child);

            visit(merged_element);
        } else {
            visit(original_child);
        }

        find_inserts(simple_selector, InsertPosition::AFTER);
        find_inserts(indexed_selector, InsertPosition::AFTER);
    }

    find_inserts("", InsertPosition::AT_BOTTOM);
}

void CHTLGenerator::visit(const std::shared_ptr<ScriptNode>& node) {
    if (salt_bridge) {
        std::string processed_script = salt_bridge->processScript(node->getContent());
        html_out << "<script>" << processed_script << "</script>";
    } else {
        // Fallback if no bridge is provided
        html_out << "<script>" << node->getContent() << "</script>";
    }
}

void CHTLGenerator::visit(const std::shared_ptr<CustomNode>& node) {}
void CHTLGenerator::visit(const std::shared_ptr<OriginNode>& node) {
    if (node->getOriginType() == "@Html") html_out << node->getContent();
    else if (node->getOriginType() == "@Style") css_out << node->getContent();
    else if (node->getOriginType() == "@JavaScript") html_out << node->getContent();
}
void CHTLGenerator::visit(const std::shared_ptr<ImportNode>& node) {}
void CHTLGenerator::visit(const std::shared_ptr<NamespaceNode>& node) {
    for (const auto& child : node->getChildren()) visit(child);
}
void CHTLGenerator::visit(const std::shared_ptr<ConfigurationNode>& node) {}
void CHTLGenerator::visit(const std::shared_ptr<PropertyNode>& node) {}
void CHTLGenerator::visit(const std::shared_ptr<RuleNode>& node) {
    css_out << node->getSelector() << "{";
    for (const auto& prop : node->getProperties()) {
        if (prop) css_out << prop->getKey() << ":" << prop->getValue() << ";";
    }
    css_out << "}";
}
void CHTLGenerator::visit(const std::shared_ptr<UseNode>& node) {
    if (node->getUseType() == "html5") html_out << "<!DOCTYPE html>";
}
void CHTLGenerator::visit(const std::shared_ptr<IfNode>& node) {
    if (element_stack.empty()) return;
    auto parentElement = element_stack.back();
    std::string selector;
    if (parentElement->hasAttribute("id")) {
        selector = "#" + parentElement->getAttributes().at("id");
    } else if (parentElement->hasAttribute("class")) {
        std::string classes = parentElement->getAttributes().at("class");
        selector = "." + classes.substr(0, classes.find(' '));
    } else {
        selector = parentElement->getTagName();
    }
    std::string media_query = "@media screen and (" + node->condition + ")";
    css_out << media_query << " {" << selector << " {";
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::NODE_PROPERTY) {
            auto prop = std::dynamic_pointer_cast<PropertyNode>(child);
            if (prop) css_out << prop->getKey() << ":" << prop->getValue() << ";";
        }
    }
    css_out << "}}";
}
void CHTLGenerator::visit(const std::shared_ptr<DeleteNode>& node) {}
void CHTLGenerator::visit(const std::shared_ptr<InsertNode>& node) {
    for (const auto& child : node->getChildren()) visit(child);
}

}