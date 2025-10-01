#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include <string>
#include <memory>
#include <sstream>
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLEvaluator/ExpressionEvaluator.h"
#include "CHTLManager/TemplateManager.h"

namespace CHTL {

class ElementNode;
class StyleNode;
class TextNode;
class ScriptNode;
class TemplateDefinitionNode;
class TemplateUsageNode;

class CHTLGenerator {
public:
    CHTLGenerator();
    std::string generate(const DocumentNode* root);

private:
    ExpressionEvaluator m_evaluator;
    std::stringstream m_global_styles;
    const BaseNode* m_root = nullptr;
    TemplateManager& m_template_manager;

    std::string generateNode(const BaseNode* node);
    std::string generateElementNode(const ElementNode* node);
    void generateStyleNode(const StyleNode* node);
    std::string generateTextNode(const TextNode* node);
    std::string generateScriptNode(const ScriptNode* node);
    void generateTemplateDefinition(const TemplateDefinitionNode* node);
    std::string generateTemplateUsage(const TemplateUsageNode* node);
};

}

#endif // CHTL_GENERATOR_H