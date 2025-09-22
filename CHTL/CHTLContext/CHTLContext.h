#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

// Forward declare to avoid circular dependencies if context needs to hold nodes
struct BaseNode;

class CHTLContext {
public:
    // For Style Templates: maps template name to a map of (property, value)
    std::map<std::string, std::map<std::string, std::string>> styleTemplates;

    // We will add maps for Element and Var templates here later
};
