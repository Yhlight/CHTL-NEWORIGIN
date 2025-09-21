#include "Generator.h"
#include <iostream>

namespace CHTL {

Generator::Generator(const std::vector<NodePtr>& nodes, const std::map<std::string, std::shared_ptr<TemplateNode>>& templates)
    : rootNodes(nodes), templateTable(templates) {}

std::string Generator::generate() {
    for (const auto& node : rootNodes) {
        collectCssRules(node);
    }

    for (const auto& node : rootNodes) {
        visit(node);
    }
    return output.str();
}

void Generator::collectCssRules(const NodePtr& node) {
    if (!node) return;

    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        if (element->styleBlock) {
            for (const auto& rule : element->styleBlock->rules) {
                globalCssRules.push_back(rule);
            }
        }
        for (const auto& child : element->children) {
            collectCssRules(child);
        }
    }
}

void Generator::generateCssRules(const std::vector<std::shared_ptr<CssRuleNode>>& rules) {
    for (const auto& rule : rules) {
        indent();
        output << rule->selector << " {\n";
        indentLevel++;
        for (const auto& prop : rule->properties) {
            indent();
            output << prop.name << ": " << evaluateExpression(prop.value) << ";\n";
        }
        indentLevel--;
        indent();
        output << "}\n";
    }
}

void Generator::generateCss() {
    if (globalCssRules.empty()) return;

    indent();
    output << "<style>\n";
    indentLevel++;
    generateCssRules(globalCssRules);
    indentLevel--;
    indent();
    output << "</style>\n";
}


void Generator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  "; // 2 spaces for indentation
    }
}

void Generator::visit(const NodePtr& node) {
    if (!node) return;
    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        visitElementNode(element);
    } else if (auto text = std::dynamic_pointer_cast<TextNode>(node)) {
        visitTextNode(text);
    } else if (auto usage = std::dynamic_pointer_cast<TemplateUsageNode>(node)) {
        visitTemplateUsageNode(usage);
    }
    // Add cases for other node types here later
}

void Generator::visitTemplateUsageNode(const std::shared_ptr<TemplateUsageNode>& node) {
    if (templateTable.count(node->name)) {
        auto templateNode = templateTable.at(node->name);
        if (templateNode->templateType == TemplateType::ELEMENT) {
            const auto& body = std::get<std::vector<NodePtr>>(templateNode->body);
            for (const auto& bodyNode : body) {
                visit(bodyNode);
            }
        }
        // Handle other template types if necessary, though @Style is handled by parser
    } else {
        // Handle error: template not found
        output << "<!-- Error: Template '" << node->name << "' not found. -->\n";
    }
}

// Simple struct to hold the result of an evaluation
struct EvaluatedValue {
    double numeric_val = 0.0;
    std::string string_val = "";
    std::string unit = "";
    bool is_numeric = false;
};

// Helper to evaluate a node and return the struct
EvaluatedValue evaluate(std::shared_ptr<ExprNode> expr) {
    if (auto num = std::dynamic_pointer_cast<NumericLiteralNode>(expr)) {
        return {num->value, "", num->unit, true};
    }
    if (auto str = std::dynamic_pointer_cast<StringLiteralNode>(expr)) {
        return {0.0, str->value, "", false};
    }
    if (auto binOp = std::dynamic_pointer_cast<BinaryOpNode>(expr)) {
        EvaluatedValue left = evaluate(binOp->left);
        EvaluatedValue right = evaluate(binOp->right);

        if (left.is_numeric && right.is_numeric) {
            // Unit check
            if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
                // For now, return error string. Proper error handling needed.
                return {0.0, "ERROR: Incompatible units", "", false};
            }
            std::string result_unit = !left.unit.empty() ? left.unit : right.unit;
            double result_val = 0.0;
            switch(binOp->op) {
                case TokenType::PLUS: result_val = left.numeric_val + right.numeric_val; break;
                case TokenType::MINUS: result_val = left.numeric_val - right.numeric_val; break;
                case TokenType::STAR: result_val = left.numeric_val * right.numeric_val; break;
                case TokenType::SLASH: result_val = left.numeric_val / right.numeric_val; break;
                default: return {0.0, "ERROR: Unsupported operator", "", false};
            }
            return {result_val, "", result_unit, true};
        }
        // Handle string concatenation if needed
    }
    return {0.0, "ERROR: Unknown expression type", "", false};
}


std::string Generator::evaluateExpression(std::shared_ptr<ExprNode> expr) {
    EvaluatedValue result = evaluate(expr);
    if (result.is_numeric) {
        std::stringstream ss;
        ss << result.numeric_val << result.unit;
        return ss.str();
    }
    return result.string_val;
}


void Generator::visitElementNode(const std::shared_ptr<ElementNode>& node) {
    indent();
    output << "<" << node->tagName;

    for (const auto& attr : node->attributes) {
        output << " " << attr.name << "=\"" << attr.value << "\"";
    }

    if (node->tagName != "style" && node->styleBlock) {
        std::stringstream style_ss;
        // First, resolve and add properties from template usages
        for (const auto& usage : node->styleBlock->templateUsages) {
            if (templateTable.count(usage->name)) {
                auto templateNode = templateTable.at(usage->name);
                if (templateNode->templateType == TemplateType::STYLE) {
                    const auto& props = std::get<std::vector<StyleProperty>>(templateNode->body);
                    for (const auto& prop : props) {
                        style_ss << prop.name << ": " << evaluateExpression(prop.value) << ";";
                    }
                }
            }
        }

        // Then, add the inline properties (they can override template properties)
        for (const auto& prop : node->styleBlock->inlineProperties) {
            style_ss << prop.name << ": " << evaluateExpression(prop.value) << ";";
        }

        if (style_ss.str().length() > 0) {
            output << " style=\"" << style_ss.str() << "\"";
        }
    }

    if (node->children.empty()) {
        // Self-closing tag for simplicity, though not all tags are self-closing in HTML
        // A proper implementation would have a list of void elements (area, base, br, etc.)
        output << " />\n";
    } else {
        output << ">\n";
        indentLevel++;

        if (node->tagName == "style" && node->styleBlock) {
            generateCssRules(node->styleBlock->rules);
        }

        for (const auto& child : node->children) {
            visit(child);
        }

        if (node->tagName == "head") {
            generateCss();
        }

        indentLevel--;
        indent();
        output << "</" << node->tagName << ">\n";
    }
}

void Generator::visitTextNode(const std::shared_ptr<TextNode>& node) {
    indent();
    output << node->text << "\n";
}

} // namespace CHTL
