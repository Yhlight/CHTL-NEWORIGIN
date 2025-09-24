#ifndef CHTL_NAMESPACE_DATA_H
#define CHTL_NAMESPACE_DATA_H

#include "../CHTLNode/TemplateNode.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

struct NamespaceData {
    std::string name;
    std::map<std::string, TemplateStyleGroup> styleTemplates;
    std::map<std::string, TemplateVarGroup> varTemplates;
    std::map<std::string, TemplateElementGroup> elementTemplates;
};

} // namespace CHTL

#endif // CHTL_NAMESPACE_DATA_H
