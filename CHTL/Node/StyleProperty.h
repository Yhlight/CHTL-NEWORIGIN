#pragma once

#include "CHTL/Expression/Expression.h"
#include <string>
#include <vector>

struct StyleProperty {
    std::string key;
    ExprPtr value;
};

using StylePropertyList = std::vector<StyleProperty>;
