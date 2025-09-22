#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <string>

#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLNode/BaseNode.h"

// Helper to convert TokenType to string for printing
std::string tokenTypeToString(CHTL::TokenType type) {
    switch (type) {
        case CHTL::TokenType::IDENTIFIER: return "IDENTIFIER";
        case CHTL::TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case CHTL::TokenType::NUMERIC_LITERAL: return "NUMERIC_LITERAL";
        case CHTL::TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case CHTL::TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case CHTL::TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case CHTL::TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case CHTL::TokenType::COLON: return "COLON";
        case CHTL::TokenType::SEMICOLON: return "SEMICOLON";
        case CHTL::TokenType::ASSIGN: return "ASSIGN";
        case CHTL::TokenType::COMMA: return "COMMA";
        case CHTL::TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case CHTL::TokenType::UNKNOWN: return "UNKNOWN";
        case CHTL::TokenType::END_OF_FILE: return "END_OF_FILE";
        default: return "UNHANDLED_TOKEN_TYPE";
    }
}

void printToken(const CHTL::Token& token) {
    std::cout << "Token(Type: " << tokenTypeToString(token.type)
              << ", Value: '" << token.value << "'"
              << ", Line: " << token.line
              << ", Col: " << token.column << ")" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_chtl_file>" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::cout << "--- Lexer Token Stream ---" << std::endl;
    CHTL::CHTLLexer lexerForDebug(source);
    CHTL::Token token;
    do {
        token = lexerForDebug.getNextToken();
        printToken(token);
    } while (token.type != CHTL::TokenType::END_OF_FILE);
    std::cout << "--------------------------" << std::endl;

    std::cout << "--- Parsing File: " << filepath << " ---" << std::endl;
    try {
        CHTL::CHTLLexer lexer(source);
        CHTL::CHTLParser parser(lexer);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();

        std::cout << "--- Abstract Syntax Tree ---" << std::endl;
        if (ast) {
            ast->print();
        } else {
            std::cout << "AST is null." << std::endl;
        }
        std::cout << "----------------------------" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "An error occurred during parsing: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
