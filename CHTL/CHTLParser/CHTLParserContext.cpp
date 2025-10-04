#include "CHTLParserContext.h"
#include "../CHTLState/InitialState.h"
#include "../CHTLContext/ConfigurationManager.h"

namespace CHTL {

CHTLParserContext::CHTLParserContext(const std::vector<Token>& tokens, std::shared_ptr<ConfigurationManager> configManager)
    : tokens(tokens), position(0), configManager(configManager) {
    setState(std::make_unique<InitialState>());
}

std::shared_ptr<BaseNode> CHTLParserContext::parse() {
    auto root = std::make_shared<ElementNode>("root");
    while (!isAtEnd()) {
        if (!currentStrategy) {
            currentState->handleToken(this, getCurrentToken());
        }

        if (currentStrategy) {
            auto node = runCurrentStrategy();
            if (node) {
                root->addChild(node);
            }
        } else {
            if (!isAtEnd()) {
                advance();
            }
        }
    }
    return root;
}

std::shared_ptr<BaseNode> CHTLParserContext::runCurrentStrategy() {
    if (currentStrategy) {
        auto strategy = std::move(currentStrategy);
        return strategy->parse(this);
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