#include "../../CHTL/CHTLLexer/Lexer.h"
#include <iostream>
#include <map>

// Helper to print token types
std::map<TokenType, std::string> tokenTypeMap = {
    {TokenType::LEFT_BRACE, "LEFT_BRACE"},
    {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::COLON, "COLON"},
    {TokenType::EQUALS, "EQUALS"},
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::STRING_LITERAL, "STRING_LITERAL"},
    {TokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
    {TokenType::SINGLE_LINE_COMMENT, "SINGLE_LINE_COMMENT"},
    {TokenType::MULTI_LINE_COMMENT, "MULTI_LINE_COMMENT"},
    {TokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
    {TokenType::END_OF_FILE, "END_OF_FILE"},
    {TokenType::UNKNOWN, "UNKNOWN"}
};

void printTokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << "Type: " << tokenTypeMap[token.type]
                  << ", Lexeme: '" << token.lexeme
                  << "', Line: " << token.line
                  << ", Col: " << token.column << std::endl;
    }
}

int main() {
    std::string source = R"(
        // This is a single line comment
        # This is a generator comment

        html {
            id: "main-html-tag";
            class = box; /* multi-line
                            comment */
        }
    )";

    CHTLLexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::cout << "--- Lexer Test ---" << std::endl;
    printTokens(tokens);
    std::cout << "--- Test End ---" << std::endl;

    // Basic assertion
    if (tokens.size() < 10) {
        std::cerr << "Test failed: Not enough tokens generated." << std::endl;
        return 1;
    }

    std::cout << "Lexer test seems to have produced a reasonable number of tokens." << std::endl;

    return 0;
}
