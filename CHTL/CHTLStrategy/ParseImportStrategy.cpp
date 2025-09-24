#include "ParseImportStrategy.h"
#include "../CHTLContext/Context.h"
#include "../CHTLParser/Parser.h"
#include <iostream>

namespace CHTL {

void ParseImportStrategy::Execute(Context* context, Parser& parser) {
    // The state has already determined this is an import block.
    // We can proceed with consuming tokens.

    // Consume '['
    parser.ConsumeToken();

    // Consume 'Import'
    parser.ConsumeToken();

    // Expect ]
    if (parser.CurrentToken().type != TokenType::RIGHT_BRACKET) {
        std::cerr << "Error: Expected ']' after [Import." << std::endl;
        return;
    }
    parser.ConsumeToken();

    // Expect @Chtl
    if (parser.CurrentToken().type != TokenType::AT) return;
    parser.ConsumeToken();
    if (parser.CurrentToken().type != TokenType::IDENTIFIER || parser.CurrentToken().lexeme != "Chtl") {
        std::cerr << "Error: Only '@Chtl' imports are supported currently." << std::endl;
        return;
    }
    parser.ConsumeToken();

    // Expect from
    if (parser.CurrentToken().type != TokenType::IDENTIFIER || parser.CurrentToken().lexeme != "from") {
        std::cerr << "Error: Expected 'from' keyword in import statement." << std::endl;
        return;
    }
    parser.ConsumeToken();

    // Expect path (string literal)
    if (parser.CurrentToken().type != TokenType::STRING) {
        std::cerr << "Error: Expected file path in quotes for import." << std::endl;
        return;
    }
    std::string filepath = parser.CurrentToken().lexeme;
    parser.ConsumeToken();

    // Call the loader to handle the import
    parser.loader.ImportFile(filepath);
}

} // namespace CHTL
