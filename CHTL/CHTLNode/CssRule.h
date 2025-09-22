#pragma once

#include <string>
#include <vector>
#include <map>

// Represents a CSS rule to be added to the global stylesheet
struct CssRule {
    std::string selector;
    std::map<std::string, std::string> properties;
};
