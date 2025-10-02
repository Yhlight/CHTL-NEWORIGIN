#include "CHTLParser.h"
#include "../CHTLState/DefaultState.h"
#include <string>

CHTLParser::CHTLParser(const std::string& input) : lexer(input) {
    root = std::make_unique<BaseNode>();
    nodeStack.push_back(root.get());
    currentState = std::make_unique<DefaultState>();
}

CHTLParser::~CHTLParser() {}

void CHTLParser::parse() {
    Token token = getNextToken();
    while (token.type != TokenType::END_OF_FILE) {
        currentState->handle(*this, token);
        token = getNextToken();
    }
}

void CHTLParser::setState(std::unique_ptr<CHTLState> newState) {
    currentState = std::move(newState);
}

Token CHTLParser::getNextToken() {
    return lexer.getNextToken();
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

BaseNode* CHTLParser::getRoot() {
    return root.get();
}