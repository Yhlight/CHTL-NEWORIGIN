#include <iostream>
#include <string>
#include <vector>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"

#include "CHTL/CHTLContext/CHTLContext.h"

int main() {
    std::string source = R"(
        [Template] @Element Box {
            div {
                class: "box";
                text { "This is from a template." }
            }
        }

    [Template] @Element Header {
        h1 {
            class: "title";
            text { "Welcome to the Site" }
        }
    }

    [Origin] @Html {
        <!-- Raw HTML at the top -->
    }

    body {
        @Element Header;

        div {
            id: "content";
            style {
                width: 100px + 50px;
                .red-text {
                    color: red;
                }
                &:hover {
                    border: 1px solid black;
                }
            }
            p {
                class: "red-text";
                text { "This text should be red." }
            }
        }
    }
    )";

    CHTL::CHTLContext context;

    std::cout << "--- Source CHTL ---" << std::endl;
    std::cout << source << std::endl;
    std::cout << "--------------------" << std::endl;

    try {
        CHTL::Lexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.tokenize();

        CHTL::Parser parser(tokens, source, context);
        std::vector<std::shared_ptr<CHTL::BaseNode>> ast = parser.parse();

        CHTL::Generator generator(ast, context);
        std::string html = generator.generate();

        std::cout << "--- Generated HTML ---" << std::endl;
        std::cout << html << std::endl;
        std::cout << "----------------------" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
