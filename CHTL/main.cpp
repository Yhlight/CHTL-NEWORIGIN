#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/Node.h"

static void printParserErrors(const std::vector<std::string>& errors) {
    std::cerr << "Compilation failed with " << errors.size() << " parser errors:" << std::endl;
    for (const auto& msg : errors) {
        std::cerr << "  - " << msg << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename.chtl>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream inFile(filename);

    if (!inFile) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string sourceCode = buffer.str();

    // 1. Lex the source code
    Lexer l(sourceCode);

    // 2. Parse the token stream
    Parser p(l);
    NodePtr program = p.parseProgram();

    // 3. Check for parsing errors
    if (!p.errors().empty()) {
        printParserErrors(p.errors());
        return 1;
    }

    // 4. Generate HTML from the AST
    Generator g;
    std::string htmlOutput = g.generate(program);

    // 5. Print the result
    std::cout << htmlOutput << std::endl;

    return 0;
}
