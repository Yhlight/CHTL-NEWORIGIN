#include "ExpressionEvaluator.h"
#include "StyleUtil.h"
#include "ASTUtil.h"
#include "../CHTLParser/Parser.h"
#include <stdexcept>
#include <sstream>

ExpressionEvaluator::ExpressionEvaluator(Parser& parser, ElementNode* contextNode)
    : parser(parser), contextNode(contextNode) {}

StyleValue ExpressionEvaluator::evaluate() {
    // This is the top-level entry point that was previously `parseStyleExpression`
    // A property value is a chain of conditional expressions, separated by commas.
    // The first one that evaluates to true wins.
    StyleValue finalValue{StyleValue::EMPTY};
    bool conditionMet = false;

    while (true) {
        // The core logic is to parse one conditional expression at a time.
        StyleValue result = parseConditionalExpr();

        if (!conditionMet) {
            // If the result is not empty, it's our potential winner.
            if (result.type != StyleValue::EMPTY) {
                finalValue = result;
                // If the result was not a simple value, it must have been a successful conditional.
                // This logic might need refinement, but the key is that once a true conditional is found, we stop updating the value.
                if (result.type == StyleValue::BOOL) {
                     conditionMet = true;
                }
            }
        }

        // If there's a comma, there's another link in the chain.
        if (parser.currentToken.type == TokenType::Comma) {
            parser.advanceTokens();
            // If we've already found our value, we still need to parse the rest of the expression
            // just to consume the tokens and advance the parser state correctly.
            if (conditionMet) {
                evaluate(); // Recursive call to consume the rest of the chain
            }
        } else {
            break; // End of chain
        }
    }
    return finalValue;
}

StyleValue ExpressionEvaluator::evaluateBoolean() {
    // This is a simplified version of evaluate() that only parses a single
    // boolean expression and does not look for a comma-separated chain.
    StyleValue result = parseBooleanOrExpr();
    if (result.type != StyleValue::BOOL) {
        throw std::runtime_error("Condition must evaluate to a boolean value.");
    }
    return result;
}


// --- Start of Expression Parsing Implementation (Moved from StyleBlockState.cpp) ---

