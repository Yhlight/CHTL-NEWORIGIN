#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/StyleTemplateUsageNode.h"
#include "../CHTLNode/ElementTemplateUsageNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/DeletePropertyNode.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<RootNode> parse();

    // Static method to parse snippets of CHTL, like template bodies
    static StyleStatementList parseStyleSnippet(const std::vector<Token>& tokens);

private:
    std::vector<Token> tokens;
    size_t position;

    Token currentToken();
    Token peekToken();
    void advance();
    bool match(TokenType type);
    void expect(TokenType type, const std::string& errorMessage);

    NodePtr parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    NodePtr parseText();
    void parseStyleBlock(ElementNode* element);
    NodePtr parseTemplateDefinition();
    std::unique_ptr<StylePropertyNode> parseStyleProperty();
    std::unique_ptr<DeletePropertyNode> parseDeleteStatement();
};
