#include "CHTLParserContext.h"
#include "../CHTLState/InitialState.h"

namespace CHTL {

CHTLParserContext::CHTLParserContext(const std::vector<Token>& tokens)
    : tokens(tokens), position(0) {
    // Set the initial state
    setState(std::make_unique<InitialState>());
}

std::shared_ptr<BaseNode> CHTLParserContext::parse() {
    auto root = std::make_shared<ElementNode>("root");
    while (!isAtEnd()) {
        currentState->handleToken(this, getCurrentToken());
        auto node = runCurrentStrategy();
        if (node) {
            root->addChild(node);
        } else {
            // If no strategy is set, advance to avoid an infinite loop
            // This might happen with comments or whitespace that is not fully skipped
             if (!isAtEnd()) {
                advance();
            }
        }
    }
    return root;
}

std::shared_ptr<BaseNode> CHTLParserContext::runCurrentStrategy() {
    if (currentStrategy) {
        return currentStrategy->parse(this);
    }
    return nullptr;
}


void CHTLParserContext::setState(std::unique_ptr<ParserState> newState) {
    currentState = std::move(newState);
}

void CHTLParserContext::setStrategy(std::unique_ptr<ParsingStrategy> newStrategy) {
    currentStrategy = std::move(newStrategy);
}

Token& CHTLParserContext::getCurrentToken() {
    return tokens[position];
}

Token& CHTLParserContext::peek(size_t offset) {
    if (position + offset >= tokens.size()) {
        // This is not ideal, but it will prevent a crash.
        // A more robust solution would be to have a dedicated EOF token object.
        static Token eofToken = {TokenType::TOKEN_EOF, "", 0, 0};
        return eofToken;
    }
    return tokens[position + offset];
}

void CHTLParserContext::advance() {
    if (!isAtEnd()) {
        position++;
    }
}

bool CHTLParserContext::isAtEnd() {
    return position >= tokens.size();
}

}