/**
 * @file Strategy.cpp
 * @brief Implementation of CHTL Strategy Pattern
 */

#include "Strategy.h"
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/StyleNode.h"
#include "CHTL/CHTLNode/ScriptNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include <iostream>

namespace CHTL {

// ===== ElementParseStrategy =====

std::shared_ptr<BaseNode> ElementParseStrategy::parse(
    const std::vector<Token>& tokens,
    CHTLContext& /* context */)
{
    if (tokens.empty()) {
        return nullptr;
    }
    
    // Create element node
    auto element = std::make_shared<ElementNode>(tokens[0].lexeme);
    
    // TODO: Parse attributes, children, etc.
    
    return element;
}

bool ElementParseStrategy::canHandle(const std::vector<Token>& tokens) const {
    return !tokens.empty() && tokens[0].type == TokenType::IDENTIFIER;
}

// ===== StyleParseStrategy =====

std::shared_ptr<BaseNode> StyleParseStrategy::parse(
    const std::vector<Token>& /* tokens */,
    CHTLContext& /* context */)
{
    auto styleNode = std::make_shared<StyleNode>();
    
    // TODO: Parse style properties
    
    return styleNode;
}

bool StyleParseStrategy::canHandle(const std::vector<Token>& tokens) const {
    return !tokens.empty() && tokens[0].type == TokenType::KEYWORD_STYLE;
}

// ===== ScriptParseStrategy =====

std::shared_ptr<BaseNode> ScriptParseStrategy::parse(
    const std::vector<Token>& /* tokens */,
    CHTLContext& /* context */)
{
    auto scriptNode = std::make_shared<ScriptNode>();
    
    // TODO: Pass to CHTL JS compiler via salt bridge
    
    return scriptNode;
}

bool ScriptParseStrategy::canHandle(const std::vector<Token>& tokens) const {
    return !tokens.empty() && tokens[0].type == TokenType::KEYWORD_SCRIPT;
}

// ===== TextParseStrategy =====

std::shared_ptr<BaseNode> TextParseStrategy::parse(
    const std::vector<Token>& tokens,
    CHTLContext& /* context */)
{
    if (tokens.empty()) {
        return nullptr;
    }
    
    std::string content;
    for (const auto& token : tokens) {
        if (isLiteral(token.type)) {
            content += token.lexeme;
        }
    }
    
    return std::make_shared<TextNode>(content);
}

bool TextParseStrategy::canHandle(const std::vector<Token>& tokens) const {
    return !tokens.empty() && 
           (tokens[0].type == TokenType::KEYWORD_TEXT ||
            isLiteral(tokens[0].type));
}

// ===== TemplateParseStrategy =====

std::shared_ptr<BaseNode> TemplateParseStrategy::parse(
    const std::vector<Token>& /* tokens */,
    CHTLContext& /* context */)
{
    // TODO: Implement template parsing
    return nullptr;
}

bool TemplateParseStrategy::canHandle(const std::vector<Token>& tokens) const {
    return !tokens.empty() && tokens[0].type == TokenType::KEYWORD_TEMPLATE;
}

// ===== CustomParseStrategy =====

std::shared_ptr<BaseNode> CustomParseStrategy::parse(
    const std::vector<Token>& /* tokens */,
    CHTLContext& /* context */)
{
    // TODO: Implement custom definition parsing
    return nullptr;
}

bool CustomParseStrategy::canHandle(const std::vector<Token>& tokens) const {
    return !tokens.empty() && tokens[0].type == TokenType::KEYWORD_CUSTOM;
}

// ===== CommentParseStrategy =====

std::shared_ptr<BaseNode> CommentParseStrategy::parse(
    const std::vector<Token>& tokens,
    CHTLContext& /* context */)
{
    if (tokens.empty()) {
        return nullptr;
    }
    
    auto commentNode = std::make_shared<CommentNode>(
        tokens[0].lexeme,
        tokens[0].type == TokenType::GENERATOR_COMMENT
    );
    
    return commentNode;
}

bool CommentParseStrategy::canHandle(const std::vector<Token>& tokens) const {
    return !tokens.empty() && 
           (tokens[0].type == TokenType::LINE_COMMENT ||
            tokens[0].type == TokenType::BLOCK_COMMENT ||
            tokens[0].type == TokenType::GENERATOR_COMMENT);
}

// ===== HTMLGenerationStrategy =====

std::string HTMLGenerationStrategy::generate(
    const std::shared_ptr<BaseNode>& node,
    CHTLContext& /* context */)
{
    if (!node) {
        return "";
    }
    
    return node->toHTML();
}

// ===== CSSGenerationStrategy =====

std::string CSSGenerationStrategy::generate(
    const std::shared_ptr<BaseNode>& node,
    CHTLContext& /* context */)
{
    if (!node) {
        return "";
    }
    
    return node->toCSS();
}

// ===== JSGenerationStrategy =====

std::string JSGenerationStrategy::generate(
    const std::shared_ptr<BaseNode>& node,
    CHTLContext& /* context */)
{
    if (!node) {
        return "";
    }
    
    return node->toJS();
}

// ===== StrategySelector =====

std::shared_ptr<ParseStrategy> StrategySelector::selectParseStrategy(
    const std::vector<Token>& tokens)
{
    if (tokens.empty()) {
        return nullptr;
    }
    
    // Try each strategy in order of specificity
    std::vector<std::shared_ptr<ParseStrategy>> strategies = {
        std::make_shared<CommentParseStrategy>(),
        std::make_shared<TemplateParseStrategy>(),
        std::make_shared<CustomParseStrategy>(),
        std::make_shared<StyleParseStrategy>(),
        std::make_shared<ScriptParseStrategy>(),
        std::make_shared<TextParseStrategy>(),
        std::make_shared<ElementParseStrategy>()
    };
    
    for (auto& strategy : strategies) {
        if (strategy->canHandle(tokens)) {
            return strategy;
        }
    }
    
    return nullptr;
}

std::shared_ptr<GenerationStrategy> StrategySelector::selectGenerationStrategy(
    const std::string& nodeType)
{
    if (nodeType == "Element" || nodeType == "Text") {
        return std::make_shared<HTMLGenerationStrategy>();
    } else if (nodeType == "Style") {
        return std::make_shared<CSSGenerationStrategy>();
    } else if (nodeType == "Script") {
        return std::make_shared<JSGenerationStrategy>();
    }
    
    return std::make_shared<HTMLGenerationStrategy>();
}

} // namespace CHTL
