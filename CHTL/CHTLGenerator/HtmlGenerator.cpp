#include "HtmlGenerator.h"
#include <vector>
#include <algorithm>
#include <iostream>

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
    this->rootNode = &node; // Store root node for lookups
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

    const StyleNode* styleNode = nullptr;
    for (const auto& child : node.getChildren()) {
        if (auto* sn = dynamic_cast<StyleNode*>(child.get())) {
            styleNode = sn;
            break;
        }
    }

    std::string inlineStyle = getInlineStyle(styleNode);
    std::stringstream conditionalStyles;

    for (const auto& conditionChain : node.getConditions()) {
        ConditionNode* currentCond = conditionChain.get();
        bool conditionMet = false;
        while(currentCond && !conditionMet) {
            if (evaluateCondition(currentCond->getCondition(), node)) {
                for (const auto& prop : currentCond->getProperties()) {
                    conditionalStyles << prop.first << ": " << prop.second << "; ";
                }
                conditionMet = true;
            }
            currentCond = currentCond->getNext();
        }
    }

    if (!conditionalStyles.str().empty()) {
        if (!inlineStyle.empty() && inlineStyle.back() != ' ') {
            inlineStyle += " ";
        }
        inlineStyle += conditionalStyles.str();
    }

    if (!inlineStyle.empty()) {
        resultStream << " style=\"" << inlineStyle << "\"";
    }

    resultStream << ">\n";
    depth++;
    for (const auto& child : node.getChildren()) {
        if (dynamic_cast<StyleNode*>(child.get()) == nullptr) {
            child->accept(*this);
        }
    }
    depth--;
    resultStream << indent << "</" << node.getTagName() << ">\n";
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

void HtmlGenerator::visit(ConditionNode& node) {
    // Logic is handled within ElementNode visitor
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

ElementNode* HtmlGenerator::findElement(BaseNode* startNode, const std::string& selector) {
    if (!startNode) return nullptr;

    if (auto* elementNode = dynamic_cast<ElementNode*>(startNode)) {
        if (elementNode->getTagName() == selector) {
            return elementNode;
        }
    }

    if (auto* docNode = dynamic_cast<DocumentNode*>(startNode)) {
        for (const auto& child : docNode->getChildren()) {
            if (auto* found = findElement(child.get(), selector)) return found;
        }
    } else if (auto* elementNode = dynamic_cast<ElementNode*>(startNode)) {
         for (const auto& child : elementNode->getChildren()) {
            if (auto* found = findElement(child.get(), selector)) return found;
        }
    }
    return nullptr;
}

double HtmlGenerator::getNumericValue(const std::string& value) {
    try {
        size_t i = 0;
        while (i < value.length() && !isdigit(value[i]) && value[i] != '-') i++;
        return std::stod(value.substr(i));
    } catch (const std::exception&) {
        return 0.0;
    }
}

bool HtmlGenerator::evaluateCondition(const std::string& condition, ElementNode& contextNode) {
    if (condition.empty()) return true; // 'else' case

    std::stringstream ss(condition);
    std::string lhs, op, rhs;
    ss >> lhs >> op >> rhs;

    if (lhs.empty() || op.empty() || rhs.empty()) {
        std::cerr << "Warning: Malformed condition: '" << condition << "'\n";
        return false;
    }

    size_t dotPos = lhs.find('.');
    std::string selector = (dotPos != std::string::npos) ? lhs.substr(0, dotPos) : "";
    std::string propertyName = (dotPos != std::string::npos) ? lhs.substr(dotPos + 1) : lhs;

    ElementNode* targetElement = selector.empty() ? &contextNode : findElement(rootNode, selector);
    if (!targetElement) {
        std::cerr << "Warning: Element '" << selector << "' not found for condition.\n";
        return false;
    }

    const StyleNode* styleNode = nullptr;
    for (const auto& child : targetElement->getChildren()) {
        if ((styleNode = dynamic_cast<StyleNode*>(child.get()))) break;
    }

    double lhsValue = 0.0;
    if (styleNode) {
        const auto& properties = styleNode->getProperties();
        auto it = std::find_if(properties.begin(), properties.end(),
                               [&](const auto& p) { return p.first == propertyName; });
        if (it != properties.end()) {
            lhsValue = getNumericValue(it->second);
        } else {
            std::cerr << "Warning: Property '" << propertyName << "' not found.\n";
            return false;
        }
    } else {
        std::cerr << "Warning: StyleNode not found for element in condition.\n";
        return false;
    }

    double rhsValue = getNumericValue(rhs);

    if (op == "<") return lhsValue < rhsValue;
    if (op == ">") return lhsValue > rhsValue;
    if (op == "<=") return lhsValue <= rhsValue;
    if (op == ">=") return lhsValue >= rhsValue;
    if (op == "==") return lhsValue == rhsValue;
    if (op == "!=") return lhsValue != rhsValue;

    std::cerr << "Warning: Unsupported operator '" << op << "'\n";
    return false;
}