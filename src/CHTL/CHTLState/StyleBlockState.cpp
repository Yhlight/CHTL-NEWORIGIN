#include "StyleBlockState.h"

#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleValue.h" // Include the new StyleValue struct
#include "../Util/StyleValueUtil.h" // Include the new util
#include <stdexcept>
#include <sstream>
#include <cmath> // For fmod
#include <iostream>

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


void StyleBlockState::parseInlineProperty(Parser& parser) {
    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::Colon);

    // Save state for potential backtracking
    ParserStateSnapshot snapshot = parser.saveState();

    try {
        StyleValue sv = parseStyleExpression(parser);
        if (parser.currentToken.type == TokenType::Semicolon) {
            // Successfully parsed a full expression.
            parser.contextNode->inlineStyles[key] = sv;
            parser.advanceTokens(); // consume semicolon
            return;
        }
    } catch (const std::runtime_error&) {
        // This catch block is intentionally empty.
        // Failure to parse as an expression is not an error, just a signal
        // to try parsing as a simple value string instead.
    }

    // --- Backtrack and Parse as Simple Value ---
    parser.restoreState(snapshot);

    // Parse as a simple, multi-token string value.
    std::string simpleValue;
    while (parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::CloseBrace) {
        simpleValue += parser.currentToken.value;
        if (parser.peekToken.type != TokenType::Semicolon && parser.peekToken.type != TokenType::CloseBrace) {
            simpleValue += " ";
        }
        parser.advanceTokens();
    }

    StyleValue sv;
    sv.type = StyleValue::STRING;
    sv.string_val = simpleValue;
    parser.contextNode->inlineStyles[key] = sv;

    parser.expectToken(TokenType::Semicolon);
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

// --- New Expression Parser with Operator Precedence ---

// Primary Expression: Numbers, strings, parenthesized expressions, or property lookups
StyleValue StyleBlockState::parsePrimaryExpr(Parser& parser) {
    if (parser.currentToken.type == TokenType::Number) {
        std::string rawValue = parser.currentToken.value;
        parser.advanceTokens();
        size_t unit_pos = rawValue.find_first_not_of("-.0123456789");
        double value = std::stod(rawValue.substr(0, unit_pos));
        std::string unit = (unit_pos != std::string::npos) ? rawValue.substr(unit_pos) : "";
        StyleValue sv;
        sv.type = StyleValue::NUMERIC;
        sv.numeric_val = value;
        sv.unit = unit;
        return sv;
    }

    if (parser.currentToken.type == TokenType::OpenParen) {
        parser.advanceTokens();
        auto result = parseStyleExpression(parser); // Recurse to the top of the precedence chain
        parser.expectToken(TokenType::CloseParen);
        return result;
    }

    if (parser.currentToken.type == TokenType::Identifier) {
        // Check if it's a variable template usage like `Theme(color)`
        if (parser.peekToken.type == TokenType::OpenParen) {
            // This is a variable template usage, e.g., MyTheme(primaryColor)
            std::string templateName = parser.currentToken.value;
            parser.advanceTokens(); // consume template name
            parser.expectToken(TokenType::OpenParen);
            std::string varName = parser.currentToken.value;
            parser.expectToken(TokenType::Identifier);
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

            StyleValue sv;
            sv.type = StyleValue::STRING;
            sv.string_val = it->second;
            return sv;
        }

        // Check if it's a property of the current element
        if (parser.contextNode && parser.contextNode->inlineStyles.count(parser.currentToken.value)) {
            std::string propName = parser.currentToken.value;
            parser.advanceTokens();
            return parser.contextNode->inlineStyles.at(propName);
        }

        // Otherwise, it's a plain string literal like 'solid' or 'red'
        std::string val = parser.currentToken.value;
        parser.advanceTokens();
        StyleValue sv;
        sv.type = StyleValue::STRING;
        sv.string_val = val;
        return sv;
    }

    if (parser.currentToken.type == TokenType::String) {
        std::string val = parser.currentToken.value;
        parser.advanceTokens();
        StyleValue sv;
        sv.type = StyleValue::STRING;
        sv.string_val = val;
        return sv;
    }

    throw std::runtime_error("Unexpected token in expression: " + parser.currentToken.value);
}

// Multiplicative: *, /, %
StyleValue StyleBlockState::parseMultiplicativeExpr(Parser& parser) {
    StyleValue result = parsePrimaryExpr(parser);
    while (parser.currentToken.type == TokenType::Asterisk || parser.currentToken.type == TokenType::Slash || parser.currentToken.type == TokenType::Percent) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("Left-hand side of *, /, or % must be numeric.");
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parsePrimaryExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("Right-hand side of *, /, or % must be numeric.");

        if (op == TokenType::Asterisk) result.numeric_val *= rhs.numeric_val;
        else if (op == TokenType::Slash) {
            if (rhs.numeric_val == 0) throw std::runtime_error("Division by zero.");
            result.numeric_val /= rhs.numeric_val;
        } else { // Percent
            result.numeric_val = fmod(result.numeric_val, rhs.numeric_val);
        }
    }
    return result;
}

