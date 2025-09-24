#include "Parser.hpp"

namespace chtl::core {

Parser::Parser(const std::vector<Token>& tokens) : toks(tokens) {}

AstNode Parser::parse() {
    return AstNode{ "Root" };
}

}
