#include "Generator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/AttributeNode.h"
#include <algorithm>

namespace CHTL {

std::string HtmlGenerator::generate(BaseNode* root) {
    // First pass: collect all CSS rules to be hoisted
    GlobalStyleVisitor style_collector;
    style_collector.collect(root);
    collectedRules = style_collector.getCollectedRules();

    // Second pass: generate HTML
    output.str("");
    output.clear();
    root->accept(*this);
    return output.str();
}

void HtmlGenerator::visit(ElementNode& node) {
    if (node.tagName == "root") {
        for (const auto& child : node.children) {
            child->accept(*this);
        }
        return;
    }

    output << "<" << node.tagName;

    // --- Automatic Attribute Injection ---
    bool has_id = false;
    bool has_class = false;
    for (const auto& attr : node.attributes) {
        if (attr->key == "id") has_id = true;
        if (attr->key == "class") has_class = true;
    }

    std::string auto_id;
    std::string auto_classes;
    for (const auto& child : node.children) {
        if (auto* style_block = dynamic_cast<StyleBlockNode*>(child.get())) {
            for (const auto& rule : style_block->rules) {
                if (!has_id && rule->selector.rfind('#', 0) == 0 && rule->selector.find(' ') == std::string::npos) {
                    auto_id = rule->selector.substr(1);
                    has_id = true; // Prevent adding more than one auto-id
                }
                if (rule->selector.rfind('.', 0) == 0 && rule->selector.find(' ') == std::string::npos) {
                    if (!auto_classes.empty()) auto_classes += " ";
                    auto_classes += rule->selector.substr(1);
                }
            }
        }
    }

    if (!auto_id.empty()) {
        output << " id=\"" << auto_id << "\"";
    }
    if (!has_class && !auto_classes.empty()) {
        output << " class=\"" << auto_classes << "\"";
    }
    // --- End Automatic Attribute Injection ---


    for (const auto& attr : node.attributes) {
        output << " ";
        attr->accept(*this);
    }

    // Aggregate inline styles from any StyleBlockNode children
    std::stringstream style_stream;
    ExpressionEvaluator evaluator;
    for (const auto& child : node.children) {
        if (auto* style_block = dynamic_cast<StyleBlockNode*>(child.get())) {
            for (const auto& prop : style_block->properties) {
                Value result = evaluator.evaluate(prop->value.get());
                style_stream << prop->property << ":";
                if (result.type == ValueType::NUMBER) {
                    style_stream << result.number_val << result.unit;
                } else {
                    style_stream << result.string_val;
                }
                style_stream << ";";
            }
        }
    }

    std::string style_string = style_stream.str();
    if (!style_string.empty()) {
        output << " style=\"" << style_string << "\"";
    }

    const std::vector<std::string> voidElements = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"};
    bool isVoid = std::find(voidElements.begin(), voidElements.end(), node.tagName) != voidElements.end();

    if (node.children.empty() && isVoid) {
        output << ">";
    } else {
        output << ">";
        // Render children
        for (const auto& child : node.children) {
            if (dynamic_cast<StyleBlockNode*>(child.get()) == nullptr) {
                child->accept(*this);
            }
        }

        // --- Global Style Injection ---
        if (node.tagName == "head") {
            if (!collectedRules.empty()) {
                output << "<style>";
                for (const auto* rule : collectedRules) {
                    output << rule->selector << "{";
                    for (const auto& prop : rule->properties) {
                         // We need to re-evaluate or store the result. Re-evaluating is easier for now.
                         Value result = evaluator.evaluate(prop->value.get());
                         output << prop->property << ":";
                         if (result.type == ValueType::NUMBER) {
                             output << result.number_val << result.unit;
                         } else {
                             output << result.string_val;
                         }
                         output << ";";
                    }
                    output << "}";
                }
                output << "</style>";
            }
        }
        // --- End Global Style Injection ---

        output << "</" << node.tagName << ">";
    }
}

void HtmlGenerator::visit(TextNode& node) {
    output << node.content;
}

void HtmlGenerator::visit(AttributeNode& node) {
    output << node.key << "=\"" << node.value << "\"";
}

void HtmlGenerator::visit(StyleBlockNode& node) {
    // Intentionally empty. Processed by ElementNode visitor.
}

void HtmlGenerator::visit(CssPropertyNode& node) {
    // Intentionally empty. Processed by parent.
}

void HtmlGenerator::visit(CssRuleNode& node) {
    // Intentionally empty. Processed by GlobalStyleVisitor and ElementNode visitor.
}

} // namespace CHTL
