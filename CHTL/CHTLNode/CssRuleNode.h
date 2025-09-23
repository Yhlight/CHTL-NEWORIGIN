#pragma once

#include "BaseNode.h"
#include "../CHTLExpr/BaseExprNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

// Represents a full CSS rule (e.g., .box { width: 100px; })
// to be stored in the context and evaluated later.
struct CssRuleNode : public BaseNode {
    std::string selector;
    std::map<std::string, std::shared_ptr<BaseExprNode>> properties;
};
