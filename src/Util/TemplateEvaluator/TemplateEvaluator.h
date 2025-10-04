#pragma once

#include "../../CHTL/CHTLNode/BaseNode.h"
#include <map>
#include <string>

class TemplateDefinitionNode;

class TemplateEvaluator {
public:
    void evaluate(BaseNode* root);

private:
    void findDefinitions(BaseNode* node);
    void expandTemplates(BaseNode* node);

    std::map<std::string, TemplateDefinitionNode*> styleTemplates;
};