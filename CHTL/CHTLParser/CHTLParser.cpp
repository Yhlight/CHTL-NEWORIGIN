#include "CHTLParser.h"
#include "../CHTLState/ParsingState.h"
#include "../CHTLState/InitialState.h"
#include "../CHTLNode/BinaryOpNode.h"
#include "../CHTLNode/LiteralNode.h"
#include <stdexcept>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens(tokens) {
    // Create a virtual root node to hold all top-level elements.
    root = std::make_unique<ElementNode>("<root>");
    nodeStack.push(root.get());
    // Set the initial state of the parser.
    setState(std::make_unique<InitialState>());
}

std::unique_ptr<BaseNode> CHTLParser::parse() {
    while (state && !isAtEnd()) {
        // Centrally handle comments. They can appear anywhere and should be ignored.
        if (peek().type == TokenType::COMMENT) {
            advance();
            continue;
        }
        state->handle(*this);
    }
    // The virtual root node contains the parsed document structure
    return std::move(root);
}

void CHTLParser::setState(std::unique_ptr<ParsingState> newState) {
    state = std::move(newState);
}

Token CHTLParser::peek() const {
    if (isAtEnd()) return {TokenType::EndOfFile, ""};
    return tokens[current];
}

Token CHTLParser::peekNext() const {
    if (current + 1 >= tokens.size()) {
        return {TokenType::EndOfFile, ""};
    }
    return tokens[current + 1];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return tokens[current - 1];
}

bool CHTLParser::isAtEnd() const {
    return current >= tokens.size() || tokens[current].type == TokenType::EndOfFile;
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

bool CHTLParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error(message + " - Found " + peek().value + " instead.");
}

void CHTLParser::openNode(std::unique_ptr<ElementNode> node) {
    ElementNode* node_ptr = node.get();
    if (nodeStack.empty()) {
         root = std::move(node);
         nodeStack.push(node_ptr);
         return;
    }
    nodeStack.top()->addChild(std::move(node));
    nodeStack.push(node_ptr);
}

void CHTLParser::closeNode() {
    if (nodeStack.size() > 1) { // Do not pop the virtual root
        nodeStack.pop();
    }
}

void CHTLParser::addNode(std::unique_ptr<BaseNode> node) {
    if (nodeStack.empty()) {
        throw std::runtime_error("Cannot add a node when the node stack is empty.");
    }
    nodeStack.top()->addChild(std::move(node));
}

ElementNode* CHTLParser::getCurrentNode() {
    if (nodeStack.empty()) {
        return nullptr;
    }
    return nodeStack.top();
}

CHTLParser::~CHTLParser() = default;

// --- Expression Parsing (Pratt/Precedence Climbing) ---

std::unique_ptr<ExpressionNode> CHTLParser::parseExpression() {
    return parseTerm();
}

std::unique_ptr<ExpressionNode> CHTLParser::parseTerm() {
    auto expr = parseFactor();
    while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS) {
        Token op = advance();
        auto right = parseFactor();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseFactor() {
    auto expr = parsePower();
    while (peek().type == TokenType::STAR || peek().type == TokenType::SLASH) {
        Token op = advance();
        auto right = parsePower();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parsePower() {
    auto expr = parsePrimary();
    if (peek().type == TokenType::STAR_STAR) {
        Token op = advance();
        auto right = parsePower(); // Right-associative
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parsePrimary() {
    if (match(TokenType::NUMBER) || match(TokenType::STRING_LITERAL) || match(TokenType::IDENTIFIER)) {
        return std::make_unique<LiteralNode>(tokens[current - 1]);
    }

    if (match(TokenType::L_BRACE)) {
        auto expr = parseExpression();
        consume(TokenType::R_BRACE, "Expected '}' after expression.");
        return expr;
    }

    throw std::runtime_error("Expected expression, but found " + peek().value);
}

} // namespace CHTL