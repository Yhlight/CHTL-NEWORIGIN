#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLContext/CHTLContext.h"
#include "CHTL/CHTLResolver/CHTLResolver.h"
#include "CHTL/CHTLEvaluator/StyleEvaluator.h"

int main() {
    std::string filename = "Test/test.chtl";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    if (source.empty()) {
        std::cout << "File is empty, nothing to compile." << std::endl;
        return 0;
    }

    try {
        // 1. Context and Lexer
        CHTLContext context;
        CHTLLexer lexer(source);

        // 2. Parser
        CHTLParser parser(lexer, context);
        std::shared_ptr<ElementNode> initial_ast = parser.parse();

        // 3. Resolver
        CHTLResolver resolver(context);
        std::shared_ptr<ElementNode> resolved_ast = resolver.resolve(initial_ast);

        // 4. Evaluator
        StyleEvaluator evaluator;
        evaluator.evaluate(resolved_ast);

        // 5. Generator
        CHTLGenerator generator(resolved_ast);
        std::string html_output = generator.generate();

        // 6. Output
        std::cout << "--- CHTL Compiler Output ---\n" << std::endl;
        std::cout << html_output << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
