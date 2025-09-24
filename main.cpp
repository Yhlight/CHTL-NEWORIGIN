#include <iostream>
#include <vector>
#include <memory>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"


// A very simple function to print the AST structure for verification
void PrintAst(const CHTL::AST::BaseNode* node, int indent = 0)
{
    if (!node) return;

    for (int i = 0; i < indent; ++i) std::cout << "  ";

    if (auto element = dynamic_cast<const CHTL::AST::ElementNode*>(node))
    {
        std::cout << "Element: <" << element->tagName << ">" << std::endl;
        for (const auto& attr : element->attributes)
        {
            for (int i = 0; i < indent + 1; ++i) std::cout << "  ";
            std::cout << "Attribute: " << attr->name << " = \"" << attr->value << "\"" << std::endl;
        }
        for (const auto& child : element->children)
        {
            PrintAst(child.get(), indent + 1);
        }
    }
    else if (auto text = dynamic_cast<const CHTL::AST::TextNode*>(node))
    {
        std::cout << "Text: \"" << text->content << "\"" << std::endl;
    }
}


int main(int argc, char* argv[]) {
    std::string source = R"(
        div {
            style {
                width: 100px * 2 + 50px; // Should be 250.000000px
                height: 1000px / 10; // Should be 100.000000px
                border: 1px solid black;
            }

            p {
                text: "This box should have calculated dimensions.";
            }
        }
    )";

    std::cout << "--- CHTL Compiler Pipeline Test ---" << std::endl;

    // 1. Lexing
    std::cout << "\n1. Lexing source code..." << std::endl;
    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.Tokenize();
    std::cout << "Lexing complete." << std::endl;

    // 2. Parsing
    std::cout << "\n2. Parsing token stream..." << std::endl;
    CHTL::Parser parser(tokens);
    std::vector<std::unique_ptr<CHTL::AST::BaseNode>> astRoots;
    try {
        astRoots = parser.Parse();
        std::cout << "Parsing complete. " << astRoots.size() << " root node(s) created." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
        return 1;
    }

    if (astRoots.empty()) {
        std::cerr << "Parsing resulted in an empty AST." << std::endl;
        return 1;
    }

    // 3. Generating
    std::cout << "\n3. Generating HTML from AST..." << std::endl;
    std::string finalHtml;
    for (const auto& root : astRoots) {
        CHTL::Generator generator;
        finalHtml += generator.Generate(*root);
    }
    std::cout << "Generation complete." << std::endl;


    // 4. Final Output
    std::cout << "\n--- Generated HTML ---" << std::endl;
    std::cout << finalHtml << std::endl;
    std::cout << "----------------------" << std::endl;

    return 0;
}
