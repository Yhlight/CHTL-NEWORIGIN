#include "../../CHTL/CHTLLexer/Lexer.h"
#include <iostream>
#include <vector>

void print_token(const CHTL::Token& tok) {
    std::cout << "Token -> Type: " << CHTL::TokenTypeToString(tok.type)
              << ", Literal: \"" << tok.literal << "\""
              << ", Line: " << tok.line << std::endl;
}

int main() {
    std::string input = R"(
// This is a comment
# This is a generator comment

html {
    id: "main-page";
    class = 'container';

    body {
        text: "Hello, CHTL!";
    }
}
/*
  Multi-line
  comment
*/
    )";

    CHTL::Lexer lexer(input);

    std::cout << "--- Lexer Test ---" << std::endl;
    for (CHTL::Token tok = lexer.nextToken(); tok.type != CHTL::TokenType::END_OF_FILE; tok = lexer.nextToken()) {
        print_token(tok);
    }
    // Print the EOF token too
    print_token({CHTL::TokenType::END_OF_FILE, "", 0});
    std::cout << "--- Test End ---" << std::endl;

    return 0;
}
