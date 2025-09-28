#pragma once

#include "CHTLJSNode.h"
#include <string>

class CHTLJSPlaceholderNode : public CHTLJSNode {
public:
    explicit CHTLJSPlaceholderNode(std::string code);
    CHTLJSNodeType getType() const override;
    std::string getCode() const;

private:
    std::string code;
};