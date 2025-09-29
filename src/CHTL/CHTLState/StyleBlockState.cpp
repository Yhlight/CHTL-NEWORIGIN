#include "StyleBlockState.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/DynamicStyleNode.h"
#include "../CHTLNode/StaticStyleNode.h"
#include "../CHTLNode/ResponsiveValueNode.h"
#include "../Util/StringUtil.h"
#include <stdexcept>
#include <sstream>

namespace {
    bool isStyleValueTerminator(TokenType type) {
        switch (type) {
            case TokenType::Semicolon:
            case TokenType::CloseBrace:
            case TokenType::EndOfFile:
                return true;
            default:
                return false;
        }
    }
}

std::unique_ptr<BaseNode> StyleBlockState::handle(Parser& parser) {
    if (!parser.contextNode) {
        throw std::runtime_error("StyleBlockState requires a context node.");
    }
    parser.expectToken(TokenType::Identifier); // "style"
    parser.expectToken(TokenType::OpenBrace);
    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::Identifier) {
            parseInlineProperty(parser);
        } else {
            throw std::runtime_error("Unexpected token in style block: " + parser.currentToken.value);
        }
    }
    parser.expectToken(TokenType::CloseBrace);
    return nullptr;
}

void StyleBlockState::parseInlineProperty(Parser& parser) {
    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::Colon);

    auto valueNode = parseStyleExpression(parser);

    // If the value is responsive, register the binding.
    if (auto* responsive_node = dynamic_cast<ResponsiveValueNode*>(valueNode.get())) {
        if (parser.contextNode->attributes.find("id") == parser.contextNode->attributes.end()) {
            parser.contextNode->attributes["id"] = std::make_unique<StaticStyleNode>("chtl-id-" + std::to_string(parser.elementIdCounter++));
        }
        std::string elementId = parser.contextNode->attributes.at("id")->toString();

        ResponsiveBinding binding;
        binding.elementId = elementId;
        binding.property = "style." + key;
        binding.unit = responsive_node->getUnit();
        parser.sharedContext.responsiveBindings[responsive_node->getVariableName()].push_back(binding);
    }

    parser.contextNode->inlineStyles[key] = std::move(valueNode);

    if (parser.currentToken.type == TokenType::Semicolon) {
        parser.advanceTokens();
    }
}

std::unique_ptr<StyleValue> StyleBlockState::parseStyleExpression(Parser& parser) {
    if (parser.currentToken.type == TokenType::OpenDoubleBrace) {
        size_t start_pos = parser.currentToken.start_pos;
        size_t end_pos = start_pos;

        while (parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::EndOfFile) {
            end_pos = parser.currentToken.start_pos + parser.currentToken.value.length();
            parser.advanceTokens();
        }

        std::string final_expr = parser.lexer.getSource().substr(start_pos, end_pos - start_pos);
        trim(final_expr);

        return std::make_unique<DynamicStyleNode>(final_expr);
    }

    if (parser.currentToken.type == TokenType::Dollar) {
        parser.advanceTokens(); // consume opening $
        if (parser.currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected identifier after '$' for responsive value.");
        }
        std::string varName = parser.currentToken.value;
        parser.advanceTokens();
        parser.expectToken(TokenType::Dollar);

        std::string unit;
        if (parser.currentToken.type == TokenType::Identifier) {
            unit = parser.currentToken.value;
            parser.advanceTokens();
        }

        return std::make_unique<ResponsiveValueNode>(varName, unit);
    }

    return parsePrimaryExpr(parser);
}

std::unique_ptr<StyleValue> StyleBlockState::parsePrimaryExpr(Parser& parser) {
    std::stringstream ss;
    while (!isStyleValueTerminator(parser.currentToken.type)) {
        ss << parser.currentToken.value;
        parser.advanceTokens();
        if (!isStyleValueTerminator(parser.currentToken.type)) {
            ss << " ";
        }
    }
    std::string value = ss.str();
    trim(value);
    return std::make_unique<StaticStyleNode>(value);
}

// --- Placeholder functions for features not yet implemented/refactored ---

void StyleBlockState::parseClassOrIdSelector(Parser& parser) {
     parser.advanceTokens(); // consume . or #
     parser.advanceTokens(); // consume name
     parser.expectToken(TokenType::OpenBrace);
     while(parser.currentToken.type != TokenType::CloseBrace) parser.advanceTokens();
     parser.expectToken(TokenType::CloseBrace);
}
void StyleBlockState::parseAmpersandSelector(Parser& parser) {
    parser.advanceTokens(); // consume &
    while(parser.currentToken.type != TokenType::OpenBrace) parser.advanceTokens();
    parser.expectToken(TokenType::OpenBrace);
    while(parser.currentToken.type != TokenType::CloseBrace) parser.advanceTokens();
    parser.expectToken(TokenType::CloseBrace);
}
std::string StyleBlockState::parseCssRuleBlock(Parser& parser) { return ""; }

void StyleBlockState::parseStyleTemplateUsage(Parser& parser) {
    parser.advanceTokens(); // @
    parser.advanceTokens(); // Style
    parser.advanceTokens(); // templateName
    if (parser.currentToken.type == TokenType::Semicolon) {
        parser.advanceTokens();
    } else if (parser.currentToken.type == TokenType::OpenBrace) {
        parser.advanceTokens();
        while(parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
            parser.advanceTokens();
        }
        parser.expectToken(TokenType::CloseBrace);
    }
}

void StyleBlockState::applyStyleTemplateRecursive(
        Parser& parser,
        const std::string& ns,
        const std::string& templateName,
        std::map<std::string, std::unique_ptr<StyleValue>>& finalStyles,
        const std::vector<std::string>& deletedTemplates,
        std::vector<std::string>& visitedTemplates
    ) {
        // Placeholder
}

std::unique_ptr<StyleValue> StyleBlockState::parseAdditiveExpr(Parser& parser) { return nullptr; }
std::unique_ptr<StyleValue> StyleBlockState::parseConditionalExpr(Parser& parser) { return nullptr; }