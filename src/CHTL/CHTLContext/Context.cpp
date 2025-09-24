#include "Context.h"
#include "CHTLState/ParseBaseState.h"
#include <stdexcept>

namespace CHTL {

CHTLContext::CHTLContext(const std::string& source)
    : _lexer(source) {
    setState(std::make_unique<ParseBaseState>());
}

void CHTLContext::setState(std::unique_ptr<State> newState) {
    _currentState = std::move(newState);
}

void CHTLContext::request() {
    if (_currentState) {
        _currentState->handle(*this);
    }
}

const Token& CHTLContext::peekToken(size_t offset) const {
    while (_currentTokenIndex + offset >= _tokens.size()) {
        Token token = const_cast<CHTLLexer&>(_lexer).getNextToken();
        const_cast<std::vector<Token>&>(_tokens).push_back(token);
        if (token.type == TokenType::END_OF_FILE) break;
    }
    return _tokens[_currentTokenIndex + offset];
}

Token CHTLContext::consumeToken() {
    peekToken(0);
    if (_tokens[_currentTokenIndex].type == TokenType::END_OF_FILE) {
        return _tokens[_currentTokenIndex];
    }
    return _tokens[_currentTokenIndex++];
}

void CHTLContext::createRootNode(const std::string& tagName) {
    auto root = std::make_unique<ElementNode>(tagName);
    _nodeStack.push_back(root.get());
    _ast = std::move(root);
}

ElementNode* CHTLContext::getCurrentNode() const {
    if (_nodeStack.empty()) {
        return nullptr;
    }
    return _nodeStack.back();
}

void CHTLContext::pushNode(ElementNode* node) {
    _nodeStack.push_back(node);
}

void CHTLContext::popNode() {
    if (!_nodeStack.empty()) {
        _nodeStack.pop_back();
    }
}

std::unique_ptr<BaseNode> CHTLContext::getAST() {
    return std::move(_ast);
}

} // namespace CHTL
