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

    std::unique_ptr<ElementNode> parse();

private:
    // Main parsing methods
    std::unique_ptr<BaseNode> parseNode();
    std::unique_ptr<ElementNode> parseElement();
    void parseStyleBlock(ElementNode& element);
    void parseTemplateDefinition();
    void parseImportStatement();

    // Helper parsing methods
    Attribute parseAttribute();
    std::unique_ptr<TextNode> parseTextNode();
    std::unique_ptr<TextNode> parseTextAttributeAsNode();
    void parseTemplateUsage(ElementNode& element);
    void parseElementSpecialization(std::vector<std::unique_ptr<BaseNode>>& nodes);
    struct ElementSelector { std::string tagName; int index = -1; /* -1 means no index */ };
    ElementSelector parseElementSelector();


    // Expression parsing
    std::unique_ptr<ExpressionNode> parseExpression(int precedence = 0);
    std::unique_ptr<ExpressionNode> parsePrefix();
    std::unique_ptr<ExpressionNode> parseInfix(std::unique_ptr<ExpressionNode> left);
    bool peekContainsOperator() const;

    // Token manipulation
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
