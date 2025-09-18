#include "../../CHTL/CHTLLexer/Lexer.h"
#include <iostream>
#include <map>

// Helper to print token types as strings
std::map<CHTL::TokenType, std::string> tokenTypeNames = {
    {CHTL::TokenType::TOKEN_UNKNOWN, "UNKNOWN"},
    {CHTL::TokenType::TOKEN_EOF, "EOF"},
    {CHTL::TokenType::TOKEN_LEFT_BRACE, "LEFT_BRACE"},
    {CHTL::TokenType::TOKEN_RIGHT_BRACE, "RIGHT_BRACE"},
    {CHTL::TokenType::TOKEN_COLON, "COLON"},
    {CHTL::TokenType::TOKEN_EQUAL, "EQUAL"},
    {CHTL::TokenType::TOKEN_SEMICOLON, "SEMICOLON"},
    {CHTL::TokenType::TOKEN_IDENTIFIER, "IDENTIFIER"},
    {CHTL::TokenType::TOKEN_QUOTED_STRING, "QUOTED_STRING"},
    {CHTL::TokenType::TOKEN_UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
    {CHTL::TokenType::TOKEN_COMMENT, "COMMENT"},
    {CHTL::TokenType::TOKEN_GENERATOR_COMMENT, "GENERATOR_COMMENT"},
};

void printTokens(const std::vector<CHTL::Token> &tokens)
{
    for (const auto &token : tokens)
    {
        std::cout << "Type: " << tokenTypeNames[token.type]
                  << "\tLexeme: '" << token.lexeme
                  << "'\tLine: " << token.line << std::endl;
    }
}

int main()
{
    std::string source = R"(
// CHTL example
html {
    id = "main-page"; // using equals
    class: "container"; // using colon

    # This is a generator comment
    text {
        "Hello CHTL!"
    }

    /*
        Multi-line comment
    */
    div {
        style {
            color: red;
            font-size: 16px;
        }
    }
}
)";

    std::cout << "--- Tokenizing CHTL Source ---" << std::endl;
    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.tokenize();
    std::cout << "--- Tokens ---" << std::endl;
    printTokens(tokens);
    std::cout << "--- End of Tokens ---" << std::endl;

    return 0;
}