Selector ExpressionEvaluator::parseSelector() {
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

        // Check for an index, e.g., [0]
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

StyleValue ExpressionEvaluator::parseReferencedProperty() {
    Selector selector = parseSelector();
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
    auto it = referencedNode->attributes.find(propertyName);
    if (it == referencedNode->attributes.end()) {
        throw std::runtime_error("Property '" + propertyName + "' not found on referenced element.");
    }
    return it->second;
}

StyleValue ExpressionEvaluator::parsePrimaryExpr() {
    // Case 1: The value is a number, but we need to check if it's part of a larger string.
    if (parser.currentToken.type == TokenType::Number) {
        // Peek ahead. If the next token is an operator, separator, or end of statement,
        // then this is a standalone number.
        TokenType nextType = parser.peekToken.type;
        if (nextType == TokenType::Plus || nextType == TokenType::Minus || nextType == TokenType::Asterisk ||
            nextType == TokenType::Slash || nextType == TokenType::GreaterThan || nextType == TokenType::LessThan ||
            nextType == TokenType::EqualsEquals || nextType == TokenType::NotEquals || nextType == TokenType::GreaterThanEquals ||
            nextType == TokenType::LessThanEquals || nextType == TokenType::QuestionMark || nextType == TokenType::Colon ||
            nextType == TokenType::LogicalAnd || nextType == TokenType::LogicalOr ||
            nextType == TokenType::Semicolon || nextType == TokenType::CloseBrace || nextType == TokenType::CloseParen ||
            nextType == TokenType::Comma || nextType == TokenType::EndOfFile)
        {
            std::string rawValue = parser.currentToken.value;
            parser.advanceTokens();
            size_t unit_pos = rawValue.find_first_not_of("-.0123456789");
            double value = std::stod(rawValue.substr(0, unit_pos));
            std::string unit = (unit_pos != std::string::npos) ? rawValue.substr(unit_pos) : "";
            return {StyleValue::NUMERIC, value, unit};
        }
        // Otherwise, fall through to the multi-word string parsing logic.
    }

    // Case 2: The value is a self-referential property lookup (e.g., `width > 50px`)
    if (contextNode && parser.currentToken.type == TokenType::Identifier) {
        // Check if it's a known attribute on the current element.
        auto it = contextNode->attributes.find(parser.currentToken.value);
        if (it != contextNode->attributes.end()) {
            // It is. Now, check if it's being used in a comparison. If not, it's just a string.
             TokenType nextType = parser.peekToken.type;
             if (nextType == TokenType::GreaterThan || nextType == TokenType::LessThan || nextType == TokenType::EqualsEquals ||
                 nextType == TokenType::NotEquals || nextType == TokenType::GreaterThanEquals || nextType == TokenType::LessThanEquals)
             {
                parser.advanceTokens();
                return it->second;
             }
        }
    }

    // Case 3: Parenthesized expression for precedence
    if (parser.currentToken.type == TokenType::OpenParen) {
        parser.advanceTokens();
        auto result = evaluate(); // Recursive call
        parser.expectToken(TokenType::CloseParen);
        return result;
    }

    // Case 4: Template variable usage, e.g. MyTheme(myColor)
    if (parser.currentToken.type == TokenType::Identifier && parser.peekToken.type == TokenType::OpenParen) {
        std::string templateName = parser.currentToken.value;
        parser.advanceTokens();
        parser.expectToken(TokenType::OpenParen);
        std::string varName = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
        // ... (template variable logic as before) ...
        parser.expectToken(TokenType::CloseParen);
        // This part needs the template manager, accessed via the parser reference
        VarTemplateNode* varTmpl = parser.templateManager.getVarTemplate(parser.getCurrentNamespace(), templateName);
        if (!varTmpl) throw std::runtime_error("Variable template '" + templateName + "' not found.");
        auto it = varTmpl->variables.find(varName);
        if (it == varTmpl->variables.end()) throw std::runtime_error("Variable '" + varName + "' not found in template '" + templateName + "'.");
        return {StyleValue::STRING, 0.0, "", it->second};
    }

    // Case 5: The value is a plain string, potentially with multiple words.
    // This is the fallback case.
    std::stringstream ss;
    ss << parser.currentToken.value;
    parser.advanceTokens();

    while (parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::CloseBrace &&
           parser.currentToken.type != TokenType::CloseParen && parser.currentToken.type != TokenType::Comma &&
           parser.currentToken.type != TokenType::QuestionMark && parser.currentToken.type != TokenType::Colon &&
           parser.currentToken.type != TokenType::EndOfFile)
    {
         ss << " " << parser.currentToken.value;
         parser.advanceTokens();
    }
    return {StyleValue::STRING, 0.0, "", ss.str()};

    throw std::runtime_error("Unexpected token in expression: " + parser.currentToken.value);
}

StyleValue ExpressionEvaluator::parseMultiplicativeExpr() {
    StyleValue result = parsePrimaryExpr();
    while (parser.currentToken.type == TokenType::Asterisk || parser.currentToken.type == TokenType::Slash) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("LHS of * or / must be numeric.");
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parsePrimaryExpr();
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("RHS of * or / must be numeric.");
        if (!result.unit.empty() && !rhs.unit.empty()) throw std::runtime_error("Cannot multiply or divide two values with units.");
        if (op == TokenType::Asterisk) result.numeric_val *= rhs.numeric_val;
        else {
            if (rhs.numeric_val == 0) throw std::runtime_error("Division by zero.");
            result.numeric_val /= rhs.numeric_val;
        }
        if (result.unit.empty()) result.unit = rhs.unit;
    }
    return result;
}

