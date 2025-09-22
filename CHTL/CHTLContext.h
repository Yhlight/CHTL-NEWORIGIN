#pragma once

#include "CHTLNode/Node.h"
#include <string>
#include <unordered_map>
#include <memory>

// A class to hold the state and symbol tables during parsing.
class ParsingContext {
public:
    // The context takes ownership of the template definitions.
    std::unordered_map<std::string, std::unique_ptr<TemplateDefinitionNode>> style_templates;
};
