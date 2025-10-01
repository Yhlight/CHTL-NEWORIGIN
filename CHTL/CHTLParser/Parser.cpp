#include "Parser.h"

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::unique_ptr<BaseNode> Parser::parse() {
    // For now, just return a null pointer.
    return nullptr;
}