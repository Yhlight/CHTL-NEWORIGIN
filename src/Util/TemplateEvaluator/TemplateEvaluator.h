#pragma once

#include "../../CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <map>
#include <memory>

class TemplateDefinitionNode;
class CustomDefinitionNode;

class TemplateEvaluator {
public:
    void evaluate(BaseNode* root);

private:
    void findDefinitions(BaseNode* node);
    void processNode(BaseNode* node);

    BaseNode* astRoot;
    std::map<std::string, const TemplateDefinitionNode*> templateDefinitions;
    std::map<std::string, const CustomDefinitionNode*> customDefinitions;
};