#include "NodeCloner.h"

#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/FragmentNode.h"

// Recursively performs a deep copy of a given node.
std::unique_ptr<BaseNode> NodeCloner::clone(const BaseNode* node) {
    if (!node) {
        return nullptr;
    }

    switch (node->getType()) {
        case NodeType::Element: {
            const auto* source = static_cast<const ElementNode*>(node);
            auto cloned = std::make_unique<ElementNode>(source->tagName);
            cloned->attributes = source->attributes;
            for (const auto& child : source->children) {
                cloned->children.push_back(clone(child.get()));
            }
            return cloned;
        }
        case NodeType::Text: {
            const auto* source = static_cast<const TextNode*>(node);
            return std::make_unique<TextNode>(source->text);
        }
        case NodeType::Comment: {
            const auto* source = static_cast<const CommentNode*>(node);
            return std::make_unique<CommentNode>(source->text);
        }
        case NodeType::Fragment: {
            const auto* source = static_cast<const FragmentNode*>(node);
            auto cloned = std::make_unique<FragmentNode>();
            for (const auto& child : source->children) {
                cloned->children.push_back(clone(child.get()));
            }
            return cloned;
        }
        default:
            // Other node types (like template definitions) are not meant to be
            // cloned and placed in the main AST, so we ignore them.
            return nullptr;
    }
}
