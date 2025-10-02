#include "Parser.h"
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLNode/ElementNode.h"
namespace CHTL {
Parser::Parser(const std::vector<Token>& tokens, CHTLContext& context)
    : tokens_(tokens), context_(context), current_(0) {}

std::shared_ptr<BaseNode> Parser::parse() {
    // TODO: Implement full parser
    auto root = std::make_shared<ElementNode>("root");
    return root;
}

Token Parser::peek() const {
    return tokens_[current_];
}

Token Parser::advance() {
    return tokens_[current_++];
}

bool Parser::isAtEnd() const {
    return current_ >= tokens_.size() || peek().type == TokenType::EOF_TOKEN;
}
}
