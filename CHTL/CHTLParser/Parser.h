#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "ParseContext.h"

#include <vector>
#include <memory>
#include <stdexcept>

class CHTLParser {
public:
    explicit CHTLParser(std::vector<Token> tokens);
    std::vector<std::shared_ptr<BaseNode>> parse();

private:
    std::shared_ptr<BaseNode> parseStatement();
    std::shared_ptr<BaseNode> parseElement();
    std::shared_ptr<BaseNode> parseTextElement();
    void parseAttribute(std::shared_ptr<ElementNode> element);
    void parseStyleBlock(std::shared_ptr<ElementNode> element);
    void parseTemplateDefinition();
    std::string parseValueUntilSemicolon();

    // Helper methods
    bool isAtEnd();
    Token& peek();
    Token& previous();
    Token& advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token& consume(TokenType type, const std::string& message);
    void synchronize(); // For error recovery

    std::vector<Token> tokens;
    int current = 0;
    ParseContext parseContext;

    enum class ParserState {
        GLOBAL,
        ELEMENT_BODY,
        STYLE_BLOCK
    };
    std::vector<ParserState> states;
    ParserState currentState();
};

// Custom exception for parsing errors
class ParseError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
