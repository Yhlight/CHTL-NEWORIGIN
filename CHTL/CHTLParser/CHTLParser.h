#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLContext.h"
#include <vector>
#include <memory>

namespace CHTL {

class CHTLParser {
public:
    explicit CHTLParser(std::vector<Token> tokens, std::shared_ptr<CHTLContext> context);

    // The main entry point for parsing. Returns the root of the AST.
    std::unique_ptr<ElementNode> parse();

private:
    // Recursive-descent parsing methods for different grammar rules.
    std::unique_ptr<BaseNode> parseNode();
    std::unique_ptr<ElementNode> parseElement();
    void parseStyleBlock(ElementNode& element);
    Attribute parseAttribute();
    std::unique_ptr<TextNode> parseTextNode();
    std::unique_ptr<TextNode> parseTextAttributeAsNode();
    void parseTemplateDefinition();
    void parseTemplateUsage(ElementNode& element, std::vector<StyleProperty>& styleList);
    void parseDeletion(std::vector<StyleProperty>& styleList);
    void parseImportStatement();


    // Expression parsing methods
    std::unique_ptr<ExpressionNode> parseExpression(int precedence = 0);
    std::unique_ptr<ExpressionNode> parsePrefix();
    std::unique_ptr<ExpressionNode> parseInfix(std::unique_ptr<ExpressionNode> left);

    // Helper methods for token stream manipulation.
    bool peekContainsOperator() const;
    const Token& currentToken() const;
    const Token& peekToken() const;
    void advance();
    bool match(TokenType type);
    const Token& consume(TokenType type, const std::string& errorMessage);

    std::vector<Token> m_tokens;
    size_t m_position = 0;
    Token m_eofToken;
    std::shared_ptr<CHTLContext> m_context;
};

} // namespace CHTL
