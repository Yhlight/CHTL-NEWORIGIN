#include "CHTLJSEnhancedSelectorNode.h"

CHTLJSEnhancedSelectorNode::CHTLJSEnhancedSelectorNode(std::string selector_text)
    : selector_text(std::move(selector_text)) {}

CHTLJSNodeType CHTLJSEnhancedSelectorNode::getType() const {
    return CHTLJSNodeType::EnhancedSelector;
}