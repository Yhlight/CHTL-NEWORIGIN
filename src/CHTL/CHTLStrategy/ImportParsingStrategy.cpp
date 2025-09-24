#include "ImportParsingStrategy.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ImportNode.h" // Include the new node
#include <stdexcept>

std::unique_ptr<BaseNode> ImportParsingStrategy::parse(Parser& parser) {
    parser.expectToken(TokenType::OpenBracket);
    parser.expectToken(TokenType::Import);

    // This is where we'd parse the optional [Custom], [Template] part
    // For now, we'll skip this and assume a simple @Type syntax for all imports.
    if (parser.currentToken.type == TokenType::CloseBracket) {
         parser.advanceTokens(); // Consume ']'
    } else {
        // In a full implementation, parse [Custom] etc. here
        // For now, just consume the tokens to advance the parser.
        parser.advanceTokens(); // e.g., Custom
        parser.advanceTokens(); // e.g., ]
    }

    parser.expectToken(TokenType::At);
    std::string importType = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    // For precise CHTL imports, the name of the item to import comes next.
    std::string itemName;
    if (parser.currentToken.type == TokenType::Identifier && parser.peekToken.type == TokenType::From) {
        itemName = parser.currentToken.value;
        parser.advanceTokens();
    }

    parser.expectKeyword(TokenType::From, "KEYWORD_FROM", "from");

    if (parser.currentToken.type != TokenType::String) {
        throw std::runtime_error("Import path must be a string literal.");
    }
    std::string path = parser.currentToken.value;
    parser.advanceTokens();

    std::string alias;
    if (parser.tryExpectKeyword(TokenType::Identifier, "KEYWORD_AS", "as")) {
        alias = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
    }

    parser.expectToken(TokenType::Semicolon);

    // Instead of processing the import here, we create a node for the AST.
    return std::make_unique<ImportNode>(importType, itemName, path, alias);
}
