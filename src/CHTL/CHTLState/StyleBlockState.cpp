#include "StyleBlockState.h"

#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include <stdexcept>
#include <sstream>

// The main handler for the style block. It dispatches to helpers based on the token.
std::unique_ptr<BaseNode> StyleBlockState::handle(Parser& parser) {
    if (!parser.contextNode) {
        throw std::runtime_error("StyleBlockState requires a context node.");
    }

    parser.expectToken(TokenType::Identifier); // consume 'style'
    parser.expectToken(TokenType::OpenBrace);

    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::Dot || parser.currentToken.type == TokenType::Hash) {
            parseClassOrIdSelector(parser);
        } else if (parser.currentToken.type == TokenType::Ampersand) {
            parseAmpersandSelector(parser);
        } else if (parser.currentToken.type == TokenType::Identifier) {
            parseInlineProperty(parser);
        } else {
            throw std::runtime_error("Unexpected token in style block: " + parser.currentToken.value);
        }
    }

    parser.expectToken(TokenType::CloseBrace);
    return nullptr;
}

// Parses an inline property like 'color: red;'.
void StyleBlockState::parseInlineProperty(Parser& parser) {
    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::Colon);
    std::string value = parseStyleExpression(parser);

    if (parser.contextNode->attributes.count("style")) {
        parser.contextNode->attributes["style"] += key + ": " + value + "; ";
    } else {
        parser.contextNode->attributes["style"] = key + ": " + value + "; ";
    }

    if (parser.currentToken.type == TokenType::Semicolon) {
        parser.advanceTokens();
    }
}

// Parses a class or ID selector block, e.g., '.box { ... }'.
void StyleBlockState::parseClassOrIdSelector(Parser& parser) {
    std::string selector;
    std::string selectorName;

    if (parser.currentToken.type == TokenType::Dot) {
        selector += ".";
        parser.advanceTokens();
        if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected identifier after '.' for class selector.");
        selectorName = parser.currentToken.value;
        parser.contextNode->attributes["class"] = selectorName;
    } else if (parser.currentToken.type == TokenType::Hash) {
        selector += "#";
        parser.advanceTokens();
        if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected identifier after '#' for ID selector.");
        selectorName = parser.currentToken.value;
        parser.contextNode->attributes["id"] = selectorName;
    }
    selector += selectorName;
    parser.advanceTokens();

    std::string cssRules = parseCssRuleBlock(parser);
    parser.globalStyleContent += selector + " {\n" + cssRules + "}\n\n";
}

// Parses a selector starting with '&', e.g., '&:hover { ... }'.
void StyleBlockState::parseAmpersandSelector(Parser& parser) {
    std::string baseSelector;
    if (parser.contextNode->attributes.count("class")) {
        baseSelector = "." + parser.contextNode->attributes["class"];
    } else if (parser.contextNode->attributes.count("id")) {
        baseSelector = "#" + parser.contextNode->attributes["id"];
    } else {
        throw std::runtime_error("Cannot use '&' selector because the parent element has no class or id defined.");
    }

    parser.advanceTokens(); // consume '&'

    std::string selectorSuffix;
    while(parser.currentToken.type != TokenType::OpenBrace && parser.currentToken.type != TokenType::EndOfFile) {
        selectorSuffix += parser.currentToken.value;
        parser.advanceTokens();
    }

    std::string finalSelector = baseSelector + selectorSuffix;
    std::string cssRules = parseCssRuleBlock(parser);
    parser.globalStyleContent += finalSelector + " {\n" + cssRules + "}\n\n";
}

// Helper to parse the body of a CSS rule block '{...}'.
std::string StyleBlockState::parseCssRuleBlock(Parser& parser) {
    parser.expectToken(TokenType::OpenBrace);
    std::string cssRules;
    while(parser.currentToken.type != TokenType::CloseBrace) {
        if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected property name inside selector block.");
        std::string key = parser.currentToken.value;
        parser.advanceTokens();
        parser.expectToken(TokenType::Colon);
        std::string value = parseStyleExpression(parser);
        cssRules += "  " + key + ": " + value + ";\n";
        if(parser.currentToken.type == TokenType::Semicolon) parser.advanceTokens();
    }
    parser.expectToken(TokenType::CloseBrace);
    return cssRules;
}

// --- Style Expression Parsing Methods ---
// (Unchanged from previous step)

std::pair<double, std::string> StyleBlockState::parsePrimaryExpr(Parser& parser) {
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

std::pair<double, std::string> StyleBlockState::parseMultiplicativeExpr(Parser& parser) {
    auto result = parsePrimaryExpr(parser);
    double& leftVal = result.first;
    std::string& unit = result.second;

    while (parser.currentToken.type == TokenType::Asterisk || parser.currentToken.type == TokenType::Slash) {
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        auto rhs = parsePrimaryExpr(parser);

        if (!unit.empty() && !rhs.second.empty()) {
            throw std::runtime_error("Cannot multiply or divide two values that both have units.");
        }

        if (op == TokenType::Asterisk) {
            leftVal *= rhs.first;
        } else {
            if (rhs.first == 0) throw std::runtime_error("Division by zero in style expression.");
            leftVal /= rhs.first;
        }

        if (unit.empty()) {
            unit = rhs.second;
        }
    }
    return result;
}

std::pair<double, std::string> StyleBlockState::parseAdditiveExpr(Parser& parser) {
    auto result = parseMultiplicativeExpr(parser);
    double& leftVal = result.first;
    std::string& unit = result.second;

    while (parser.currentToken.type == TokenType::Plus || parser.currentToken.type == TokenType::Minus) {
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        auto rhs = parseMultiplicativeExpr(parser);

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

std::string StyleBlockState::parseStyleExpression(Parser& parser) {
    auto final_result = parseAdditiveExpr(parser);

    std::ostringstream oss;
    oss << final_result.first;
    std::string numStr = oss.str();
    size_t last_not_zero = numStr.find_last_not_of('0');
    if (std::string::npos != last_not_zero) {
        numStr.erase(last_not_zero + 1);
    }
    if (!numStr.empty() && numStr.back() == '.') {
        numStr.pop_back();
    }

    return numStr + final_result.second;
}
