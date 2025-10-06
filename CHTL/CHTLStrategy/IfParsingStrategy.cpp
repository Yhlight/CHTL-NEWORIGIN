#include "IfParsingStrategy.h"
#include "ElementParsingStrategy.h"
#include "TextParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/IfNode.h"
#include "../CHTLNode/PropertyNode.h"
#include <stdexcept>

namespace CHTL {

// Forward declarations
std::shared_ptr<BaseNode> parseStylingIfChain(CHTLParserContext* context);
std::shared_ptr<BaseNode> parseRenderingIfChain(CHTLParserContext* context);

// Constructor
IfParsingStrategy::IfParsingStrategy(IfParsingMode mode) : mode(mode) {}

// Main parse method - dispatches to the correct implementation
std::shared_ptr<BaseNode> IfParsingStrategy::parse(CHTLParserContext* context) {
    if (context->getCurrentToken().type != TokenType::TOKEN_IF) {
        // This should be unreachable if the calling strategies are correct.
        throw std::logic_error("IfParsingStrategy must be called on an 'if' token.");
    }

    if (mode == IfParsingMode::Styling) {
        return parseStylingIfChain(context);
    } else {
        return parseRenderingIfChain(context);
    }
}

// =============================================================================
// Common Helpers
// =============================================================================

namespace {
    std::string parse_value_in_block(CHTLParserContext* context) {
        std::string value;
        while (context->getCurrentToken().type != TokenType::TOKEN_COMMA &&
               context->getCurrentToken().type != TokenType::TOKEN_RBRACE &&
               !context->isAtEnd()) {
            value += context->getCurrentToken().lexeme;
            const auto& next_token = context->peek(1);
            if (next_token.type != TokenType::TOKEN_COMMA && next_token.type != TokenType::TOKEN_RBRACE) {
                 value += " ";
            }
            context->advance();
        }
        if (!value.empty() && value.back() == ' ') {
            value.pop_back();
        }
        return value;
    }
}

// =============================================================================
// Styling Mode Implementation
// =============================================================================

namespace {
    void parse_styling_block_content(CHTLParserContext* context, std::shared_ptr<IfNode> ifNode) {
        while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
            if (context->getCurrentToken().type != TokenType::TOKEN_IDENTIFIER && context->getCurrentToken().type != TokenType::TOKEN_UNQUOTED_LITERAL) {
                throw std::runtime_error("Expected property identifier in style 'if' block.");
            }
            std::string key = context->getCurrentToken().lexeme;
            context->advance();

            if (context->getCurrentToken().type != TokenType::TOKEN_COLON) {
                throw std::runtime_error("Expected ':' after property key '" + key + "'.");
            }
            context->advance();

            std::string value = parse_value_in_block(context);

            if (key == "condition") {
                if (ifNode->if_type == IfType::ELSE) throw std::runtime_error("'else' block cannot have a condition.");
                ifNode->condition = value;
            } else {
                ifNode->addChild(std::make_shared<PropertyNode>(key, value));
            }

            if (context->getCurrentToken().type == TokenType::TOKEN_COMMA) {
                context->advance();
            } else if (context->getCurrentToken().type != TokenType::TOKEN_RBRACE) {
                throw std::runtime_error("Expected ',' or '}' after property in style 'if' block.");
            }
        }
        if (ifNode->if_type != IfType::ELSE && ifNode->condition.empty()) {
            throw std::runtime_error("'if' and 'else if' blocks must have a 'condition' property.");
        }
    }
}

std::shared_ptr<BaseNode> parseStylingIfChain(CHTLParserContext* context) {
    auto headNode = std::make_shared<IfNode>();
    auto currentNode = headNode;

    // --- Parse initial 'if' ---
    currentNode->if_type = IfType::IF;
    context->advance(); // consume 'if'

    if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE) throw std::runtime_error("Expected '{' after 'if' keyword.");
    context->advance(); // consume '{'
    parse_styling_block_content(context, currentNode);
    if (context->getCurrentToken().type != TokenType::TOKEN_RBRACE) throw std::runtime_error("Expected '}' to close 'if' block.");
    context->advance(); // consume '}'

    // --- Loop for 'else if' / 'else' ---
    while (!context->isAtEnd() && context->getCurrentToken().type == TokenType::TOKEN_ELSE) {
        context->advance(); // consume 'else'

        auto nextNode = std::make_shared<IfNode>();
        if (context->getCurrentToken().type == TokenType::TOKEN_IF) {
            nextNode->if_type = IfType::ELSE_IF;
            context->advance(); // consume 'if'
        } else {
            nextNode->if_type = IfType::ELSE;
        }

        currentNode->next_if = nextNode;
        currentNode = nextNode;

        if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE) throw std::runtime_error("Expected '{' for 'else'/'else if' block.");
        context->advance(); // consume '{'
        parse_styling_block_content(context, currentNode);
        if (context->getCurrentToken().type != TokenType::TOKEN_RBRACE) throw std::runtime_error("Expected '}' to close 'else'/'else if' block.");
        context->advance(); // consume '}'

        if (currentNode->if_type == IfType::ELSE) break;
    }
    return headNode;
}

