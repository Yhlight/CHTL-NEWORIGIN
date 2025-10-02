#include "HtmlGenerator.h"
#include "ExpressionEvaluator/ExpressionEvaluator.h"
#include <vector>

HtmlGenerator::HtmlGenerator() {
    selfClosingTags = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"};
}

// Helper to get inline style string from a StyleNode
std::string getInlineStyle(const StyleNode* styleNode) {
    if (!styleNode) return "";
    std::stringstream ss;
    const auto& properties = styleNode->getProperties();
    for (size_t i = 0; i < properties.size(); ++i) {
        ss << properties[i].first << ": " << properties[i].second << ";";
        if (i < properties.size() - 1) {
            ss << " ";
        }
    }
    return ss.str();
}

std::string HtmlGenerator::getResult() {
    return resultStream.str();
}

void HtmlGenerator::visit(DocumentNode& node) {
    if (node.getHasHtml5Doctype()) {
        resultStream << "<!DOCTYPE html>\n";
    }
    resultStream << "<html>\n";

    // First pass: collect all CSS rules for hoisting
    std::vector<StyleNode*> styleNodes;
    findStyleNodes(&node, styleNodes);
    for (auto* styleNode : styleNodes) {
        styleNode->accept(*this);
    }

    // Generate head with hoisted styles
    resultStream << "<head>\n";
    if (!hoistedCss.str().empty()) {
        resultStream << "  <style>\n" << hoistedCss.str() << "  </style>\n";
    }
    resultStream << "</head>\n";

    // Generate body
    resultStream << "<body>\n";
    depth++;
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
    depth--;
    resultStream << "</body>\n";

    resultStream << "</html>\n";
}

void HtmlGenerator::visit(ElementNode& node) {
    std::string indent(depth * 2, ' ');
    resultStream << indent << "<" << node.getTagName();

    // Handle attributes
    for (const auto& attr : node.getAttributes()) {
        resultStream << " " << attr.first << "=\"" << attr.second << "\"";
    }

    // Collect all inline styles from StyleNode and IfNode children
    std::vector<std::pair<std::string, std::string>> inlineStyles;

    for (const auto& child : node.getChildren()) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& prop : styleNode->getProperties()) {
                inlineStyles.push_back(prop);
            }
        } else if (auto* ifNode = dynamic_cast<IfNode*>(child.get())) {
            bool conditionMet = false;
            auto* currentIf = ifNode;

            while(currentIf && !conditionMet) {
                ExpressionEvaluator evaluator(node);
                // Check for else block (empty condition) or evaluate condition
                if (currentIf->getConditionTokens().empty() || evaluator.evaluate(currentIf->getConditionTokens())) {
                    conditionMet = true;
                    for (const auto& prop : currentIf->getProperties()) {
                        inlineStyles.push_back(prop);
                    }
                }

                // Move to the else branch if it exists
                if (!conditionMet && currentIf->getElseBranch()) {
                    currentIf = dynamic_cast<IfNode*>(currentIf->getElseBranch().get());
                } else {
                    break;
                }
            }
        }
    }

    if (!inlineStyles.empty()) {
        resultStream << " style=\"";
        for (size_t i = 0; i < inlineStyles.size(); ++i) {
            resultStream << inlineStyles[i].first << ": " << inlineStyles[i].second << ";";
            if (i < inlineStyles.size() - 1) {
                resultStream << " ";
            }
        }
        resultStream << "\"";
    }

    if (selfClosingTags.count(node.getTagName())) {
        resultStream << " />\n";
    } else {
        resultStream << ">\n";
        depth++;
        for (const auto& child : node.getChildren()) {
            // StyleNodes and IfNodes are processed for styles, so skip direct rendering
            if (dynamic_cast<StyleNode*>(child.get()) == nullptr && dynamic_cast<IfNode*>(child.get()) == nullptr) {
                child->accept(*this);
            }
        }
        depth--;
        resultStream << indent << "</" << node.getTagName() << ">\n";
    }
}

void HtmlGenerator::visit(TextNode& node) {
    std::string indent(depth * 2, ' ');
    resultStream << indent << node.toString(0); // Use existing simple to string for content
}

void HtmlGenerator::visit(CommentNode& node) {
    std::string indent(depth * 2, ' ');
    resultStream << indent << node.toString(0); // Use existing simple to string for content
}

void HtmlGenerator::visit(StyleNode& node) {
    // This visit is for hoisting CSS rules. Inline styles are handled by ElementNode visitor.
    for (const auto& rule : node.getRules()) {
        rule->accept(*this);
    }
}

void HtmlGenerator::visit(CssRuleNode& node) {
    std::string indent(depth * 2, ' ');
    hoistedCss << indent << "  " << node.getSelector() << " {\n";
    for (const auto& prop : node.getProperties()) {
        hoistedCss << indent << "    " << prop.first << ": " << prop.second << ";\n";
    }
    hoistedCss << indent << "  }\n";
}

void HtmlGenerator::findStyleNodes(BaseNode* node, std::vector<StyleNode*>& styleNodes) {
    if (!node) return;

    if (auto* elementNode = dynamic_cast<ElementNode*>(node)) {
        for (const auto& child : elementNode->getChildren()) {
            if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
                styleNodes.push_back(styleNode);
            }
            // Recurse into children of elements
            findStyleNodes(child.get(), styleNodes);
        }
    } else if (auto* docNode = dynamic_cast<DocumentNode*>(node)) {
        for (const auto& child : docNode->getChildren()) {
            findStyleNodes(child.get(), styleNodes);
        }
    }
    // Other node types (like NamespaceNode) could be handled here if they can contain elements
}