#pragma once

#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLState/CHTLState.h"
#include "../CHTLContext/CHTLContext.h"
#include <memory>
#include <stdexcept>

class CHTLParser {
public:
    explicit CHTLParser(CHTLLexer& lexer, CHTLContext& context);
    std::shared_ptr<ElementNode> parse();

    // Methods for ExprParser to access the token stream
    void consumeToken();
    const Token& getCurrentToken() const;
    Token peekNextToken();


private:
    CHTLLexer& lexer;
    CHTLContext& context;
    Token currentToken;
    CHTLState state;

    // Token management
    void advanceToken();
    void expect(TokenType type, const std::string& message);

    // Parsing helpers
    void parseDirective();
    void parseTemplateDefinition();
    std::map<std::string, std::shared_ptr<BaseExprNode>> parseCssRuleProperties();
    std::shared_ptr<BaseNode> parseStatement();
    std::shared_ptr<ElementNode> parseElement();
    void parseBlock(const std::shared_ptr<ElementNode>& element);
    std::shared_ptr<AttributeNode> parseAttribute();
    std::shared_ptr<BaseNode> parseTemplateUsage(const std::shared_ptr<ElementNode>& element);
    void parseStyleBlock(const std::shared_ptr<ElementNode>& element);
    std::shared_ptr<TextNode> parseTextNode();
    std::shared_ptr<CommentNode> parseCommentNode();

    // Error handling
    void error(const std::string& message);
};
