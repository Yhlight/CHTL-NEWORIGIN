#include "StyleBlockState.h"

#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleValue.h" // Include the new StyleValue struct
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
        if (parser.currentToken.type == TokenType::At) {
            parseStyleTemplateUsage(parser);
        } else if (parser.currentToken.type == TokenType::Dot || parser.currentToken.type == TokenType::Hash) {
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

// Converts a StyleValue object to its final string representation for CSS.
std::string styleValueToString(const StyleValue& sv) {
    if (sv.type == StyleValue::NUMERIC) {
        std::ostringstream oss;
        oss << sv.numeric_val;
        std::string numStr = oss.str();
        // Clean up trailing zeros
        size_t last_not_zero = numStr.find_last_not_of('0');
        if (std::string::npos != last_not_zero) {
            numStr.erase(last_not_zero + 1);
        }
        if (!numStr.empty() && numStr.back() == '.') {
            numStr.pop_back();
        }
        return numStr + sv.unit;
    }
    return sv.string_val;
}

// Parses an inline property like 'color: red;'.
void StyleBlockState::parseInlineProperty(Parser& parser) {
    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::Colon);

    StyleValue sv = parseStyleExpression(parser);
    std::string value = styleValueToString(sv);

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
        StyleValue sv = parseStyleExpression(parser);
        std::string value = styleValueToString(sv);
        cssRules += "  " + key + ": " + value + ";\n";
        if(parser.currentToken.type == TokenType::Semicolon) parser.advanceTokens();
    }
    parser.expectToken(TokenType::CloseBrace);
    return cssRules;
}

// --- Expression Parser Refactored to use StyleValue ---

StyleValue StyleBlockState::parsePrimaryExpr(Parser& parser) {
    if (parser.currentToken.type == TokenType::Number) {
        std::string rawValue = parser.currentToken.value;
        parser.advanceTokens();
        size_t unit_pos = rawValue.find_first_not_of("-.0123456789");
        double value = std::stod(rawValue.substr(0, unit_pos));
        std::string unit = (unit_pos != std::string::npos) ? rawValue.substr(unit_pos) : "";
        return {StyleValue::NUMERIC, value, unit, ""};
    } else if (parser.currentToken.type == TokenType::OpenParen) {
        parser.advanceTokens(); // consume '('
        auto result = parseAdditiveExpr(parser);
        parser.expectToken(TokenType::CloseParen); // consume ')'
        return result;
    } else if (parser.currentToken.type == TokenType::Identifier && parser.peekToken.type == TokenType::OpenParen) {
        // This is a variable template usage, e.g., MyTheme(primaryColor)
        std::string templateName = parser.currentToken.value;
        parser.advanceTokens(); // consume template name
        parser.expectToken(TokenType::OpenParen);

        std::string varName = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);

        // Handle optional 'from <namespace>' clause inside the parentheses
        std::string ns = parser.getCurrentNamespace();
        if (parser.currentToken.type == TokenType::From) {
            parser.advanceTokens(); // consume 'from'
            ns = parser.currentToken.value;
            parser.expectToken(TokenType::Identifier);
        }

        parser.expectToken(TokenType::CloseParen);

        VarTemplateNode* varTmpl = parser.templateManager.getVarTemplate(ns, templateName);
        if (!varTmpl) throw std::runtime_error("Variable template not found: " + templateName);

        auto it = varTmpl->variables.find(varName);
        if (it == varTmpl->variables.end()) throw std::runtime_error("Variable '" + varName + "' not found in template '" + templateName + "'.");

        // The value is a simple string replacement.
        return {StyleValue::STRING, 0.0, "", it->second};
    } else if (parser.currentToken.type == TokenType::Identifier || parser.currentToken.type == TokenType::String) {
        // A plain identifier or string (e.g., "solid", "red", "auto")
        std::string val = parser.currentToken.value;
        parser.advanceTokens();
        return {StyleValue::STRING, 0.0, "", val};
    }

    throw std::runtime_error("Unexpected token in expression: " + parser.currentToken.value);
}

