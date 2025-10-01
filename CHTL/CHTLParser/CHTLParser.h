#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include <vector>
#include <memory>
#include <string>
#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLNode/ConditionalExpressionNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/CustomDefinitionNode.h"
#include "../CHTLNode/DeleteNode.h"
#include "../CHTLNode/InsertNode.h"
#include "../CHTLNode/OriginNode.h"

namespace CHTL {

class CHTLParser {
public:
    CHTLParser(const std::string& input, const std::vector<Token>& tokens);
    std::unique_ptr<DocumentNode> parse();

private:
    const std::string& m_input;
    const std::vector<Token>& tokens;
    size_t current = 0;

    // Token manipulation helpers
    Token peek() const;
    Token advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);

    // Directive parsing
    void parseUseDirective(DocumentNode* doc);

    // Statement-level parsing
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElementNode();
    std::unique_ptr<BaseNode> parseTextNode();
    std::unique_ptr<StyleNode> parseStyleNode(ElementNode* parent);
    std::unique_ptr<ScriptNode> parseScriptNode();
    void parseAttribute(ElementNode* element);
    std::unique_ptr<TemplateDefinitionNode> parseTemplateDefinition();
    std::unique_ptr<CustomDefinitionNode> parseCustomDefinition();
    std::unique_ptr<TemplateUsageNode> parseTemplateUsage();
    std::unique_ptr<OriginNode> parseOriginNode();

    // Expression parsing (Pratt/Precedence Climbing)
    std::unique_ptr<ExpressionNode> parseExpression();
    std::unique_ptr<ExpressionNode> parseTernary();
    std::unique_ptr<ExpressionNode> parseLogicalOr();
    std::unique_ptr<ExpressionNode> parseLogicalAnd();
    std::unique_ptr<ExpressionNode> parseEquality();
    std::unique_ptr<ExpressionNode> parseComparison();
    std::unique_ptr<ExpressionNode> parseTerm();
    std::unique_ptr<ExpressionNode> parseFactor();
    std::unique_ptr<ExpressionNode> parsePower();
    std::unique_ptr<ExpressionNode> parsePrimary();
};

} // namespace CHTL

#endif // CHTL_PARSER_H