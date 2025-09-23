#include "StatementState.h"

#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <stdexcept>
#include <string>
#include <utility> // For std::pair
#include <sstream> // For std::ostringstream

// Forward declare to resolve circular dependency between element parsing and statement parsing
class ElementNode;

// The main handler for this state. It acts as a dispatcher.
std::unique_ptr<BaseNode> StatementState::handle(Parser& parser) {
    if (parser.currentToken.type == TokenType::Identifier) {
        if (parser.currentToken.value == "text") {
            return parseTextElement(parser);
        }
        // Any other identifier is assumed to be an element tag.
        return parseElement(parser);
    } else if (parser.currentToken.type == TokenType::HashComment) {
        return parseComment(parser);
    }

    throw std::runtime_error("Statements must begin with an identifier or hash comment. Found '" + parser.currentToken.value + "' instead.");
}

// Parses a full element, including its body.
std::unique_ptr<BaseNode> StatementState::parseElement(Parser& parser) {
    auto element = std::make_unique<ElementNode>(parser.currentToken.value);
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::OpenBrace);

    parseElementBody(parser, *element);

    parser.expectToken(TokenType::CloseBrace);
    return element;
}

// Parses the body of an element, dispatching to helpers for attributes, styles, or child nodes.
void StatementState::parseElementBody(Parser& parser, ElementNode& element) {
    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::Identifier && parser.currentToken.value == "style" && parser.peekToken.type == TokenType::OpenBrace) {
            parseStyleBlock(parser, element);
        } else if (parser.currentToken.type == TokenType::Identifier && (parser.peekToken.type == TokenType::Colon || parser.peekToken.type == TokenType::Equals)) {
            parseAttribute(parser, element);
        } else {
            // It's a child statement. Set the state and recursively call the main handler.
            // Note: This simple implementation doesn't use setState, it just calls handle directly.
            // A more complex implementation would transition states.
            element.children.push_back(handle(parser));
        }
    }
}

// Parses a 'text { "..." }' block.
std::unique_ptr<BaseNode> StatementState::parseTextElement(Parser& parser) {
    parser.expectToken(TokenType::Identifier); // consume 'text'
    parser.expectToken(TokenType::OpenBrace);
    std::string textContent = parser.currentToken.value;
    parser.expectToken(TokenType::String);
    parser.expectToken(TokenType::CloseBrace);
    return std::make_unique<TextNode>(textContent);
}

// Parses a '# comment' line.
std::unique_ptr<BaseNode> StatementState::parseComment(Parser& parser) {
    auto comment = std::make_unique<CommentNode>(parser.currentToken.value);
    parser.expectToken(TokenType::HashComment);
    return comment;
}

// Parses an attribute 'key: value;'.
void StatementState::parseAttribute(Parser& parser, ElementNode& element) {
    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    if (parser.currentToken.type != TokenType::Colon && parser.currentToken.type != TokenType::Equals) {
        throw std::runtime_error("Expected ':' or '=' after attribute key '" + key + "'.");
    }
    parser.advanceTokens();

    std::string value;
    if (parser.currentToken.type == TokenType::Identifier || parser.currentToken.type == TokenType::String) {
        value = parser.currentToken.value;
        parser.advanceTokens();
    } else {
        throw std::runtime_error("Attribute value for '" + key + "' must be an identifier or a string.");
    }

    parser.expectToken(TokenType::Semicolon);

    if (key == "text") {
        element.children.push_back(std::make_unique<TextNode>(value));
    } else {
        element.attributes[key] = value;
    }
}

// Parses a 'style { ... }' block.
void StatementState::parseStyleBlock(Parser& parser, ElementNode& element) {
    parser.expectToken(TokenType::Identifier); // consume 'style'
    parser.expectToken(TokenType::OpenBrace);

    std::string styleString;
    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        // Key
        std::string key = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
        parser.expectToken(TokenType::Colon);

        // Value (parsed as an expression)
        std::string value = parseStyleExpression(parser);
        styleString += key + ": " + value + "; ";

        // Consume optional semicolon after the expression
        if (parser.currentToken.type == TokenType::Semicolon) {
            parser.advanceTokens();
        }
    }

    parser.expectToken(TokenType::CloseBrace);

    if (element.attributes.count("style")) {
        element.attributes["style"] += styleString;
    } else {
        element.attributes["style"] = styleString;
    }
}

// --- Style Expression Parsing Methods ---

// Parses a primary expression, which is either a number or a parenthesized expression.
std::pair<double, std::string> StatementState::parsePrimaryExpr(Parser& parser) {
    if (parser.currentToken.type == TokenType::Number) {
        std::string rawValue = parser.currentToken.value;
        parser.advanceTokens();
        size_t unit_pos = rawValue.find_first_not_of("-.0123456789");
        double value = std::stod(rawValue.substr(0, unit_pos));
        std::string unit = (unit_pos != std::string::npos) ? rawValue.substr(unit_pos) : "";
        return {value, unit};
    } else if (parser.currentToken.type == TokenType::OpenParen) {
        parser.advanceTokens(); // consume '('
        auto result = parseAdditiveExpr(parser);
        parser.expectToken(TokenType::CloseParen); // consume ')'
        return result;
    }
    throw std::runtime_error("Unexpected token in expression: " + parser.currentToken.value);
}

// Parses multiplicative expressions (*, /).
std::pair<double, std::string> StatementState::parseMultiplicativeExpr(Parser& parser) {
    auto result = parsePrimaryExpr(parser);
    double& leftVal = result.first;
    std::string& unit = result.second;

    while (parser.currentToken.type == TokenType::Asterisk || parser.currentToken.type == TokenType::Slash) {
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        auto rhs = parsePrimaryExpr(parser);

        // For multiplication/division, one operand must be unit-less.
        if (!unit.empty() && !rhs.second.empty()) {
            throw std::runtime_error("Cannot multiply or divide two values that both have units.");
        }

        if (op == TokenType::Asterisk) {
            leftVal *= rhs.first;
        } else {
            if (rhs.first == 0) throw std::runtime_error("Division by zero in style expression.");
            leftVal /= rhs.first;
        }
        // The result inherits the unit from whichever operand had one.
        if (unit.empty()) {
            unit = rhs.second;
        }
    }
    return result;
}

// Parses additive expressions (+, -).
std::pair<double, std::string> StatementState::parseAdditiveExpr(Parser& parser) {
    auto result = parseMultiplicativeExpr(parser);
    double& leftVal = result.first;
    std::string& unit = result.second;

    while (parser.currentToken.type == TokenType::Plus || parser.currentToken.type == TokenType::Minus) {
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        auto rhs = parseMultiplicativeExpr(parser);

        // For addition/subtraction, units must match.
        if (unit != rhs.second) {
            throw std::runtime_error("Mismatched units for addition/subtraction: '" + unit + "' and '" + rhs.second + "'.");
        }

        if (op == TokenType::Plus) {
            leftVal += rhs.first;
        } else {
            leftVal -= rhs.first;
        }
    }
    return result;
}

// Entry point for parsing a style value expression.
std::string StatementState::parseStyleExpression(Parser& parser) {
    // Start by parsing the highest-precedence operations first.
    auto final_result = parseAdditiveExpr(parser);

    std::ostringstream oss;
    oss << final_result.first;
    // Remove trailing zeros from the string representation of the number.
    std::string numStr = oss.str();
    numStr.erase(numStr.find_last_not_of('0') + 1, std::string::npos);
    if (numStr.back() == '.') {
        numStr.pop_back();
    }

    return numStr + final_result.second;
}
