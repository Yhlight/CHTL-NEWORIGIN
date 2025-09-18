#pragma once

#include "CHTLNode/TemplateNode.h"
#include <string>
#include <map>
#include <memory>

namespace CHTL {

// The CHTLContext holds the state needed during parsing, such as defined templates.
class CHTLContext {
public:
    // Using shared_ptr to allow multiple references to the same template definition
    // without complex lifetime management.
    std::map<std::string, std::shared_ptr<StyleTemplateNode>> styleTemplates;
    std::map<std::string, std::shared_ptr<ElementTemplateNode>> elementTemplates;

    // We can add other context-sensitive information here later, like imported namespaces,
    // configuration settings, etc.
};

} // namespace CHTL
