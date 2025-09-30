#include "StyleBlockState.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/DynamicStyleNode.h"
#include "../CHTLNode/StaticStyleNode.h"
#include "../CHTLNode/ResponsiveValueNode.h"
#include "../ExpressionParser/ExpressionLexer.h"
#include "../ExpressionParser/ExpressionParser.h"
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
        } else if (parser.currentToken.type == TokenType::At) {
            parseStyleTemplateUsage(parser);
        }
        else {
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
    // 1. Collect tokens until a terminator. This is more robust than grabbing a raw substring
    // and correctly handles the lexer's new behavior of separating numbers from units.
    std::vector<Token> valueTokens;
    while (!isStyleValueTerminator(parser.currentToken.type)) {
        valueTokens.push_back(parser.currentToken);
        parser.advanceTokens();
    }

    if (valueTokens.empty()) {
        throw std::runtime_error("Expected a value for the style property.");
    }

    // 2. Reconstruct the expression string and check for operators.
    std::stringstream expression_ss;
    bool hasOperator = false;
    for (size_t i = 0; i < valueTokens.size(); ++i) {
        const auto& token = valueTokens[i];
        expression_ss << token.value;

        if (i < valueTokens.size() - 1) {
            const auto& nextToken = valueTokens[i+1];
            bool isUnit = (nextToken.type == TokenType::Identifier || nextToken.type == TokenType::Percent);
            if (token.type != TokenType::Number || !isUnit) {
                 expression_ss << " ";
            }
        }
        if (token.type == TokenType::Plus || token.type == TokenType::Minus || token.type == TokenType::Asterisk ||
            token.type == TokenType::Slash || token.type == TokenType::QuestionMark) {
            hasOperator = true;
        }
    }
    std::string expression_string = expression_ss.str();
    trim(expression_string);

    // 3. If there are no complex operators, treat it as a static or responsive value.
    if (!hasOperator) {
        // Check for responsive values, e.g., $myVar$ or $myVar$px
        if (valueTokens.size() > 2 && valueTokens[0].type == TokenType::Dollar && valueTokens[2].type == TokenType::Dollar) {
            if (valueTokens.size() == 3) {
                // Case: $myVar$
                return std::make_unique<ResponsiveValueNode>(valueTokens[1].value, "");
            }
            if (valueTokens.size() == 4 && valueTokens[3].type == TokenType::Identifier) {
                // Case: $myVar$px
                return std::make_unique<ResponsiveValueNode>(valueTokens[1].value, valueTokens[3].value);
            }
        }
        return std::make_unique<StaticStyleNode>(expression_string);
    }

    // 4. If it's a complex expression, use the ExpressionParser.
    ExpressionLexer expressionLexer(expression_string);
    std::vector<ExpressionToken> expressionTokens = expressionLexer.tokenize();
    ExpressionParser expressionParser(expressionTokens);
    std::unique_ptr<ExpressionBaseNode> expressionAst = expressionParser.parse();

    return std::make_unique<DynamicStyleNode>(std::move(expressionAst));
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
    parser.expectToken(TokenType::At);
    if (parser.currentToken.value != "Style") {
        throw std::runtime_error("Expected '@Style' for template usage.");
    }
    parser.advanceTokens(); // Consume 'Style'

    std::string templateName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::Semicolon);

    // Get the template from the manager
    const StyleTemplateNode* styleTemplate = parser.templateManager.getStyleTemplate(parser.getCurrentNamespace(), templateName);

    if (!styleTemplate) {
        throw std::runtime_error("Style template '" + templateName + "' not found in namespace '" + parser.getCurrentNamespace() + "'.");
    }

    // Apply the styles to the context node
    for (const auto& pair : styleTemplate->styles) {
        parser.contextNode->inlineStyles[pair.first] = pair.second->clone();
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