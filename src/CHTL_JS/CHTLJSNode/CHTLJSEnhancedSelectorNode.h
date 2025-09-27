#pragma once

#include "CHTLJSNode.h"
#include <string>

class CHTLJSEnhancedSelectorNode : public CHTLJSNode {
public:
    explicit CHTLJSEnhancedSelectorNode(std::string selector_text);
    CHTLJSNodeType getType() const override;

    std::string selector_text;
};