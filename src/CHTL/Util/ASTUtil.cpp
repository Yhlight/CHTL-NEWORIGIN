#include "ASTUtil.h"

// Recursive helper to search from a single node downwards.
const ElementNode* findNodeRecursive(const BaseNode* node, const std::string& selectorValue, char selectorType) {
    if (!node || node->getType() != NodeType::Element) {
        return nullptr;
    }

    const auto* element = static_cast<const ElementNode*>(node);
    bool match = false;

    if (selectorType == '#') {
        auto it = element->attributes.find("id");
        if (it != element->attributes.end() && it->second == selectorValue) {
            match = true;
        }
    } else if (selectorType == '.') {
        auto it = element->attributes.find("class");
        // Simple check for now; doesn't handle space-separated classes.
        if (it != element->attributes.end() && it->second == selectorValue) {
            match = true;
        }
    }

    if (match) {
        return element;
    }

    // If not found on the current node, search its children.
    for (const auto& child : element->children) {
        const ElementNode* found = findNodeRecursive(child.get(), selectorValue, selectorType);
        if (found) {
            return found; // Return the first match found in the subtree.
        }
    }

    return nullptr;
}

const ElementNode* findNodeBySelector(const std::vector<std::unique_ptr<BaseNode>>& nodes, const std::string& selector) {
    if (selector.length() < 2 || (selector[0] != '.' && selector[0] != '#')) {
        return nullptr; // Invalid or unsupported selector.
    }

    char selectorType = selector[0];
    std::string selectorValue = selector.substr(1);

    for (const auto& rootNode : nodes) {
        const ElementNode* found = findNodeRecursive(rootNode.get(), selectorValue, selectorType);
        if (found) {
            return found;
        }
    }

    return nullptr;
}
