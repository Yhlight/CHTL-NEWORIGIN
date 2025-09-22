#pragma once

#include "StyleProperty.h"
#include <string>
#include <vector>

struct CssRule {
    std::string selector;
    StylePropertyList properties;

    // The selector of the element this rule was defined in.
    // Used for resolving '&' references.
    std::string context_selector;
};

using CssRuleList = std::vector<CssRule>;
