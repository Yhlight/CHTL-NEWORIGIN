#include "StyleUtil.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "ASTUtil.h"
#include <sstream>
#include <stdexcept>

// Forward declarations for the recursive descent parser
StyleValue parseStyleExpression(Parser& parser);
StyleValue parseConditionalExpr(Parser& parser);
StyleValue parseDynamicConditionalExpression(Parser& parser);

std::string styleValueToString(const StyleValue& sv) {
    switch (sv.type) {
        case StyleValue::NUMERIC: {
            std::ostringstream oss;
            oss.precision(15); // Preserve precision
            oss << sv.numeric_val;
            std::string numStr = oss.str();
            if (numStr.find('.') != std::string::npos) {
                numStr.erase(numStr.find_last_not_of('0') + 1, std::string::npos);
                if (numStr.back() == '.') {
                    numStr.pop_back();
                }
            }
            return numStr + sv.unit;
        }
        case StyleValue::STRING:
            return sv.string_val;
        case StyleValue::EMPTY:
            return "";
        case StyleValue::BOOL:
        case StyleValue::DYNAMIC_CONDITIONAL:
             throw std::runtime_error("Cannot convert a complex expression result to a string style value directly.");
    }
    return ""; // Should not be reached
}


// --- Start of Expression Parsing Implementation ---

