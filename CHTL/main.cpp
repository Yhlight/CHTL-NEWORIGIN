#include <iostream>
#include <vector>
#include <memory>
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/BaseNode.h"

int main() {
    std::string chtl_source = R"(
        // This is a demonstration of the CHTL parser.
        html {
            head {
                // The head of the document.
            }
            body {
                div {
                    text { "Hello, World!" }
                }
                span {
                    // An empty span.
                }
            }
        }
    )";

    using namespace CHTL;

    CHTLLexer lexer;
    std::vector<Token> tokens = lexer.tokenize(chtl_source);

    std::cout << "--- Tokens ---" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "Type: " << static_cast<int>(token.type) << ", Value: '" << token.value << "'" << std::endl;
    }
    std::cout << "--------------" << std::endl;

    CHTLParser parser(tokens);
    std::unique_ptr<BaseNode> ast = parser.parse();

    if (ast) {
        std::cout << "\n--- AST ---" << std::endl;
        ast->print();
        std::cout << "-------------" << std::endl;
    } else {
        std::cout << "Failed to parse CHTL source." << std::endl;
    }

    return 0;
}