#pragma once

#include "CHTLJSNode.h"
#include <string>

class RawJavaScriptNode : public CHTLJSNode {
public:
    explicit RawJavaScriptNode(std::string js_code);
    CHTLJSNodeType getType() const override;

    std::string js_code;
};