StyleValue parseDynamicConditionalExpression(Parser& parser) {
    auto expr = std::make_shared<DynamicConditionalExpression>();

    parser.expectToken(TokenType::OpenDoubleBrace);
    if(parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected identifier for selector in dynamic expression");
    expr->selector = parser.currentToken.value;
    parser.advanceTokens();
    parser.expectToken(TokenType::CloseDoubleBrace);

    parser.expectToken(TokenType::RightArrow);

    if(parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected property name in dynamic expression");
    expr->property = parser.currentToken.value;
    parser.advanceTokens();

    // Parse operator
    const auto& opToken = parser.currentToken;
    if (opToken.type == TokenType::GreaterThan || opToken.type == TokenType::LessThan || opToken.type == TokenType::EqualsEquals ||
        opToken.type == TokenType::NotEquals || opToken.type == TokenType::GreaterThanEquals || opToken.type == TokenType::LessThanEquals) {
        expr->op = opToken.value;
        parser.advanceTokens();
    } else {
        throw std::runtime_error("Expected a comparison operator in dynamic expression.");
    }

    // Parse value to compare
    if(parser.currentToken.type != TokenType::Number) throw std::runtime_error("Expected number for comparison in dynamic expression");
    expr->value_to_compare = std::stod(parser.currentToken.value);
    parser.advanceTokens();

    parser.expectToken(TokenType::QuestionMark);

    // Parse true and false branches.
    expr->true_branch = std::make_unique<StyleValue>(parseStyleExpression(parser));
    parser.expectToken(TokenType::Colon);
    expr->false_branch = std::make_unique<StyleValue>(parseStyleExpression(parser));

    StyleValue result;
    result.type = StyleValue::DYNAMIC_CONDITIONAL;
    result.dynamic_expr = expr;
    return result;
}


Selector parseSelector(Parser& parser) {
    Selector selector;
    selector.parts.emplace_back(); // Start with the first part

    while (true) {
        SelectorPart& currentPart = selector.parts.back();

        if (parser.currentToken.type == TokenType::Dot) {
            parser.advanceTokens(); // consume '.'
            if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected class name after '.'.");
            currentPart.className = parser.currentToken.value;
            parser.advanceTokens();
        } else if (parser.currentToken.type == TokenType::Hash) {
            parser.advanceTokens(); // consume '#'
            if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected id name after '#'.");
            currentPart.id = parser.currentToken.value;
            parser.advanceTokens();
        } else if (parser.currentToken.type == TokenType::Identifier) {
            currentPart.tagName = parser.currentToken.value;
            parser.advanceTokens();
        }

        if (parser.currentToken.type == TokenType::OpenBracket) {
            parser.advanceTokens(); // consume '['
            if (parser.currentToken.type != TokenType::Number) throw std::runtime_error("Expected number inside index '[]'.");
            currentPart.index = std::stoi(parser.currentToken.value);
            parser.advanceTokens();
            parser.expectToken(TokenType::CloseBracket);
        }

        if (parser.currentToken.type == TokenType::Identifier || parser.currentToken.type == TokenType::Dot || parser.currentToken.type == TokenType::Hash) {
            if(parser.currentToken.type == TokenType::Dot && parser.peekToken.type == TokenType::Identifier && parser.peekToken2.type != TokenType::OpenBracket) {
                break;
            }
            selector.parts.emplace_back();
        } else {
            break;
        }
    }
    return selector;
}

StyleValue parseReferencedProperty(Parser& parser) {
    Selector selector = parseSelector(parser);

    parser.expectToken(TokenType::Dot);
    std::string propertyName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    if (!parser.parsedNodes) {
        throw std::runtime_error("Parser context does not have access to parsed nodes for lookup.");
    }
    const ElementNode* referencedNode = findNodeBySelector(*parser.parsedNodes, selector);
    if (!referencedNode) {
        throw std::runtime_error("Could not find element for property reference.");
    }
    auto it = referencedNode->inlineStyles.find(propertyName);
    if (it == referencedNode->inlineStyles.end()) {
        throw std::runtime_error("Property '" + propertyName + "' not found on referenced element.");
    }
    return it->second;
}

StyleValue parsePrimaryExpr(Parser& parser) {
    if (parser.currentToken.type == TokenType::Identifier) {
        auto it = parser.contextNode->inlineStyles.find(parser.currentToken.value);
        if (it != parser.contextNode->inlineStyles.end()) {
            parser.advanceTokens();
            return it->second;
        }
    }

    if (parser.currentToken.type == TokenType::Dot || parser.currentToken.type == TokenType::Hash) {
        return parseReferencedProperty(parser);
    }
    if (parser.currentToken.type == TokenType::Identifier && parser.peekToken.type == TokenType::Dot) {
        if (parser.contextNode->inlineStyles.find(parser.currentToken.value) == parser.contextNode->inlineStyles.end()) {
            return parseReferencedProperty(parser);
        }
    }

    if (parser.currentToken.type == TokenType::Number) {
        std::string rawValue = parser.currentToken.value;
        parser.advanceTokens();
        size_t unit_pos = rawValue.find_first_not_of("-.0123456789");
        double value = std::stod(rawValue.substr(0, unit_pos));
        std::string unit = (unit_pos != std::string::npos) ? rawValue.substr(unit_pos) : "";
        return StyleValue(value, unit);
    }

    if (parser.currentToken.type == TokenType::OpenParen) {
        parser.advanceTokens();
        auto result = parseStyleExpression(parser);
        parser.expectToken(TokenType::CloseParen);
        return result;
    }

    if (parser.currentToken.type == TokenType::Identifier && parser.peekToken.type == TokenType::OpenParen) {
        std::string templateName = parser.currentToken.value;
        parser.advanceTokens();
        parser.expectToken(TokenType::OpenParen);

        std::string varName = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);

        std::string specializedValue;
        bool isSpecialized = false;
        if (parser.currentToken.type == TokenType::Equals) {
            parser.advanceTokens();

            std::stringstream ss;
            bool first = true;
            while(parser.currentToken.type != TokenType::CloseParen && parser.currentToken.type != TokenType::EndOfFile) {
                if (!first) ss << " ";
                ss << parser.currentToken.value;
                parser.advanceTokens();
                first = false;
            }

            specializedValue = ss.str();
            isSpecialized = true;
        }

        parser.expectToken(TokenType::CloseParen);

        std::string ns = parser.getCurrentNamespace();
        if (parser.currentToken.type == TokenType::From) {
            parser.advanceTokens();

            std::stringstream ns_builder;
            ns_builder << parser.currentToken.value;
            parser.expectToken(TokenType::Identifier);

            while(parser.currentToken.type == TokenType::Dot) {
                parser.advanceTokens();
                ns_builder << "." << parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
            }
            ns = ns_builder.str();
        }

        if (isSpecialized) {
            return StyleValue(specializedValue);
        }

        VarTemplateNode* varTmpl = parser.templateManager.getVarTemplate(ns, templateName);
        if (!varTmpl) throw std::runtime_error("Variable template '" + templateName + "' not found in namespace '" + ns + "'.");

        auto it = varTmpl->variables.find(varName);
        if (it == varTmpl->variables.end()) throw std::runtime_error("Variable '" + varName + "' not found in template '" + templateName + "'.");

        return StyleValue(it->second);
    }

    if (parser.currentToken.type == TokenType::Identifier || parser.currentToken.type == TokenType::String) {
        std::stringstream ss;
        ss << parser.currentToken.value;
        parser.advanceTokens();

        while (parser.currentToken.type == TokenType::Identifier || parser.currentToken.type == TokenType::Number || parser.currentToken.type == TokenType::String) {
             if (parser.currentToken.type == TokenType::Semicolon || parser.currentToken.type == TokenType::CloseBrace) {
                 break;
             }
             ss << " " << parser.currentToken.value;
             parser.advanceTokens();
        }

        return StyleValue(ss.str());
    }

    throw std::runtime_error("Unexpected token in expression: " + parser.currentToken.value);
}

StyleValue parseMultiplicativeExpr(Parser& parser) {
    StyleValue result = parsePrimaryExpr(parser);
    while (parser.currentToken.type == TokenType::Asterisk || parser.currentToken.type == TokenType::Slash) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("LHS of * or / must be numeric.");
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parsePrimaryExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("RHS of * or / must be numeric.");

        if (!result.unit.empty() && !rhs.unit.empty()) {
            throw std::runtime_error("Cannot multiply or divide two values with units.");
        }

        if (op == TokenType::Asterisk) {
            result.numeric_val *= rhs.numeric_val;
        } else {
            if (rhs.numeric_val == 0) throw std::runtime_error("Division by zero.");
            result.numeric_val /= rhs.numeric_val;
        }

        if (result.unit.empty()) {
            result.unit = rhs.unit;
        }
    }
    return result;
}

