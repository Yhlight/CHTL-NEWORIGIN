#include "ElementParsingStrategy.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/Constraint.h"
#include "CHTL/CHTLState/StyleBlockState.h"
#include "CHTL/CHTLState/ParserState.h"
#include "CHTL/CHTLNode/StaticStyleNode.h"
#include "CHTL/CHTLNode/ResponsiveValueNode.h"

#include <stdexcept>
#include <sstream>

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

    if (parser.currentToken.type == TokenType::Dollar) {
        parser.advanceTokens(); // consume opening $
        if (parser.currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected identifier after '$' for responsive value.");
        }
        std::string varName = parser.currentToken.value;
        parser.advanceTokens();
        parser.expectToken(TokenType::Dollar);

        // Create the responsive node
        auto responsiveNode = std::make_unique<ResponsiveValueNode>(varName);

        // Register binding in SharedContext
        if (element.attributes.find("id") == element.attributes.end()) {
             element.attributes["id"] = std::make_unique<StaticStyleNode>("chtl-id-" + std::to_string(parser.elementIdCounter++));
        }
        std::string elementId = element.attributes.at("id")->toString();

        ResponsiveBinding binding;
        binding.elementId = elementId;
        binding.property = key;
        binding.unit = ""; // No units for attributes
        parser.sharedContext.responsiveBindings[varName].push_back(binding);

        element.attributes[key] = std::move(responsiveNode);
        parser.expectToken(TokenType::Semicolon);
        return;
    }

    // Default to static string parsing if not a responsive value
    std::stringstream value_ss;
    while(parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::EndOfFile) {
        value_ss << parser.currentToken.value;
        parser.advanceTokens();
        if (parser.currentToken.type != TokenType::Semicolon) {
             value_ss << " ";
        }
    }

    std::string value_str = value_ss.str();

    if (value_str.length() >= 2 && value_str.front() == '"' && value_str.back() == '"') {
        value_str = value_str.substr(1, value_str.length() - 2);
    }

    parser.expectToken(TokenType::Semicolon);

    if (key == "text") {
        element.children.push_back(std::make_unique<TextNode>(value_str));
    } else {
        element.attributes[key] = std::make_unique<StaticStyleNode>(value_str);
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