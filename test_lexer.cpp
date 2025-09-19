#include <iostream>
#include <string>
#include <vector>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLLexer/Token.h"

using namespace CHTL;

int main() {
    std::string input = "html\n{\n    head\n    {\n        title\n        {\n            \"Simple Test\"\n        }\n    }\n    \n    body\n    {\n        div\n        {\n            text\n            {\n                \"Hello World\"\n            }\n        }\n    }\n}";
    
    CHTLLexer lexer;
    std::vector<Token> tokens = lexer.tokenize(input);
    
    std::cout << "Tokens generated: " << tokens.size() << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "Token " << i << ": " << static_cast<int>(tokens[i].type) << " '" << tokens[i].value << "'" << std::endl;
    }
    
    return 0;
}