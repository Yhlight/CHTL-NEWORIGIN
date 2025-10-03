#include "HtmlGenerator.h"
#include "../Util/ExpressionEvaluator/ExpressionEvaluator.h"
#include <vector>
#include <set>

HtmlGenerator::HtmlGenerator() {
    selfClosingTags = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"};
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
    resultStream << "  <style>\n";
    if (!hoistedCss.str().empty()) {
        resultStream << hoistedCss.str();
    }
    resultStream << "  </style>\n";
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
    // Collect variables from conditional properties to avoid rendering them as attributes
    std::set<std::string> conditionalVars;
    for (const auto& child : node.getChildren()) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& prop : styleNode->getProperties()) {
                if (const auto* condProp = std::get_if<ConditionalPropertyValue>(&prop.second)) {
                    for (const auto& branch : condProp->branches) {
                        for (const auto& token : branch.condition) {
                            if (token.type == TokenType::Identifier) {
                                conditionalVars.insert(token.value);
                            }
                        }
                    }
                }
            }
        }
    }

    std::string indent(depth * 2, ' ');
    resultStream << indent << "<" << node.getTagName();

    // Handle attributes, skipping those used as variables in conditionals
    for (const auto& attr : node.getAttributes()) {
        if (conditionalVars.find(attr.first) == conditionalVars.end()) {
            resultStream << " " << attr.first << "=\"" << attr.second << "\"";
        }
    }

    // Collect all inline styles from StyleNode and IfNode children
    std::vector<std::pair<std::string, std::string>> inlineStyles;
    ExpressionEvaluator evaluator(node);

    for (const auto& child : node.getChildren()) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& prop : styleNode->getProperties()) {
                std::string finalValue;
                if (const auto* stringVal = std::get_if<std::string>(&prop.second)) {
                    finalValue = *stringVal;
                } else if (const auto* condVal = std::get_if<ConditionalPropertyValue>(&prop.second)) {
                    finalValue = evaluator.resolveConditionalProperty(*condVal);
                }

                if (!finalValue.empty()) {
                    inlineStyles.push_back({prop.first, finalValue});
                }
            }
        } else if (auto* ifNode = dynamic_cast<IfNode*>(child.get())) {
            bool conditionMet = false;
            auto* currentIf = ifNode;

            while(currentIf && !conditionMet) {
                // Check for else block (empty condition) or evaluate condition
                if (currentIf->getConditionTokens().empty() || evaluator.evaluate(currentIf->getConditionTokens())) {
                    conditionMet = true;
                    for (const auto& prop : currentIf->getProperties()) {
                        std::string finalValue;
                        if (const auto* stringVal = std::get_if<std::string>(&prop.second)) {
                            finalValue = *stringVal;
                        } else if (const auto* condVal = std::get_if<ConditionalPropertyValue>(&prop.second)) {
                            finalValue = evaluator.resolveConditionalProperty(*condVal);
                        }

                        if (!finalValue.empty()) {
                            inlineStyles.push_back({prop.first, finalValue});
                        }
                    }
                }

                // Move to the else branch if it exists
                if (!conditionMet && currentIf->getElseBranch()) {
                    currentIf = dynamic_cast<IfNode*>(currentIf->getElseBranch().get());
                } else {
                    currentIf = nullptr; // End of chain
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

void HtmlGenerator::visit(IfNode& node) {
    // This visitor is a no-op because the logic for if-statements is
    // handled within the parent ElementNode's visit method. The if-statement
    // modifies the parent's styles rather than emitting its own output.
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