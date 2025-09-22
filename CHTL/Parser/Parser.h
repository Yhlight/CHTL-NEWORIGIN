#pragma once

#include "../Lexer/Lexer.h"
#include "../Node/BaseNode.h"
#include "../Node/ElementNode.h"
#include "../Node/TextNode.h"
#include "../Node/StyleNode.h"
#include "../Node/TemplateUsageNode.h"
#include "../Common/TemplateStore.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace CHTL {

class Parser {
public:
    Parser(Lexer& lexer, TemplateStore& store);
    std::unique_ptr<ElementNode> parse();

private:
    Lexer& lexer;
    TemplateStore& store;
    Token currentToken;
    Token peekToken;

    void nextToken();
    bool currentTokenIs(TokenType type);
    bool peekTokenIs(TokenType type);
    void expectPeek(TokenType type);

    void parseTemplateDefinition();
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElementNode();
    std::unique_ptr<TextNode> parseTextNode();
    std::unique_ptr<StyleNode> parseStyleNode();
    StyleRule parseStyleRule();
    std::unique_ptr<TemplateUsageNode> parseTemplateUsage();
};

} // namespace CHTL
