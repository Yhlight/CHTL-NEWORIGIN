#pragma once

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLNode/Node.h"
#include "CHTL/CHTLContext/Context.h"
#include <vector>
#include <memory>

namespace CHTL {

class Parser {
public:
    Parser(const std::vector<Token>& tokens, Context& context);
    std::unique_ptr<ElementNode> parse();

private:
    std::vector<Token> m_tokens;
    Context& m_context;
    size_t m_current = 0;

    std::unique_ptr<Node> parseNode(ElementNode* parent);
    std::unique_ptr<ElementNode> parseElement(ElementNode* parent);
    std::unique_ptr<TextNode> parseText();
    void parseAttributes(ElementNode& element);
    std::shared_ptr<TemplateDeclarationNode> parseTemplateDeclaration();
    std::unique_ptr<TemplateInstantiationNode> parseTemplateInstantiation(ElementNode* parent);
    std::unique_ptr<StylePropertyNode> parseStyleProperty();

    Token& peek();
    Token& previous();
    Token& advance();
    bool isAtEnd();
    bool check(TokenType type);
    bool match(TokenType type);
    Token& consume(TokenType type, const std::string& message);
};

} // namespace CHTL
