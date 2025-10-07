/**
 * @file ConditionalParser.cpp
 * @brief Implementation of ConditionalParser
 */

#include "ConditionalParser.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <sstream>

namespace CHTL {

// Static invalid token
const Token ConditionalParser::invalidToken_ = Token();

// ============================================================================
// Constructor
// ============================================================================

ConditionalParser::ConditionalParser(const Vector<Token>& tokens)
    : tokens_(tokens) {
}

// ============================================================================
// Main Parsing Methods
// ============================================================================

SharedPtr<ConditionalNode> ConditionalParser::parseConditionalChain(size_t& currentPos) {
    // Parse main if block
    auto ifNode = parseIfBlock(currentPos);
    if (!ifNode) {
        return nullptr;
    }
    
    // Parse else-if chain
    while (isAtElseIfKeywords(currentPos)) {
        auto elseIfNode = parseElseIfBlock(currentPos);
        if (elseIfNode) {
            ifNode->addElseIfBlock(elseIfNode);
        } else {
            break;
        }
    }
    
    // Parse optional else block
    if (isAtElseKeyword(currentPos) && !isAtElseIfKeywords(currentPos)) {
        auto elseNode = parseElseBlock(currentPos);
        if (elseNode) {
            ifNode->setElseBlock(elseNode);
        }
    }
    
    return ifNode;
}

SharedPtr<ConditionalNode> ConditionalParser::parseIfBlock(size_t& currentPos) {
    // Expect 'if' keyword
    if (!expectKeyword(currentPos, "if", "Expected 'if' keyword")) {
        return nullptr;
    }
    
    // Create conditional node
    auto node = std::make_shared<ConditionalNode>();
    node->setBlockType(ConditionalNode::BlockType::If);
    
    // Expect '{'
    if (!expectToken(currentPos, TokenType::LeftBrace, "Expected '{' after 'if'")) {
        return nullptr;
    }
    
    // Parse condition and properties
    parseConditionalProperties(currentPos, node);
    
    // Expect '}'
    if (!expectToken(currentPos, TokenType::RightBrace, "Expected '}' to close 'if' block")) {
        return nullptr;
    }
    
    return node;
}

SharedPtr<ConditionalNode> ConditionalParser::parseElseIfBlock(size_t& currentPos) {
    // Expect 'else' keyword
    if (!expectKeyword(currentPos, "else", "Expected 'else' keyword")) {
        return nullptr;
    }
    
    // Expect 'if' keyword
    if (!expectKeyword(currentPos, "if", "Expected 'if' keyword after 'else'")) {
        return nullptr;
    }
    
    // Create conditional node
    auto node = std::make_shared<ConditionalNode>();
    node->setBlockType(ConditionalNode::BlockType::ElseIf);
    
    // Expect '{'
    if (!expectToken(currentPos, TokenType::LeftBrace, "Expected '{' after 'else if'")) {
        return nullptr;
    }
    
    // Parse condition and properties
    parseConditionalProperties(currentPos, node);
    
    // Expect '}'
    if (!expectToken(currentPos, TokenType::RightBrace, "Expected '}' to close 'else if' block")) {
        return nullptr;
    }
    
    return node;
}

SharedPtr<ConditionalNode> ConditionalParser::parseElseBlock(size_t& currentPos) {
    // Expect 'else' keyword
    if (!expectKeyword(currentPos, "else", "Expected 'else' keyword")) {
        return nullptr;
    }
    
    // Create conditional node
    auto node = std::make_shared<ConditionalNode>();
    node->setBlockType(ConditionalNode::BlockType::Else);
    
    // Expect '{'
    if (!expectToken(currentPos, TokenType::LeftBrace, "Expected '{' after 'else'")) {
        return nullptr;
    }
    
    // Parse properties (no condition for else block)
    parseConditionalProperties(currentPos, node);
    
    // Expect '}'
    if (!expectToken(currentPos, TokenType::RightBrace, "Expected '}' to close 'else' block")) {
        return nullptr;
    }
    
    return node;
}

// ============================================================================
// Helper Parsing Methods
// ============================================================================

void ConditionalParser::parseConditionalProperties(size_t& currentPos, SharedPtr<ConditionalNode>& node) {
    skipWhitespaceAndComments(currentPos);
    
    // Parse properties until we hit '}'
    while (isValidPosition(currentPos) && !matchToken(currentPos, TokenType::RightBrace)) {
        // Check for 'condition:' key
        if (matchKeyword(currentPos, "condition")) {
            currentPos++; // consume 'condition'
            
            // Expect ':'
            if (!expectToken(currentPos, TokenType::Colon, "Expected ':' after 'condition'")) {
                break;
            }
            
            // Parse condition expression
            String condition = parseCondition(currentPos);
            node->setCondition(condition);
            
            // Check if dynamic
            if (isDynamicCondition(condition)) {
                node->setDynamic(true);
            }
            
            // Expect ',' (optional)
            if (matchToken(currentPos, TokenType::Comma)) {
                currentPos++;
            }
        }
        // Parse CSS property
        else {
            auto [prop, value] = parseProperty(currentPos);
            if (!prop.empty()) {
                node->addStyle(prop, value);
            }
            
            // Expect ',' (optional)
            if (matchToken(currentPos, TokenType::Comma)) {
                currentPos++;
            }
        }
        
        skipWhitespaceAndComments(currentPos);
    }
}

String ConditionalParser::parseCondition(size_t& currentPos) {
    std::stringstream condition;
    
    // Parse tokens until we hit ',' or '}'
    while (isValidPosition(currentPos) && 
           !matchToken(currentPos, TokenType::Comma) && 
           !matchToken(currentPos, TokenType::RightBrace)) {
        
        const auto& token = getToken(currentPos);
        
        if (token.type == TokenType::Whitespace) {
            condition << " ";
        } else {
            condition << token.value;
        }
        
        currentPos++;
    }
    
    return StringUtil::trim(condition.str());
}

Pair<String, String> ConditionalParser::parseProperty(size_t& currentPos) {
    // Property format: property: value; or property = value;
    
    skipWhitespaceAndComments(currentPos);
    
    // Get property name
    const auto& propToken = getToken(currentPos);
    if (propToken.type != TokenType::Identifier) {
        return {"", ""};
    }
    
    String property = propToken.value;
    currentPos++;
    
    // Expect ':' or '=' (CE对等式)
    if (!matchToken(currentPos, TokenType::Colon) && !matchToken(currentPos, TokenType::Equal)) {
        return {"", ""};
    }
    currentPos++;
    
    skipWhitespaceAndComments(currentPos);
    
    // Get property value (may be multiple tokens)
    std::stringstream value;
    
    while (isValidPosition(currentPos) && 
           !matchToken(currentPos, TokenType::Comma) &&
           !matchToken(currentPos, TokenType::Semicolon) &&
           !matchToken(currentPos, TokenType::RightBrace)) {
        
        const auto& token = getToken(currentPos);
        
        if (token.type == TokenType::Whitespace) {
            value << " ";
        } else {
            value << token.value;
        }
        
        currentPos++;
    }
    
    // Optional semicolon
    if (matchToken(currentPos, TokenType::Semicolon)) {
        currentPos++;
    }
    
    return {property, StringUtil::trim(value.str())};
}

// ============================================================================
// Keyword and Token Checking
// ============================================================================

bool ConditionalParser::isAtIfKeyword(size_t pos) const {
    return matchKeyword(pos, "if");
}

bool ConditionalParser::isAtElseKeyword(size_t pos) const {
    return matchKeyword(pos, "else");
}

bool ConditionalParser::isAtElseIfKeywords(size_t pos) const {
    if (!matchKeyword(pos, "else")) {
        return false;
    }
    
    // Check next token is 'if'
    size_t nextPos = pos + 1;
    skipWhitespaceAndComments(nextPos);
    
    return matchKeyword(nextPos, "if");
}

// ============================================================================
// Utility Methods
// ============================================================================

bool ConditionalParser::expectToken(size_t& currentPos, TokenType type, const String& errorMsg) {
    skipWhitespaceAndComments(currentPos);
    
    if (!matchToken(currentPos, type)) {
        if (!errorMsg.empty()) {
            // TODO: Add error reporting
        }
        return false;
    }
    
    currentPos++;
    return true;
}

bool ConditionalParser::expectKeyword(size_t& currentPos, const String& keyword, const String& errorMsg) {
    skipWhitespaceAndComments(currentPos);
    
    if (!matchKeyword(currentPos, keyword)) {
        if (!errorMsg.empty()) {
            // TODO: Add error reporting
        }
        return false;
    }
    
    currentPos++;
    return true;
}

bool ConditionalParser::matchToken(size_t pos, TokenType type) const {
    if (!isValidPosition(pos)) {
        return false;
    }
    
    return tokens_[pos].type == type;
}

bool ConditionalParser::matchKeyword(size_t pos, const String& keyword) const {
    if (!isValidPosition(pos)) {
        return false;
    }
    
    const auto& token = tokens_[pos];
    return token.type == TokenType::Identifier && token.value == keyword;
}

const Token& ConditionalParser::getToken(size_t pos) const {
    if (!isValidPosition(pos)) {
        return invalidToken_;
    }
    
    return tokens_[pos];
}

bool ConditionalParser::isValidPosition(size_t pos) const {
    return pos < tokens_.size();
}

bool ConditionalParser::isDynamicCondition(const String& condition) const {
    return ConditionalUtils::containsEnhancedSelector(condition);
}

void ConditionalParser::skipWhitespaceAndComments(size_t& currentPos) {
    while (isValidPosition(currentPos)) {
        const auto& token = tokens_[currentPos];
        
        if (token.type == TokenType::Whitespace ||
            token.type == TokenType::Comment ||
            token.type == TokenType::MultilineComment) {
            currentPos++;
        } else {
            break;
        }
    }
}

} // namespace CHTL
