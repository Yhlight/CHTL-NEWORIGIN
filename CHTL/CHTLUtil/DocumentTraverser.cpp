#include "DocumentTraverser.h"
#include "CHTLNode/ElementNode.h"

namespace CHTL {

const ElementNode* DocumentTraverser::find(const BaseNode* root, const std::string& selector) {
    if (!root) {
        return nullptr;
    }

    if (const auto* element = dynamic_cast<const ElementNode*>(root)) {
        bool match = false;
        if (selector[0] == '.') { // Class selector
            if (element->getAttribute("class") == selector.substr(1)) {
                match = true;
            }
        } else if (selector[0] == '#') { // ID selector
            if (element->getAttribute("id") == selector.substr(1)) {
                match = true;
            }
        } else { // Tag selector
            if (element->getTagName() == selector) {
                match = true;
            }
        }

        if (match) {
            return element;
        }
    }

    if (const auto* element = dynamic_cast<const ElementNode*>(root)) {
        for (const auto& child : element->getChildren()) {
            const ElementNode* found = find(child.get(), selector);
            if (found) {
                return found;
            }
        }
    }

    return nullptr;
}

} // namespace CHTL