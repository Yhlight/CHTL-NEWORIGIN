#include "CHTLParser.h"
#include "../CHTLState/DefaultState.h"
#include "../CHTLNode/RootNode.h"
#include <string>

CHTLParser::CHTLParser(const std::string& input) : lexer(input) {
    root = std::make_unique<RootNode>();
    nodeStack.push_back(root.get());
    currentState = std::make_unique<DefaultState>();
}

CHTLParser::~CHTLParser() {}

void CHTLParser::parse() {
    Token token = consume();
    while (token.type != TokenType::END_OF_FILE) {
        currentState->handle(*this, token);
        token = consume();
    }
}

void CHTLParser::setState(std::unique_ptr<CHTLState> newState) {
    currentState = std::move(newState);
}

Token CHTLParser::consume() {
    if (!tokenBuffer.empty()) {
        Token token = tokenBuffer.front();
        tokenBuffer.erase(tokenBuffer.begin());
        return token;
    }
    return lexer.getNextToken();
}

Token CHTLParser::peek() {
    if (tokenBuffer.empty()) {
        tokenBuffer.push_back(lexer.getNextToken());
    }
    return tokenBuffer.front();
}

void CHTLParser::putback(Token token) {
    tokenBuffer.insert(tokenBuffer.begin(), token);
}

void CHTLParser::setRawContentMode(bool enabled) {
    lexer.setRawContentMode(enabled);
}

void CHTLParser::addNode(std::unique_ptr<BaseNode> node) {
    getCurrentScope()->addChild(std::move(node));
}

void CHTLParser::openScope(BaseNode* node) {
    nodeStack.push_back(node);
}

void CHTLParser::closeScope() {
    if (nodeStack.size() > 1) {
        nodeStack.pop_back();
    }
}

BaseNode* CHTLParser::getCurrentScope() {
    return nodeStack.back();
}

BaseNode* CHTLParser::getParentScope() {
    if (nodeStack.size() > 1) {
        return nodeStack[nodeStack.size() - 2];
    }
    return nullptr;
}

BaseNode* CHTLParser::getRoot() {
    return root.get();
}

std::unique_ptr<BaseNode> CHTLParser::releaseRoot() {
    return std::move(root);
}

GlobalStylesheet& CHTLParser::getGlobalStylesheet() {
    return stylesheet;
}