#include "ASTUtil.h"
#include "../CHTLNode/FragmentNode.h" // Add missing include
#include <iostream>

// Helper to check if a single element node matches a single selector part.
bool elementMatchesPart(const ElementNode* element, const SelectorPart& part) {
    if (!element) {
        return false;
    }
    if (!part.tagName.empty() && element->tagName != part.tagName) {
        return false;
    }
    if (!part.id.empty()) {
        auto it = element->attributes.find("id");
        if (it == element->attributes.end() || !it->second || it->second->toString() != part.id) {
            return false;
        }
    }
    if (!part.className.empty()) {
        auto it = element->attributes.find("class");
        if (it == element->attributes.end() || !it->second || it->second->toString().find(part.className) == std::string::npos) {
            // Basic check, doesn't handle whole-word matching perfectly but is an improvement.
            return false;
        }
    }
    return true;
}

// Recursive helper to find all nodes matching a selector part within a subtree.
void findNodesRecursive(const BaseNode* currentNode, const SelectorPart& part, std::vector<const ElementNode*>& foundNodes) {
    if (!currentNode) {
        return;
    }

    if (currentNode->getType() == NodeType::Element) {
        const auto* element = static_cast<const ElementNode*>(currentNode);
        if (elementMatchesPart(element, part)) {
            foundNodes.push_back(element);
        }

        // Continue searching in children
        for (const auto& child : element->children) {
            findNodesRecursive(child.get(), part, foundNodes);
        }
    }
    // If the node is a fragment, search its children
    else if (currentNode->getType() == NodeType::Fragment) {
        const auto* fragment = static_cast<const FragmentNode*>(currentNode);
        for (const auto& child : fragment->children) {
            findNodesRecursive(child.get(), part, foundNodes);
        }
    }
}


const ElementNode* findNodeBySelector(const std::vector<std::unique_ptr<BaseNode>>& nodes, const Selector& selector) {
    if (selector.parts.empty()) {
        return nullptr;
    }

    // This vector holds the set of nodes to search within for the *next* part of the selector.
    // Initially, it's the root nodes of the AST.
    std::vector<const BaseNode*> searchScope(nodes.size());
    for(size_t i = 0; i < nodes.size(); ++i) {
        searchScope[i] = nodes[i].get();
    }

    std::vector<const ElementNode*> finalMatches;

    // Iterate through each part of the descendant selector (e.g., ".container", then "div")
    for (size_t i = 0; i < selector.parts.size(); ++i) {
        const auto& part = selector.parts[i];
        std::vector<const ElementNode*> currentPartMatches;

        // Search for nodes matching the current part within the current scope.
        for (const auto* scopeNode : searchScope) {
            // For the first part, we search the whole subtree of each root node.
            // For subsequent parts, we only search the subtrees of the previous matches.
            findNodesRecursive(scopeNode, part, currentPartMatches);
        }

        if (currentPartMatches.empty()) {
            return nullptr; // If any part of the selector matches nothing, the whole selector fails.
        }

        // If this is the last part of the selector, we're done.
        if (i == selector.parts.size() - 1) {
            finalMatches = currentPartMatches;
        } else {
            // Otherwise, the matches from this part become the search scope for the next part.
            searchScope.assign(currentPartMatches.begin(), currentPartMatches.end());
        }
    }

    if (finalMatches.empty()) {
        return nullptr;
    }

    // Handle index if provided
    const auto& lastPart = selector.parts.back();
    if (lastPart.index != -1) {
        if (lastPart.index >= 0 && static_cast<size_t>(lastPart.index) < finalMatches.size()) {
            return finalMatches[lastPart.index];
        } else {
            return nullptr; // Index out of bounds
        }
    }

    // If no index, return the first match.
    return finalMatches[0];
}
