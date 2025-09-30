#include "CHTLParser.h"
#include "../CHTLNode/TextNode.h"

namespace CHTL {

CHTLParser::CHTLParser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

CHTLParser::~CHTLParser() {}

std::unique_ptr<BaseNode> CHTLParser::parse() {
    // This is a very basic, single-statement parser for now.
    // It only looks for a text block.
    if (tokens.size() >= 4 &&
        tokens[0].type == TokenType::TEXT_KEYWORD &&
        tokens[1].type == TokenType::L_BRACE &&
        tokens[2].type == TokenType::STRING_LITERAL &&
        tokens[3].type == TokenType::R_BRACE) {

        // Create a new TextNode with the value from the string literal token.
        return std::make_unique<TextNode>(tokens[2].value);
    }

    // If the pattern doesn't match, return a null pointer.
    return nullptr;
}

} // namespace CHTL