// Additive: +, -
StyleValue StyleBlockState::parseAdditiveExpr(Parser& parser) {
    StyleValue result = parseMultiplicativeExpr(parser);
    while (parser.currentToken.type == TokenType::Plus || parser.currentToken.type == TokenType::Minus) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("Left-hand side of + or - must be numeric.");
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parseMultiplicativeExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("Right-hand side of + or - must be numeric.");
        if (result.unit != rhs.unit) throw std::runtime_error("Mismatched units for + or - operation.");

        if (op == TokenType::Plus) result.numeric_val += rhs.numeric_val;
        else result.numeric_val -= rhs.numeric_val;
    }
    return result;
}

// Comparison: >, <, >=, <=
StyleValue StyleBlockState::parseComparisonExpr(Parser& parser) {
    StyleValue result = parseAdditiveExpr(parser);
    while (parser.currentToken.type == TokenType::GreaterThan || parser.currentToken.type == TokenType::LessThan ||
           parser.currentToken.type == TokenType::GreaterThanEquals || parser.currentToken.type == TokenType::LessThanEquals) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("Left-hand side of comparison must be numeric.");
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parseAdditiveExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("Right-hand side of comparison must be numeric.");
        if (result.unit != rhs.unit) throw std::runtime_error("Mismatched units for comparison.");

        bool bool_res = false;
        if (op == TokenType::GreaterThan) bool_res = result.numeric_val > rhs.numeric_val;
        else if (op == TokenType::LessThan) bool_res = result.numeric_val < rhs.numeric_val;
        else if (op == TokenType::GreaterThanEquals) bool_res = result.numeric_val >= rhs.numeric_val;
        else if (op == TokenType::LessThanEquals) bool_res = result.numeric_val <= rhs.numeric_val;

        result.type = StyleValue::BOOL;
        result.bool_val = bool_res;
    }
    return result;
}

// Equality: ==, !=
StyleValue StyleBlockState::parseEqualityExpr(Parser& parser) {
    StyleValue result = parseComparisonExpr(parser);
    while (parser.currentToken.type == TokenType::EqualsEquals || parser.currentToken.type == TokenType::NotEquals) {
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parseComparisonExpr(parser);

        bool bool_res = false;
        if (result.type != rhs.type) {
             bool_res = (op == TokenType::NotEquals);
        } else if (result.type == StyleValue::NUMERIC) {
            bool_res = (op == TokenType::EqualsEquals) ? (result.numeric_val == rhs.numeric_val) : (result.numeric_val != rhs.numeric_val);
        } else if (result.type == StyleValue::STRING) {
            bool_res = (op == TokenType::EqualsEquals) ? (result.string_val == rhs.string_val) : (result.string_val != rhs.string_val);
        } else {
            throw std::runtime_error("Invalid operands for equality check.");
        }

        result.type = StyleValue::BOOL;
        result.bool_val = bool_res;
    }
    return result;
}

// Logical AND: &&
StyleValue StyleBlockState::parseLogicalAndExpr(Parser& parser) {
    StyleValue result = parseEqualityExpr(parser);
    while (parser.currentToken.type == TokenType::LogicalAnd) {
        if (result.type != StyleValue::BOOL) throw std::runtime_error("Left-hand side of && must be a boolean expression.");
        parser.advanceTokens();
        StyleValue rhs = parseEqualityExpr(parser);
        if (rhs.type != StyleValue::BOOL) throw std::runtime_error("Right-hand side of && must be a boolean expression.");
        result.bool_val = result.bool_val && rhs.bool_val;
    }
    return result;
}

// Logical OR: ||
StyleValue StyleBlockState::parseLogicalOrExpr(Parser& parser) {
    StyleValue result = parseLogicalAndExpr(parser);
    while (parser.currentToken.type == TokenType::LogicalOr) {
        if (result.type != StyleValue::BOOL) throw std::runtime_error("Left-hand side of || must be a boolean expression.");
        parser.advanceTokens();
        StyleValue rhs = parseLogicalAndExpr(parser);
        if (rhs.type != StyleValue::BOOL) throw std::runtime_error("Right-hand side of || must be a boolean expression.");
        result.bool_val = result.bool_val || rhs.bool_val;
    }
    return result;
}

// Ternary: ? :
StyleValue StyleBlockState::parseTernaryExpr(Parser& parser) {
    StyleValue condition = parseLogicalOrExpr(parser);
    if (parser.currentToken.type == TokenType::QuestionMark) {
        if (condition.type != StyleValue::BOOL) throw std::runtime_error("Condition for ternary operator must be a boolean expression.");
        parser.advanceTokens(); // consume '?'
        StyleValue true_expr = parseTernaryExpr(parser);
        parser.expectToken(TokenType::Colon);
        StyleValue false_expr = parseTernaryExpr(parser);
        return condition.bool_val ? true_expr : false_expr;
    }
    return condition;
}

// Entry point for expression parsing
StyleValue StyleBlockState::parseStyleExpression(Parser& parser) {
    return parseTernaryExpr(parser);
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
