#include "NodeCloner.h"

#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/FragmentNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"


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
        case NodeType::Origin: {
            const auto* source = static_cast<const OriginNode*>(node);
            return std::make_unique<OriginNode>(source->type, source->content, source->name);
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
        case NodeType::StyleTemplate: {
            const auto* source = static_cast<const StyleTemplateNode*>(node);
            auto cloned = std::make_unique<StyleTemplateNode>();
            cloned->isCustom = source->isCustom;
            cloned->styles = source->styles;
            cloned->valuelessProperties = source->valuelessProperties;
            return cloned;
        }
        case NodeType::ElementTemplate: {
            const auto* source = static_cast<const ElementTemplateNode*>(node);
            auto cloned = std::make_unique<ElementTemplateNode>();
            cloned->isCustom = source->isCustom;
            for (const auto& child : source->children) {
                cloned->children.push_back(clone(child.get()));
            }
            return cloned;
        }
        case NodeType::VarTemplate: {
            const auto* source = static_cast<const VarTemplateNode*>(node);
            auto cloned = std::make_unique<VarTemplateNode>();
            cloned->isCustom = source->isCustom;
            cloned->variables = source->variables;
            return cloned;
        }
        default:
            // Other node types are not meant to be cloned.
            return nullptr;
    }
}
