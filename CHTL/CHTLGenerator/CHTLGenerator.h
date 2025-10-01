#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include <string>
#include <sstream>
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/CssRuleNode.h"
#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/BinaryOpNode.h"

namespace CHTL {

class CHTLGenerator {
public:
    std::string generate(const BaseNode& root);

private:
    void visit(const BaseNode* node);
    void visitElementNode(const ElementNode* node);
    void visitTextNode(const TextNode* node);
    void visitStyleNode(const ElementNode* parent, const StyleNode* node);
    std::string visitExpression(const ExpressionNode* node);

    std::stringstream html_ss;
    std::stringstream css_ss;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H