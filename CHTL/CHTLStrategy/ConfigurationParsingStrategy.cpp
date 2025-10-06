#include "ConfigurationParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/ConfigurationNode.h"
#include "../CHTLNode/PropertyNode.h"
#include <stdexcept>

namespace CHTL {

// Helper function to check if a token can be a property key inside [Name]
bool is_valid_key_token(TokenType type) {
    return type == TokenType::TOKEN_IDENTIFIER ||
           type == TokenType::TOKEN_UNQUOTED_LITERAL ||
           type == TokenType::TOKEN_STYLE ||
           type == TokenType::TOKEN_SCRIPT ||
           type == TokenType::TOKEN_TEXT ||
           type == TokenType::TOKEN_IF ||
           type == TokenType::TOKEN_ELSE ||
           type == TokenType::TOKEN_EXCEPT ||
           type == TokenType::TOKEN_KEYWORD_USE ||
           type == TokenType::TOKEN_KEYWORD_HTML5 ||
           type == TokenType::TOKEN_KEYWORD_FROM;
}

// Helper function to parse a single property, returning a PropertyNode or nullptr if no property is found.
static std::shared_ptr<PropertyNode> parse_property(CHTLParserContext* context) {
    if (!is_valid_key_token(context->getCurrentToken().type)) {
        return nullptr;
    }
    std::string key = context->getCurrentToken().lexeme;
    context->advance();

    if (context->getCurrentToken().type != TokenType::TOKEN_COLON && context->getCurrentToken().type != TokenType::TOKEN_ASSIGN) {
        throw std::runtime_error("Expected ':' or '=' after property key '" + key + "'.");
    }
    context->advance(); // consume ':' or '='

    std::string value;
    if (context->getCurrentToken().type == TokenType::TOKEN_STRING_LITERAL ||
        context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL ||
        context->getCurrentToken().type == TokenType::TOKEN_NUMERIC_LITERAL ||
        context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
        value = context->getCurrentToken().lexeme;
        context->advance();
    } else {
        throw std::runtime_error("Invalid value for property '" + key + "'.");
    }

    if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
        context->advance(); // consume optional ';'
    }

    return std::make_shared<PropertyNode>(key, value);
}

std::shared_ptr<BaseNode> ConfigurationParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume '['
    if (context->getCurrentToken().lexeme != "Configuration") throw std::runtime_error("Expected 'Configuration' keyword.");
    context->advance();

    auto configNode = std::make_shared<ConfigurationNode>();

    if (context->getCurrentToken().type != TokenType::TOKEN_RBRACKET) throw std::runtime_error("Expected ']' to close configuration tag.");
    context->advance();

    if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE) throw std::runtime_error("Expected '{' to start configuration block.");
    context->advance();

    while (true) {
        if (context->isAtEnd()) throw std::runtime_error("Unclosed configuration block.");
        if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
            context->advance(); // Consume the final '}'
            break;
        }

        if (context->getCurrentToken().type == TokenType::TOKEN_LBRACKET) {
            context->advance(); // consume '['
            std::string blockName = context->getCurrentToken().lexeme;
            context->advance(); // consume block name
            if (context->getCurrentToken().type != TokenType::TOKEN_RBRACKET) throw std::runtime_error("Expected ']' to close nested block tag.");
            context->advance();

            if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE) throw std::runtime_error("Expected '{' to start nested block.");
            context->advance();

            std::vector<std::shared_ptr<PropertyNode>> properties;
            while(true) {
                if (context->isAtEnd()) throw std::runtime_error("Unclosed nested block.");
                if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
                    context->advance(); // Consume nested '}'
                    break;
                }
                if(auto prop = parse_property(context)) {
                    properties.push_back(prop);
                } else {
                    throw std::runtime_error("Unexpected token in nested configuration block: " + context->getCurrentToken().lexeme);
                }
            }
            configNode->addNestedBlock(blockName, properties);
        } else if (auto prop = parse_property(context)) {
            configNode->addChild(prop);
        } else {
            throw std::runtime_error("Unexpected token in configuration block: " + context->getCurrentToken().lexeme);
        }
    }

    return configNode;
}

}