StyleValue parseAdditiveExpr(Parser& parser) {
    StyleValue result = parseMultiplicativeExpr(parser);
    while (parser.currentToken.type == TokenType::Plus || parser.currentToken.type == TokenType::Minus) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("LHS of + or - must be numeric.");
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parseMultiplicativeExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("RHS of + or - must be numeric.");

        if (result.unit != rhs.unit && !result.unit.empty() && !rhs.unit.empty()) {
            std::string lhs_str = styleValueToString(result);
            std::string rhs_str = styleValueToString(rhs);
            std::string op_str = (op == TokenType::Plus) ? " + " : " - ";
            result = StyleValue("calc(" + lhs_str + op_str + rhs_str + ")");
        } else {
            if (op == TokenType::Plus) result.numeric_val += rhs.numeric_val;
            else result.numeric_val -= rhs.numeric_val;
            if (result.unit.empty()) {
                result.unit = rhs.unit;
            }
        }
    }
    return result;
}

StyleValue parseBooleanRelationalExpr(Parser& parser) {
    StyleValue lhs = parseAdditiveExpr(parser);

    if (parser.currentToken.type == TokenType::GreaterThan || parser.currentToken.type == TokenType::LessThan ||
        parser.currentToken.type == TokenType::EqualsEquals || parser.currentToken.type == TokenType::NotEquals ||
        parser.currentToken.type == TokenType::GreaterThanEquals || parser.currentToken.type == TokenType::LessThanEquals)
    {
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parseAdditiveExpr(parser);

        StyleValue bool_result;
        bool_result.type = StyleValue::BOOL;
        if (lhs.type == StyleValue::NUMERIC && rhs.type == StyleValue::NUMERIC) {
             if (lhs.unit != rhs.unit) throw std::runtime_error("Cannot compare different units.");
             switch(op) {
                case TokenType::GreaterThan: bool_result.bool_val = lhs.numeric_val > rhs.numeric_val; break;
                case TokenType::LessThan: bool_result.bool_val = lhs.numeric_val < rhs.numeric_val; break;
                case TokenType::EqualsEquals: bool_result.bool_val = lhs.numeric_val == rhs.numeric_val; break;
                case TokenType::NotEquals: bool_result.bool_val = lhs.numeric_val != rhs.numeric_val; break;
                case TokenType::GreaterThanEquals: bool_result.bool_val = lhs.numeric_val >= rhs.numeric_val; break;
                case TokenType::LessThanEquals: bool_result.bool_val = lhs.numeric_val <= rhs.numeric_val; break;
                default: bool_result.bool_val = false;
             }
        } else if (lhs.type == StyleValue::STRING && rhs.type == StyleValue::STRING) {
            if (op == TokenType::EqualsEquals) bool_result.bool_val = lhs.string_val == rhs.string_val;
            else if (op == TokenType::NotEquals) bool_result.bool_val = lhs.string_val != rhs.string_val;
            else throw std::runtime_error("Invalid operator for string comparison.");
        } else {
            throw std::runtime_error("Invalid types for comparison.");
        }
        return bool_result;
    }
    return lhs;
}

