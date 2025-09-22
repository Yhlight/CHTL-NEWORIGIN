#pragma once

#include <string>
#include <vector>

struct Attribute {
    std::string key;
    std::string value;
};

using AttributeList = std::vector<Attribute>;
