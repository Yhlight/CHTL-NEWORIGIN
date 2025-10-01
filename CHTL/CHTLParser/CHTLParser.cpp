#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/SelectorBlockNode.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/BinaryOpNode.h"
#include "../CHTLNode/ReferenceNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLManager/TemplateManager.h"
#include "../CHTLState/GlobalState.h"
#include <stdexcept>
#include <vector>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens(tokens) {
    // Start in the global state
    currentState = std::make_unique<GlobalState>();
}

void CHTLParser::setState(std::unique_ptr<State> newState) {
    currentState = std::move(newState);
}

// --- Context Stack Management ---

void CHTLParser::pushNode(BaseNode* node) {
    nodeStack.push(node);
}

void CHTLParser::popNode() {
    if (!nodeStack.empty()) {
        nodeStack.pop();
    }
}

BaseNode* CHTLParser::currentNode() const {
    if (nodeStack.empty()) {
        return nullptr;
    }
    return nodeStack.top();
}

void CHTLParser::addNodeToCurrent(std::unique_ptr<BaseNode> node) {
    if (!node) return;

    if (nodeStack.empty()) {
        ast.push_back(std::move(node));
    } else {
        BaseNode* parent = currentNode();
        if (auto* element_parent = dynamic_cast<ElementNode*>(parent)) {
            element_parent->addChild(std::move(node));
        } else if (auto* template_parent = dynamic_cast<TemplateDefinitionNode*>(parent)) {
            template_parent->addChild(std::move(node));
        }
    }
}

// --- Core Parsing Logic ---

void CHTLParser::parse() {
    while (!isAtEnd()) {
        if (currentState) {
            currentState->handle(*this);
        } else {
            advance(); // Safeguard
        }
    }
}

std::vector<std::unique_ptr<BaseNode>> CHTLParser::getAST() {
    std::vector<std::unique_ptr<BaseNode>> main_nodes;
    for (auto& node : ast) {
        if (auto* def_node = dynamic_cast<TemplateDefinitionNode*>(node.get())) {
            TemplateManager::getInstance().registerTemplate(
                std::unique_ptr<TemplateDefinitionNode>(static_cast<TemplateDefinitionNode*>(node.release()))
            );
        } else {
            if (node) {
                main_nodes.push_back(std::move(node));
            }
        }
    }
    ast.clear();
    return main_nodes;
}


// --- Public Token Manipulation Helpers ---

bool CHTLParser::isAtEnd() const {
    return current >= tokens.size() || peek().type == TokenType::EndOfFile;
}

Token CHTLParser::peek() const {
    if (current >= tokens.size()) return {TokenType::EndOfFile, ""};
    return tokens[current];
}

Token CHTLParser::peekNext() const {
    if (current + 1 >= tokens.size()) return {TokenType::EndOfFile, ""};
    return tokens[current + 1];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return tokens[current - 1];
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) return false;
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
    throw std::runtime_error(message);
}

Token CHTLParser::previous() const {
    return tokens[current - 1];
}



// --- Expression Parsing (Precedence Climbing) ---

std::unique_ptr<ExpressionNode> CHTLParser::parseExpression() {
    return parseTernary();
}

std::unique_ptr<ExpressionNode> CHTLParser::parseTernary() {
    auto expr = parseLogicalOr();
    if (match(TokenType::QUESTION_MARK)) {
        auto true_expr = parseExpression();
        consume(TokenType::COLON, "Expect ':' after true expression in ternary operator.");
        auto false_expr = parseExpression();
        return std::make_unique<ConditionalExpressionNode>(std::move(expr), std::move(true_expr), std::move(false_expr));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    while (match(TokenType::PIPE_PIPE)) {
        Token op = tokens[current - 1];
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseLogicalAnd() {
    auto expr = parseEquality();
    while (match(TokenType::AMPERSAND_AMPERSAND)) {
        Token op = tokens[current - 1];
        auto right = parseEquality();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseEquality() {
    auto expr = parseComparison();
    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL)) {
        Token op = tokens[current - 1];
        auto right = parseComparison();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseComparison() {
    auto expr = parseTerm();
    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) || match(TokenType::LESS) || match(TokenType::LESS_EQUAL)) {
        Token op = tokens[current - 1];
        auto right = parseTerm();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseTerm() {
    auto expr = parseFactor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = tokens[current - 1];
        auto right = parseFactor();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseFactor() {
    auto expr = parsePower();
    while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT)) {
        Token op = tokens[current - 1];
        auto right = parsePower();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parsePower() {
    auto expr = parsePrimary();
    if (match(TokenType::STAR_STAR)) {
        Token op = tokens[current - 1];
        auto right = parsePower();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        Token num_token = tokens[current - 1];
        std::string unit;
        if (check(TokenType::IDENTIFIER)) {
            unit = advance().value;
        } else if (check(TokenType::PERCENT)) {
            size_t next_token_idx = current + 1;
            if (next_token_idx >= tokens.size() ||
                (tokens[next_token_idx].type != TokenType::NUMBER &&
                 tokens[next_token_idx].type != TokenType::IDENTIFIER &&
                 tokens[next_token_idx].type != TokenType::L_BRACE)) {
                unit = advance().value;
            }
        }
        return std::make_unique<LiteralNode>(num_token, unit);
    }

    if (check(TokenType::DOT) || check(TokenType::HASH)) {
        Token prefix = advance();
        Token name = consume(TokenType::IDENTIFIER, "Expect selector name.");
        consume(TokenType::DOT, "Expect '.' after selector.");
        Token prop = consume(TokenType::IDENTIFIER, "Expect property name.");
        return std::make_unique<ReferenceNode>(prefix.value + name.value, prop.value);
    }

    if (check(TokenType::IDENTIFIER)) {
        if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::DOT) {
            Token selector = advance();
            advance(); // consume dot
            Token prop = consume(TokenType::IDENTIFIER, "Expect property name.");
            return std::make_unique<ReferenceNode>(selector.value, prop.value);
        }
    }

    if (match(TokenType::IDENTIFIER) || match(TokenType::STRING_LITERAL)) {
        return std::make_unique<LiteralNode>(tokens[current - 1], "");
    }

    if (match(TokenType::L_BRACE)) {
        auto expr = parseExpression();
        consume(TokenType::R_BRACE, "Expect '}' after expression.");
        return expr;
    }

    throw std::runtime_error("Expected expression, but found " + peek().value);
}

} // namespace CHTL