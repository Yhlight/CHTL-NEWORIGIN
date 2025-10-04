#include <iostream>
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"

int main() {
    std::string input = R"(
div {
    id: "main";
    style {
        color: red;
    }
    text { "Hello, CHTL!" }
}
)";

    CHTL::CHTLLexer lexer(input);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }

    CHTL::CHTLParser parser(tokens);
    std::shared_ptr<CHTL::BaseNode> ast = parser.parse();

    CHTL::CHTLGenerator generator;
    generator.generate(ast);
    std::string html = generator.getHtml();

    std::cout << "Generated HTML:" << std::endl;
    std::cout << html << std::endl;

    return 0;
}