StyleValue StyleBlockState::parseMultiplicativeExpr(Parser& parser) {
    StyleValue result = parsePrimaryExpr(parser);

    while (parser.currentToken.type == TokenType::Asterisk || parser.currentToken.type == TokenType::Slash) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("Left-hand side of * or / must be numeric.");

        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parsePrimaryExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("Right-hand side of * or / must be numeric.");

        if (!result.unit.empty() && !rhs.unit.empty()) {
            throw std::runtime_error("Cannot multiply or divide two values that both have units.");
        }

        if (op == TokenType::Asterisk) {
            result.numeric_val *= rhs.numeric_val;
        } else {
            if (rhs.numeric_val == 0) throw std::runtime_error("Division by zero in style expression.");
            result.numeric_val /= rhs.numeric_val;
        }

        if (result.unit.empty()) {
            result.unit = rhs.unit;
        }
    }
    return result;
}

StyleValue StyleBlockState::parseAdditiveExpr(Parser& parser) {
    StyleValue result = parseMultiplicativeExpr(parser);

    while (parser.currentToken.type == TokenType::Plus || parser.currentToken.type == TokenType::Minus) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("Left-hand side of + or - must be numeric.");

        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parseMultiplicativeExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("Right-hand side of + or - must be numeric.");

        if (result.unit != rhs.unit) {
            throw std::runtime_error("Mismatched units for + or - operation: '" + result.unit + "' and '" + rhs.unit + "'.");
        }

        if (op == TokenType::Plus) {
            result.numeric_val += rhs.numeric_val;
        } else {
            result.numeric_val -= rhs.numeric_val;
        }
    }
    return result;
}

StyleValue StyleBlockState::parseStyleExpression(Parser& parser) {
    return parseAdditiveExpr(parser);
}

void StyleBlockState::parseStyleTemplateUsage(Parser& parser) {
    parser.expectToken(TokenType::At);
    if (parser.currentToken.value != "Style") throw std::runtime_error("Expected 'Style' after '@' for template usage.");
    parser.expectToken(TokenType::Identifier); // consume "Style"

    std::string templateName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    std::string ns = parser.getCurrentNamespace();
    if (parser.currentToken.type == TokenType::From) {
        parser.advanceTokens();
        ns = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
    }

    StyleTemplateNode* tmpl = parser.templateManager.getStyleTemplate(ns, templateName);
    if (!tmpl) throw std::runtime_error("Style template not found: " + templateName);

    // Copy the base styles from the template
    std::map<std::string, std::string> finalStyles = tmpl->styles;

    // If the template is custom and a specialization block is provided, parse it.
    if (tmpl->isCustom && parser.currentToken.type == TokenType::OpenBrace) {
        parser.expectToken(TokenType::OpenBrace);
        while (parser.currentToken.type != TokenType::CloseBrace) {
            if (parser.currentToken.type == TokenType::Delete) {
                parser.advanceTokens(); // consume 'delete'
                while(parser.currentToken.type != TokenType::Semicolon) {
                    if (parser.currentToken.type == TokenType::Identifier) {
                        finalStyles.erase(parser.currentToken.value);
                        parser.advanceTokens();
                    }
                    if (parser.currentToken.type == TokenType::Comma) {
                        parser.advanceTokens();
                    }
                }
                parser.expectToken(TokenType::Semicolon);
            } else {
                // Handle property completion
                std::string key = parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
                parser.expectToken(TokenType::Colon);
                StyleValue sv = parseStyleExpression(parser);
                finalStyles[key] = styleValueToString(sv); // Add or overwrite
                if(parser.currentToken.type == TokenType::Semicolon) parser.advanceTokens();
            }
        }
        parser.expectToken(TokenType::CloseBrace);
    } else {
        parser.expectToken(TokenType::Semicolon);
    }

    // Apply the final, specialized styles to the element
    std::string styleString;
    for (const auto& pair : finalStyles) {
        if (!pair.second.empty()) { // Don't add valueless properties that weren't completed
             styleString += pair.first + ": " + pair.second + "; ";
        }
    }

    if (!styleString.empty()) {
        if (parser.contextNode->attributes.count("style")) {
            parser.contextNode->attributes["style"] += styleString;
        } else {
            parser.contextNode->attributes["style"] = styleString;
        }
    }
}
