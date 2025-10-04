#include <iostream>
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLLoader/CHTLLoader.h"

int main() {
    std::string input = R"(
[Import] @Chtl from "test.chtl";

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

    CHTL::CHTLLoader loader("."); // Assuming current directory as base path
    loader.loadImports(ast);

    CHTL::CHTLGenerator generator;
    generator.generate(ast);
    for (const auto& pair : loader.getLoadedAsts()) {
        generator.generate(pair.second);
    }

    std::string html = generator.getHtml();
    std::string css = generator.getCss();

    std::cout << "Generated HTML:" << std::endl;
    std::cout << html << std::endl;
    std::cout << "\nGenerated CSS:" << std::endl;
    std::cout << css << std::endl;

    return 0;
}