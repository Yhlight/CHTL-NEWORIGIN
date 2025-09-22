#pragma once

#include "CHTL/Node/Node.h"
#include "CHTL/Node/CssRule.h"
#include <vector>
#include <memory>

struct ParseResult {
    NodeList ast;
    CssRuleList global_styles;
};