StyleValue ExpressionEvaluator::parseAdditiveExpr() {
    StyleValue result = parseMultiplicativeExpr();
    while (parser.currentToken.type == TokenType::Plus || parser.currentToken.type == TokenType::Minus) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("LHS of + or - must be numeric.");
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parseMultiplicativeExpr();
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("RHS of + or - must be numeric.");
        if (result.unit != rhs.unit && !result.unit.empty() && !rhs.unit.empty()) {
            std::string lhs_str = styleValueToString(result);
            std::string rhs_str = styleValueToString(rhs);
            std::string op_str = (op == TokenType::Plus) ? " + " : " - ";
            result = {StyleValue::STRING, 0.0, "", "calc(" + lhs_str + op_str + rhs_str + ")"};
        } else {
            if (op == TokenType::Plus) result.numeric_val += rhs.numeric_val;
            else result.numeric_val -= rhs.numeric_val;
            if (result.unit.empty()) result.unit = rhs.unit;
        }
    }
    return result;
}

StyleValue ExpressionEvaluator::parseBooleanRelationalExpr() {
    StyleValue lhs = parseAdditiveExpr();
    if (parser.currentToken.type == TokenType::GreaterThan || parser.currentToken.type == TokenType::LessThan ||
        parser.currentToken.type == TokenType::EqualsEquals || parser.currentToken.type == TokenType::NotEquals ||
        parser.currentToken.type == TokenType::GreaterThanEquals || parser.currentToken.type == TokenType::LessThanEquals)
    {
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parseAdditiveExpr();
        StyleValue bool_result;
        bool_result.type = StyleValue::BOOL;
        if (lhs.type == StyleValue::NUMERIC && rhs.type == StyleValue::NUMERIC) {
             if (lhs.unit != rhs.unit) throw std::runtime_error("Cannot compare different units.");
             switch(op) {
                case TokenType::GreaterThan: bool_result.bool_val = lhs.numeric_val > rhs.numeric_val; break;
                // ... other cases ...
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

StyleValue ExpressionEvaluator::parseBooleanAndExpr() {
    StyleValue result = parseBooleanRelationalExpr();
    while (parser.currentToken.type == TokenType::LogicalAnd) {
        if (result.type != StyleValue::BOOL) throw std::runtime_error("LHS of '&&' must be a boolean expression.");
        parser.advanceTokens();
        StyleValue rhs = parseBooleanRelationalExpr();
        if (rhs.type != StyleValue::BOOL) throw std::runtime_error("RHS of '&&' must be a boolean expression.");
        result.bool_val = result.bool_val && rhs.bool_val;
    }
    return result;
}

StyleValue ExpressionEvaluator::parseBooleanOrExpr() {
    StyleValue result = parseBooleanAndExpr();
    while (parser.currentToken.type == TokenType::LogicalOr) {
        if (result.type != StyleValue::BOOL) throw std::runtime_error("LHS of '||' must be a boolean expression.");
        parser.advanceTokens();
        StyleValue rhs = parseBooleanAndExpr();
        if (rhs.type != StyleValue::BOOL) throw std::runtime_error("RHS of '||' must be a boolean expression.");
        result.bool_val = result.bool_val || rhs.bool_val;
    }
    return result;
}

StyleValue ExpressionEvaluator::parseConditionalExpr() {
    StyleValue condition = parseBooleanOrExpr();
    if (parser.currentToken.type == TokenType::QuestionMark) {
        if (condition.type != StyleValue::BOOL) {
            throw std::runtime_error("Expression before '?' must evaluate to a boolean.");
        }
        parser.advanceTokens(); // consume '?'
        StyleValue true_val = parseConditionalExpr();
        if (parser.currentToken.type == TokenType::Colon) {
            parser.advanceTokens(); // consume ':'
            StyleValue false_val = parseConditionalExpr();
            return condition.bool_val ? true_val : false_val;
        }
        return condition.bool_val ? true_val : StyleValue{StyleValue::EMPTY};
    }
    return condition;
}