#include "ElementParsingStrategy.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/Constraint.h"
#include "CHTL/CHTLState/StyleBlockState.h"
#include "CHTL/Util/StyleUtil.h"
#include "CHTL/CHTLState/ParserState.h"

#include <stdexcept>
#include <sstream>

// Forward declaration for a helper from another strategy (to be created)
// void parseScriptBlock(Parser& parser, ElementNode& element);


std::unique_ptr<BaseNode> ElementParsingStrategy::parse(Parser& parser) {
    auto element = std::make_unique<ElementNode>(parser.currentToken.value);
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::OpenBrace);

    parseElementBody(parser, *element);

    parser.expectToken(TokenType::CloseBrace);
    return element;
}

void ElementParsingStrategy::parseElementBody(Parser& parser, ElementNode& element) {
    parser.contextNode = &element;

    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::Identifier && parser.currentToken.value == "style" && parser.peekToken.type == TokenType::OpenBrace) {
            StyleBlockState styleState;
            styleState.handle(parser);
        } else if (parser.tryExpectKeyword(TokenType::Except, "KEYWORD_EXCEPT", "except")) {
            parseExceptClause(parser, element);
        } else if (parser.currentToken.type == TokenType::Identifier && (parser.peekToken.type == TokenType::Colon || parser.peekToken.type == TokenType::Equals)) {
            parseAttribute(parser, element);
        } else {
            // Delegate back to the parser to handle the nested statement.
            auto childNode = parser.currentState->handle(parser);
            if(childNode) {
                element.children.push_back(std::move(childNode));
            }
        }
    }

    parser.contextNode = nullptr;
}

void ElementParsingStrategy::parseAttribute(Parser& parser, ElementNode& element) {
    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    if (parser.currentToken.type != TokenType::Colon && parser.currentToken.type != TokenType::Equals) {
        throw std::runtime_error("Expected ':' or '=' after attribute key '" + key + "'.");
    }
    parser.advanceTokens();

    StyleBlockState tempStyleState;
    StyleValue value = tempStyleState.parseStyleExpression(parser);

    parser.expectToken(TokenType::Semicolon);

    if (value.type == StyleValue::RESPONSIVE) {
        if (element.attributes.find("id") == element.attributes.end()) {
            StyleValue id_val;
            id_val.type = StyleValue::STRING;
            id_val.string_val = "chtl-id-" + std::to_string(parser.elementIdCounter++);
            element.attributes["id"] = id_val;
        }
        std::string elementId = element.attributes["id"].string_val;

        ResponsiveBinding binding;
        binding.elementId = elementId;
        binding.property = key;
        binding.unit = value.unit;

        parser.sharedContext.responsiveBindings[value.responsive_var_name].push_back(binding);
    } else if (key == "text") {
        element.children.push_back(std::make_unique<TextNode>(styleValueToString(value)));
    } else {
        element.attributes[key] = value;
    }
}

void ElementParsingStrategy::parseExceptClause(Parser& parser, ElementNode& element) {
    while (parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::EndOfFile) {
        Constraint constraint;

        if (parser.currentToken.type == TokenType::OpenBracket) {
            std::stringstream ss;
            ss << parser.currentToken.value;
            parser.advanceTokens();
            ss << parser.currentToken.value;
            parser.advanceTokens();
            ss << parser.currentToken.value;
            parser.expectToken(TokenType::CloseBracket);
            constraint.meta_qualifier = ss.str();
        }

        if (parser.currentToken.type == TokenType::At) {
            parser.advanceTokens();
            constraint.type_qualifier = "@" + parser.currentToken.value;
            parser.expectToken(TokenType::Identifier);

            if (constraint.meta_qualifier.empty()) {
                if (constraint.type_qualifier == "@Html") {
                    constraint.type = ConstraintTargetType::HTML_CATEGORY;
                } else {
                    throw std::runtime_error("Category constraint with '@' must be '@Html'.");
                }
            } else {
                if (constraint.type_qualifier == "@Var" && constraint.meta_qualifier == "[Template]") {
                    constraint.type = ConstraintTargetType::TEMPLATE_VAR_CATEGORY;
                } else if (constraint.type_qualifier == "@Element" || constraint.type_qualifier == "@Style") {
                    constraint.type = ConstraintTargetType::QUALIFIED_NAME;
                    constraint.name = parser.currentToken.value;
                    parser.expectToken(TokenType::Identifier);
                } else {
                    throw std::runtime_error("Unsupported type qualifier for constraint.");
                }
            }
        } else if (!constraint.meta_qualifier.empty()) {
            if (constraint.meta_qualifier == "[Template]") {
                constraint.type = ConstraintTargetType::TEMPLATE_CATEGORY;
            } else if (constraint.meta_qualifier == "[Custom]") {
                constraint.type = ConstraintTargetType::CUSTOM_CATEGORY;
            } else {
                 throw std::runtime_error("Unsupported meta qualifier for constraint.");
            }
        } else if (parser.currentToken.type == TokenType::Identifier) {
            constraint.type = ConstraintTargetType::TAG_NAME;
            constraint.name = parser.currentToken.value;
            parser.advanceTokens();
        } else {
            throw std::runtime_error("Invalid token in 'except' clause.");
        }

        element.constraints.push_back(constraint);

        if (parser.currentToken.type == TokenType::Comma) {
            parser.advanceTokens();
        } else {
            break;
        }
    }
    parser.expectToken(TokenType::Semicolon);
}