// =============================================================================
// Rendering Mode Implementation
// =============================================================================

namespace {
    void parse_rendering_children(CHTLParserContext* context, std::shared_ptr<BaseNode> parentNode) {
        while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
            TokenType currentType = context->getCurrentToken().type;
            if (currentType == TokenType::TOKEN_IDENTIFIER) {
                context->setStrategy(std::make_unique<ElementParsingStrategy>());
                parentNode->addChild(context->runCurrentStrategy());
            } else if (currentType == TokenType::TOKEN_TEXT) {
                context->setStrategy(std::make_unique<TextParsingStrategy>());
                parentNode->addChild(context->runCurrentStrategy());
            } else if (currentType == TokenType::TOKEN_IF) {
                context->setStrategy(std::make_unique<IfParsingStrategy>(IfParsingMode::Rendering));
                parentNode->addChild(context->runCurrentStrategy());
            } else {
                throw std::runtime_error("Unexpected token '" + context->getCurrentToken().lexeme + "' in rendering 'if' block body.");
            }
        }
    }

    std::string parse_rendering_condition(CHTLParserContext* context) {
        if (context->getCurrentToken().lexeme != "condition") throw std::runtime_error("Expected 'condition' property.");
        context->advance();
        if (context->getCurrentToken().type != TokenType::TOKEN_COLON) throw std::runtime_error("Expected ':' after 'condition'.");
        context->advance();
        std::string condition_str = parse_value_in_block(context);
        if (context->getCurrentToken().type == TokenType::TOKEN_COMMA || context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) context->advance();
        if (condition_str.empty()) throw std::runtime_error("Condition expression cannot be empty.");
        return condition_str;
    }
}

std::shared_ptr<BaseNode> parseRenderingIfChain(CHTLParserContext* context) {
    auto headNode = std::make_shared<IfNode>();
    auto currentNode = headNode;

    // --- Parse initial 'if' ---
    currentNode->if_type = IfType::IF;
    context->advance(); // consume 'if'
    if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE) throw std::runtime_error("Expected '{' after 'if' keyword.");
    context->advance(); // consume '{'
    currentNode->condition = parse_rendering_condition(context);
    parse_rendering_children(context, currentNode);
    if (context->getCurrentToken().type != TokenType::TOKEN_RBRACE) throw std::runtime_error("Expected '}' to close 'if' block.");
    context->advance(); // consume '}'

    // --- Loop for 'else if' / 'else' ---
    while (!context->isAtEnd() && context->getCurrentToken().type == TokenType::TOKEN_ELSE) {
        context->advance(); // consume 'else'
        auto nextNode = std::make_shared<IfNode>();
        if (context->getCurrentToken().type == TokenType::TOKEN_IF) {
            nextNode->if_type = IfType::ELSE_IF;
            context->advance(); // consume 'if'
        } else {
            nextNode->if_type = IfType::ELSE;
        }
        currentNode->next_if = nextNode;
        currentNode = nextNode;
        if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE) throw std::runtime_error("Expected '{' for 'else'/'else if' block.");
        context->advance(); // consume '{'
        if (currentNode->if_type == IfType::ELSE_IF) {
            currentNode->condition = parse_rendering_condition(context);
        } else if (context->getCurrentToken().lexeme == "condition") {
            throw std::runtime_error("'else' block cannot have a condition.");
        }
        parse_rendering_children(context, currentNode);
        if (context->getCurrentToken().type != TokenType::TOKEN_RBRACE) throw std::runtime_error("Expected '}' to close 'else'/'else if' block.");
        context->advance(); // consume '}'
        if (currentNode->if_type == IfType::ELSE) break;
    }
    return headNode;
}

}