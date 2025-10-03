#include "HtmlGenerator.h"
#include "../Util/ExpressionEvaluator/ExpressionEvaluator.h"
#include <vector>
#include <set>
#include <algorithm> // For std::replace
#include <cctype>    // For toupper

HtmlGenerator::HtmlGenerator() : dynamicIdCounter(0) {
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

    if (!dynamicScripts.str().empty()) {
        resultStream << "<script>\n" << dynamicScripts.str() << "</script>\n";
    }

    resultStream << "</html>\n";
}

void HtmlGenerator::visit(ElementNode& node) {
    // Check for dynamic if children to decide if an ID is needed
    const auto& attrs = node.getAttributes();
    auto it = attrs.find("id");
    std::string elementId = (it != attrs.end()) ? it->second : "";
    bool hasDynamicIf = false;
    const IfNode* firstDynamicIf = nullptr;

    for (const auto& child : node.getChildren()) {
        if (const auto* ifNode = dynamic_cast<IfNode*>(child.get())) {
            if (ifNode->isDynamic()) {
                hasDynamicIf = true;
                firstDynamicIf = ifNode; // We only need to trigger generation for the first in a chain
                break;
            }
        }
    }

    if (hasDynamicIf && elementId.empty()) {
        elementId = "chtl-dynamic-" + std::to_string(dynamicIdCounter++);
        const_cast<ElementNode&>(node).setAttribute("id", elementId);
    }

    std::string indent(depth * 2, ' ');
    resultStream << indent << "<" << node.getTagName();

    // Handle attributes
    for (const auto& attr : node.getAttributes()) {
        resultStream << " " << attr.first << "=\"" << attr.second << "\"";
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
                } else if (const auto* arithExpr = std::get_if<ArithmeticExpression>(&prop.second)) {
                    finalValue = evaluator.resolveArithmeticExpression(*arithExpr);
                }

                if (!finalValue.empty()) {
                    inlineStyles.push_back({prop.first, finalValue});
                }
            }
        } else if (auto* ifNode = dynamic_cast<IfNode*>(child.get())) {
            if (ifNode->isDynamic()) {
                if (ifNode == firstDynamicIf) {
                    generateDynamicScript(elementId, *ifNode);
                }
            } else {
                bool conditionMet = false;
                auto* currentIf = ifNode;
                while(currentIf && !conditionMet) {
                    if (currentIf->getConditionTokens().empty() || evaluator.evaluate(currentIf->getConditionTokens())) {
                        conditionMet = true;
                        for (const auto& prop : currentIf->getProperties()) {
                            std::string finalValue;
                            if (const auto* stringVal = std::get_if<std::string>(&prop.second)) {
                                finalValue = *stringVal;
                            } else if (const auto* condVal = std::get_if<ConditionalPropertyValue>(&prop.second)) {
                                finalValue = evaluator.resolveConditionalProperty(*condVal);
                            } else if (const auto* arithExpr = std::get_if<ArithmeticExpression>(&prop.second)) {
                                finalValue = evaluator.resolveArithmeticExpression(*arithExpr);
                            }

                            if (!finalValue.empty()) {
                                inlineStyles.push_back({prop.first, finalValue});
                            }
                        }
                    }
                    if (!conditionMet && currentIf->getElseBranch()) {
                        currentIf = dynamic_cast<IfNode*>(currentIf->getElseBranch().get());
                    } else {
                        currentIf = nullptr;
                    }
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
    resultStream << indent << node.toString(0);
}

void HtmlGenerator::visit(CommentNode& node) {
    std::string indent(depth * 2, ' ');
    resultStream << indent << node.toString(0);
}

void HtmlGenerator::visit(StyleNode& node) {
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
    // No-op, handled by parent ElementNode
}

void HtmlGenerator::findStyleNodes(BaseNode* node, std::vector<StyleNode*>& styleNodes) {
    if (!node) return;

    if (auto* elementNode = dynamic_cast<ElementNode*>(node)) {
        for (const auto& child : elementNode->getChildren()) {
            if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
                styleNodes.push_back(styleNode);
            }
            findStyleNodes(child.get(), styleNodes);
        }
    } else if (auto* docNode = dynamic_cast<DocumentNode*>(node)) {
        for (const auto& child : docNode->getChildren()) {
            findStyleNodes(child.get(), styleNodes);
        }
    }
}

std::string HtmlGenerator::translateTokensToJs(const std::vector<Token>& tokens) {
    std::stringstream js;
    bool inDynamicSelector = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::DynamicSelectorStart) {
            inDynamicSelector = true;
            continue;
        }
        if (token.type == TokenType::DynamicSelectorEnd) {
            inDynamicSelector = false;
            continue;
        }

        if (inDynamicSelector) {
            // A more robust solution would parse this, but for now we just output the value.
            // e.g. {{obj.prop}} -> obj.prop
            js << token.value;
        } else {
            js << token.value;
        }
        js << " ";
    }
    return js.str();
}

void HtmlGenerator::generateDynamicScript(const std::string& elementId, const IfNode& rootIfNode) {
    std::string functionName = "update_" + elementId;
    // Sanitize function name
    std::replace(functionName.begin(), functionName.end(), '-', '_');

    dynamicScripts << "function " << functionName << "() {\n";
    dynamicScripts << "  const element = document.getElementById('" << elementId << "');\n";
    dynamicScripts << "  if (!element) return;\n";

    const IfNode* currentIf = &rootIfNode;
    bool first = true;
    while (currentIf) {
        if (currentIf->getConditionTokens().empty()) {
            dynamicScripts << "  } else {\n";
        } else {
            std::string jsCondition = translateTokensToJs(currentIf->getConditionTokens());
            if (first) {
                dynamicScripts << "  if (" << jsCondition << ") {\n";
                first = false;
            } else {
                dynamicScripts << "  } else if (" << jsCondition << ") {\n";
            }
        }

        for (const auto& prop : currentIf->getProperties()) {
            if (const auto* val = std::get_if<std::string>(&prop.second)) {
                 std::string jsProp = prop.first;
                 size_t hyphenPos;
                 while((hyphenPos = jsProp.find('-')) != std::string::npos) {
                     jsProp.erase(hyphenPos, 1);
                     if (hyphenPos < jsProp.length()) {
                         jsProp[hyphenPos] = toupper(jsProp[hyphenPos]);
                     }
                 }
                 std::string propVal = *val;
                 // If the parser provided a quoted string, un-quote it first.
                 if (propVal.length() >= 2 && propVal.front() == '"' && propVal.back() == '"') {
                     propVal = propVal.substr(1, propVal.length() - 2);
                 }

                 // escape single quotes for JS
                 size_t pos = 0;
                 while ((pos = propVal.find('\'', pos)) != std::string::npos) {
                     propVal.replace(pos, 1, "\\'");
                     pos += 2;
                 }
                 dynamicScripts << "    element.style." << jsProp << " = '" << propVal << "';\n";
            }
        }

        // Move to the next branch in the if-else chain
        currentIf = currentIf->getElseBranch() ? dynamic_cast<IfNode*>(currentIf->getElseBranch().get()) : nullptr;
    }

    dynamicScripts << "  }\n";
    dynamicScripts << "}\n\n";

    dynamicScripts << "window.addEventListener('DOMContentLoaded', " << functionName << ");\n";
    dynamicScripts << "window.addEventListener('resize', " << functionName << ");\n\n";
}