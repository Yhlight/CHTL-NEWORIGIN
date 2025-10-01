#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include <string>
#include <memory>
#include "CHTLNode/BaseNode.h"
#include "CHTLEvaluator/ExpressionEvaluator.h"

namespace CHTL {

class ElementNode;
class StyleNode;
class TextNode;
class ScriptNode;

class CHTLGenerator {
public:
    CHTLGenerator();
    std::string generate(const BaseNode* root);

private:
    ExpressionEvaluator m_evaluator;
    std::string generateNode(const BaseNode* node);
    std::string generateElementNode(const ElementNode* node);
    std::string generateStyleNode(const StyleNode* node);
    std::string generateTextNode(const TextNode* node);
    std::string generateScriptNode(const ScriptNode* node);
};

}

#endif // CHTL_GENERATOR_H