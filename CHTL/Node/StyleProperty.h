#pragma once

#include <string>
#include <vector>

struct StyleProperty {
    std::string key;
    std::string value;
};

using StylePropertyList = std::vector<StyleProperty>;