StyleValue parseBooleanAndExpr(Parser& parser) {
    StyleValue result = parseBooleanRelationalExpr(parser);
    while (parser.currentToken.type == TokenType::LogicalAnd) {
        if (result.type != StyleValue::BOOL) throw std::runtime_error("LHS of '&&' must be a boolean expression.");
        parser.advanceTokens();
        StyleValue rhs = parseBooleanRelationalExpr(parser);
        if (rhs.type != StyleValue::BOOL) throw std::runtime_error("RHS of '&&' must be a boolean expression.");
        result.bool_val = result.bool_val && rhs.bool_val;
    }
    return result;
}

StyleValue parseBooleanOrExpr(Parser& parser) {
    StyleValue result = parseBooleanAndExpr(parser);
    while (parser.currentToken.type == TokenType::LogicalOr) {
        if (result.type != StyleValue::BOOL) throw std::runtime_error("LHS of '||' must be a boolean expression.");
        parser.advanceTokens();
        StyleValue rhs = parseBooleanAndExpr(parser);
        if (rhs.type != StyleValue::BOOL) throw std::runtime_error("RHS of '||' must be a boolean expression.");
        result.bool_val = result.bool_val || rhs.bool_val;
    }
    return result;
}

StyleValue parseConditionalExpr(Parser& parser) {
    StyleValue condition = parseBooleanOrExpr(parser);

    if (parser.currentToken.type == TokenType::QuestionMark) {
        if (condition.type != StyleValue::BOOL) {
            throw std::runtime_error("Expression before '?' must evaluate to a boolean.");
        }
        parser.advanceTokens();

        StyleValue true_val = parseConditionalExpr(parser);

        if (parser.currentToken.type == TokenType::Colon) {
            parser.advanceTokens();
            StyleValue false_val = parseConditionalExpr(parser);
            return condition.bool_val ? true_val : false_val;
        }

        return condition.bool_val ? true_val : StyleValue(StyleValue::EMPTY);
    }

    return condition;
}

StyleValue parseStyleExpression(Parser& parser) {
    // Check for dynamic conditional at the start of the expression
    if (parser.currentToken.type == TokenType::OpenDoubleBrace) {
        return parseDynamicConditionalExpression(parser);
    }

    StyleValue finalValue(StyleValue::EMPTY);
    bool conditionMet = false;

    while (true) {
        StyleValue result = parseConditionalExpr(parser);

        if (!conditionMet) {
            if (result.type != StyleValue::EMPTY) {
                finalValue = result;
                if (result.type != StyleValue::NUMERIC && result.type != StyleValue::STRING) {
                     conditionMet = true;
                }
            }
        }

        if (parser.currentToken.type == TokenType::Comma) {
            parser.advanceTokens();
            if (conditionMet) {
                parseStyleExpression(parser);
            }
        } else {
            break;
        }
    }
    return finalValue;
}