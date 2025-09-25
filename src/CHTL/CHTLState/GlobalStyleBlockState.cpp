#include "GlobalStyleBlockState.h"
#include "../CHTLParser/Parser.h"
#include "../Util/StyleUtil.h"
#include "../CHTLNode/ElementNode.h"
#include <stdexcept>
#include <sstream>

std::unique_ptr<BaseNode> GlobalStyleBlockState::handle(Parser& parser) {
    parser.expectToken(TokenType::Identifier); // consume 'style'
    parser.expectToken(TokenType::OpenBrace);

    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::Identifier || parser.currentToken.type == TokenType::Dot || parser.currentToken.type == TokenType::Hash) {
            std::stringstream selector;
            while(parser.currentToken.type != TokenType::OpenBrace && parser.currentToken.type != TokenType::EndOfFile) {
                selector << parser.currentToken.value;
                parser.advanceTokens();
            }

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
            parser.globalStyleContent += selector.str() + " {\n" + cssRules + "}\n\n";

        } else {
            throw std::runtime_error("Unexpected token in global style block: " + parser.currentToken.value);
        }
    }

    parser.expectToken(TokenType::CloseBrace);
    return nullptr;
}

// --- Expression Parsing Methods (adapted from StyleBlockState) ---

StyleValue GlobalStyleBlockState::parseStyleExpression(Parser& parser) {
    return parseConditionalExpr(parser);
}

StyleValue GlobalStyleBlockState::parseConditionalExpr(Parser& parser) {
    StyleValue condition = parseBooleanOrExpr(parser);

    if (parser.currentToken.type == TokenType::QuestionMark) {
        if (condition.type != StyleValue::BOOL) {
            throw std::runtime_error("Expression before '?' must evaluate to a boolean.");
        }
        parser.advanceTokens(); // consume '?'

        StyleValue true_val = parseConditionalExpr(parser);

        if (parser.currentToken.type == TokenType::Colon) {
            parser.advanceTokens(); // consume ':'
            StyleValue false_val = parseConditionalExpr(parser);
            return condition.bool_val ? true_val : false_val;
        }

        return condition.bool_val ? true_val : StyleValue{StyleValue::EMPTY};
    }

    return condition;
}

StyleValue GlobalStyleBlockState::parseBooleanOrExpr(Parser& parser) {
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

StyleValue GlobalStyleBlockState::parseBooleanAndExpr(Parser& parser) {
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

StyleValue GlobalStyleBlockState::parseBooleanRelationalExpr(Parser& parser) {
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

StyleValue GlobalStyleBlockState::parseAdditiveExpr(Parser& parser) {
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
            result = {StyleValue::STRING, 0.0, "", "calc(" + lhs_str + op_str + rhs_str + ")"};
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

StyleValue GlobalStyleBlockState::parseMultiplicativeExpr(Parser& parser) {
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

StyleValue GlobalStyleBlockState::parsePrimaryExpr(Parser& parser) {
    if (parser.currentToken.type == TokenType::Identifier) {
        // Self-referential properties are not allowed in global style blocks.
        throw std::runtime_error("Self-referential properties are not allowed in global style blocks.");
    }

    if (parser.currentToken.type == TokenType::Dot || parser.currentToken.type == TokenType::Hash || (parser.currentToken.type == TokenType::Identifier && parser.peekToken.type == TokenType::Dot)) {
        return parseReferencedProperty(parser);
    }

    if (parser.currentToken.type == TokenType::Number) {
        std::string rawValue = parser.currentToken.value;
        parser.advanceTokens();
        size_t unit_pos = rawValue.find_first_not_of("-.0123456789");
        double value = std::stod(rawValue.substr(0, unit_pos));
        std::string unit = (unit_pos != std::string::npos) ? rawValue.substr(unit_pos) : "";
        return {StyleValue::NUMERIC, value, unit};
    }

    if (parser.currentToken.type == TokenType::Dollar) {
        throw std::runtime_error("Responsive values are not supported in global style blocks.");
    }

    if (parser.currentToken.type == TokenType::OpenParen) {
        parser.advanceTokens();
        auto result = parseStyleExpression(parser);
        parser.expectToken(TokenType::CloseParen);
        return result;
    }

    if (parser.currentToken.type == TokenType::Identifier && parser.peekToken.type == TokenType::OpenParen) {
        // This is a variable template usage, e.g., MyTheme(primaryColor)
        std::string templateName = parser.currentToken.value;
        parser.advanceTokens(); // consume template name
        parser.expectToken(TokenType::OpenParen);

        std::string varName = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);

        parser.expectToken(TokenType::CloseParen);

        VarTemplateNode* varTmpl = parser.templateManager.getVarTemplate(parser.getCurrentNamespace(), templateName);
        if (!varTmpl) throw std::runtime_error("Variable template '" + templateName + "' not found.");

        auto it = varTmpl->variables.find(varName);
        if (it == varTmpl->variables.end()) throw std::runtime_error("Variable '" + varName + "' not found in template '" + templateName + "'.");

        return {StyleValue::STRING, 0.0, "", it->second};
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

        return {StyleValue::STRING, 0.0, "", ss.str()};
    }

    throw std::runtime_error("Unexpected token in expression: " + parser.currentToken.value);
}

StyleValue GlobalStyleBlockState::parseReferencedProperty(Parser& parser) {
    throw std::runtime_error("Property references are not supported in global style blocks.");
}

Selector GlobalStyleBlockState::parseSelector(Parser& parser) {
    // This is a placeholder implementation.
    // A real implementation would parse the selector and return a Selector object.
    return Selector();
}