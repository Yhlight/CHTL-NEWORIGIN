#pragma once

#include <string>
#include <vector>

// Represents one part of a selector chain, e.g., "div", ".card", "#main", or "button[0]".
struct SelectorPart {
    std::string tagName;
    std::string id;
    std::string className;
    int index = -1; // -1 means no index is specified.
};

// Represents a full CSS selector, which can be a chain of parts for descendant selection.
// For example, ".container div[0]" would be a Selector with two SelectorParts.
struct Selector {
    std::vector<SelectorPart> parts;
};
