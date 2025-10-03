#include "HtmlGenerator.h"
#include "../Util/ExpressionEvaluator/ExpressionEvaluator.h"
#include <vector>
#include <set>
#include <algorithm> // For std::replace
#include <cctype>    // For toupper

HtmlGenerator::HtmlGenerator() : dynamicIdCounter(0) {
    selfClosingTags = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"};
    cssProperties = {
        "align-content", "align-items", "align-self", "all", "animation", "animation-delay", "animation-direction",
        "animation-duration", "animation-fill-mode", "animation-iteration-count", "animation-name", "animation-play-state",
        "animation-timing-function", "backface-visibility", "background", "background-attachment", "background-blend-mode",
        "background-clip", "background-color", "background-image", "background-origin", "background-position",
        "background-repeat", "background-size", "border", "border-bottom", "border-bottom-color", "border-bottom-left-radius",
        "border-bottom-right-radius", "border-bottom-style", "border-bottom-width", "border-collapse", "border-color",
        "border-image", "border-image-outset", "border-image-repeat", "border-image-slice", "border-image-source",
        "border-image-width", "border-left", "border-left-color", "border-left-style", "border-left-width",
        "border-radius", "border-right", "border-right-color", "border-right-style", "border-right-width",
        "border-spacing", "border-style", "border-top", "border-top-color", "border-top-left-radius",
        "border-top-right-radius", "border-top-style", "border-top-width", "border-width", "bottom", "box-shadow",
        "box-sizing", "caption-side", "caret-color", "clear", "clip", "clip-path", "color", "column-count",
        "column-fill", "column-gap", "column-rule", "column-rule-color", "column-rule-style", "column-rule-width",
        "column-span", "column-width", "columns", "content", "counter-increment", "counter-reset", "cursor",
        "direction", "display", "empty-cells", "filter", "flex", "flex-basis", "flex-direction", "flex-flow",
        "flex-grow", "flex-shrink", "flex-wrap", "float", "font", "font-family", "font-kerning", "font-size",
        "font-size-adjust", "font-stretch", "font-style", "font-variant", "font-weight", "grid", "grid-area",
        "grid-auto-columns", "grid-auto-flow", "grid-auto-rows", "grid-column", "grid-column-end", "grid-column-gap",
        "grid-column-start", "grid-gap", "grid-row", "grid-row-end", "grid-row-gap", "grid-row-start", "grid-template",
        "grid-template-areas", "grid-template-columns", "grid-template-rows", "height", "hyphens", "isolation",
        "justify-content", "left", "letter-spacing", "line-height", "list-style", "list-style-image",
        "list-style-position", "list-style-type", "margin", "margin-bottom", "margin-left", "margin-right",
        "margin-top", "max-height", "max-width", "min-height", "min-width", "mix-blend-mode", "object-fit",
        "object-position", "opacity", "order", "orphans", "outline", "outline-color", "outline-offset",
        "outline-style", "outline-width", "overflow", "overflow-x", "overflow-y", "padding", "padding-bottom",
        "padding-left", "padding-right", "padding-top", "page-break-after", "page-break-before", "page-break-inside",
        "perspective", "perspective-origin", "pointer-events", "position", "quotes", "resize", "right",
        "scroll-behavior", "tab-size", "table-layout", "text-align", "text-align-last", "text-decoration",
        "text-decoration-color", "text-decoration-line", "text-decoration-style", "text-indent", "text-justify",
        "text-overflow", "text-shadow", "text-transform", "top", "transform", "transform-origin", "transform-style",
        "transition", "transition-delay", "transition-duration", "transition-property", "transition-timing-function",
        "unicode-bidi", "user-select", "vertical-align", "visibility", "white-space", "widows", "width",
        "word-break", "word-spacing", "word-wrap", "writing-mode", "z-index"
    };
}

bool HtmlGenerator::isStyleProperty(const std::string& name) const {
    return cssProperties.count(name) > 0;
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

    auto finalAttributes = node.getAttributes();
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
                            }

                            if (!finalValue.empty()) {
                                if (isStyleProperty(prop.first)) {
                                    inlineStyles.push_back({prop.first, finalValue});
                                } else {
                                    finalAttributes[prop.first] = finalValue;
                                }
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

    std::string indent(depth * 2, ' ');
    resultStream << indent << "<" << node.getTagName();

    for (const auto& attr : finalAttributes) {
        resultStream << " " << attr.first << "=\"" << attr.second << "\"";
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
                std::string propVal = *val;
                // escape single quotes
                size_t pos = 0;
                while ((pos = propVal.find('\'', pos)) != std::string::npos) {
                    propVal.replace(pos, 1, "\\'");
                    pos += 2;
                }

                if (isStyleProperty(prop.first)) {
                    std::string jsProp = prop.first;
                    size_t hyphenPos;
                    while((hyphenPos = jsProp.find('-')) != std::string::npos) {
                        jsProp.erase(hyphenPos, 1);
                        if (hyphenPos < jsProp.length()) {
                            jsProp[hyphenPos] = toupper(jsProp[hyphenPos]);
                        }
                    }
                    dynamicScripts << "    element.style." << jsProp << " = '" << propVal << "';\n";
                } else {
                    dynamicScripts << "    element.setAttribute('" << prop.first << "', '" << propVal << "');\n";
                }
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