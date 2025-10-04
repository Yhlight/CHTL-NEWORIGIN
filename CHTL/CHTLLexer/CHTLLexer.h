#pragma once

#include "Token.h"
#include "../CHTLContext/ConfigurationManager.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

    class CHTLLexer {
    public:
        CHTLLexer(const std::string& source, std::shared_ptr<ConfigurationManager> configManager);
        Token getNextToken();

    private:
        std::string source;
        std::shared_ptr<ConfigurationManager> configManager;
        size_t position;
        int line;
        int column;

        char peek();
        char advance();
        void skipWhitespace();
        Token makeToken(TokenType type, const std::string& lexeme);
        Token lexIdentifierOrLiteral();
        Token stringLiteral(char quoteType);
        Token singleLineComment();
        Token multiLineComment();
        Token generatorComment();
        Token errorToken(const std::string& message);
    };

}