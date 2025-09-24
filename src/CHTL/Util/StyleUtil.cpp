#include "StyleUtil.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "ASTUtil.h"
#include <sstream>
#include <stdexcept>

// Forward declarations for the recursive descent parser
StyleValue parseConditionalExpr(Parser& parser);

std::string styleValueToString(const StyleValue& sv) {
    switch (sv.type) {
        case StyleValue::NUMERIC: {
            std::ostringstream oss;
            oss.precision(15); // Preserve precision
            oss << sv.numeric_val;
            std::string numStr = oss.str();
            // Clean up trailing zeros for non-decimal numbers
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
            throw std::runtime_error("Cannot convert a boolean expression result to a string style value.");
    }
    return ""; // Should not be reached
}


// --- Start of Expression Parsing Implementation ---

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

        // Check for an index, e.g., [0]
        if (parser.currentToken.type == TokenType::OpenBracket) {
            parser.advanceTokens(); // consume '['
            if (parser.currentToken.type != TokenType::Number) throw std::runtime_error("Expected number inside index '[]'.");
            currentPart.index = std::stoi(parser.currentToken.value);
            parser.advanceTokens();
            parser.expectToken(TokenType::CloseBracket);
        }

        // Peek ahead to see if there's a descendant selector (another part)
        // or if the selector is ending (e.g., followed by a dot for the property).
        if (parser.currentToken.type == TokenType::Identifier || parser.currentToken.type == TokenType::Dot || parser.currentToken.type == TokenType::Hash) {
            // If the next token is a property-dot, the selector is done.
            if(parser.currentToken.type == TokenType::Dot && parser.peekToken.type == TokenType::Identifier && parser.peekToken2.type != TokenType::OpenBracket) {
                break;
            }
             // Otherwise, it's a new part of a descendant selector
            selector.parts.emplace_back();
        } else {
            // Any other token ends the selector
            break;
        }
    }
    return selector;
}

StyleValue parseReferencedProperty(Parser& parser) {
    // 1. Use the new selector parser
    Selector selector = parseSelector(parser);

    // 2. The selector parser stops right before the property's dot.
    parser.expectToken(TokenType::Dot);
    std::string propertyName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    // 3. Find the node using the new findNodeBySelector (which we'll update next)
    if (!parser.parsedNodes) {
        throw std::runtime_error("Parser context does not have access to parsed nodes for lookup.");
    }
    // Note: This will fail to compile until we update ASTUtil.h/cpp
    const ElementNode* referencedNode = findNodeBySelector(*parser.parsedNodes, selector);
    if (!referencedNode) {
        // TODO: Create a utility to serialize the selector struct to a string for better error messages
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
        // Could be a self-referential property or a string literal.
        // Check for self-reference by looking up the identifier in the current node's styles and attributes.
        if (parser.contextNode) {
            // Check inline styles first
            auto it_style = parser.contextNode->inlineStyles.find(parser.currentToken.value);
            if (it_style != parser.contextNode->inlineStyles.end()) {
                parser.advanceTokens();
                return it_style->second;
            }
            // Then check attributes
            auto it_attr = parser.contextNode->attributes.find(parser.currentToken.value);
            if (it_attr != parser.contextNode->attributes.end()) {
                parser.advanceTokens();
                return it_attr->second;
            }
        }
    }

    // Check if the expression is a property reference.
    if (parser.currentToken.type == TokenType::Dot || parser.currentToken.type == TokenType::Hash) {
        // Starts with a class or ID selector, must be a reference.
        return parseReferencedProperty(parser);
    }
    if (parser.currentToken.type == TokenType::Identifier && parser.peekToken.type == TokenType::Dot) {
        // Starts with a tag selector, e.g., "div.width".
        // We must also check that it's not a self-reference to a property with the same name.
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
        return {StyleValue::NUMERIC, value, unit};
    }

    if (parser.currentToken.type == TokenType::OpenParen) {
        parser.advanceTokens();
        auto result = parseStyleExpression(parser); // Recursive call for precedence
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

        std::string specializedValue;
        bool isSpecialized = false;
        if (parser.currentToken.type == TokenType::Equals) {
            parser.advanceTokens(); // consume '='

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
            // Do not advance tokens here, the loop consumes up to the ')'
        }

        parser.expectToken(TokenType::CloseParen);

        // Handle optional 'from <namespace>' clause
        std::string ns = parser.getCurrentNamespace();
        if (parser.currentToken.type == TokenType::From) {
            parser.advanceTokens(); // consume 'from'

            std::stringstream ns_builder;
            ns_builder << parser.currentToken.value;
            parser.expectToken(TokenType::Identifier);

            while(parser.currentToken.type == TokenType::Dot) {
                parser.advanceTokens(); // consume '.'
                ns_builder << "." << parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
            }
            ns = ns_builder.str();
        }

        if (isSpecialized) {
            return {StyleValue::STRING, 0.0, "", specializedValue};
        }

        // Handle non-specialized case
        VarTemplateNode* varTmpl = parser.templateManager.getVarTemplate(ns, templateName);
        if (!varTmpl) throw std::runtime_error("Variable template '" + templateName + "' not found in namespace '" + ns + "'.");

        auto it = varTmpl->variables.find(varName);
        if (it == varTmpl->variables.end()) throw std::runtime_error("Variable '" + varName + "' not found in template '" + templateName + "'.");

        return {StyleValue::STRING, 0.0, "", it->second};
    }

    if (parser.currentToken.type == TokenType::Identifier || parser.currentToken.type == TokenType::String) {
        std::stringstream ss;
        ss << parser.currentToken.value;
        parser.advanceTokens();

        // Greedily consume subsequent identifiers/numbers as part of a multi-word string literal.
        // This allows for values like `font-family: Times New Roman;`
        while (parser.currentToken.type == TokenType::Identifier || parser.currentToken.type == TokenType::Number || parser.currentToken.type == TokenType::String) {
             // Stop if we hit a semicolon or the end of the block, as that marks the end of the value.
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

#include <cmath> // For std::pow

StyleValue parsePowerExpr(Parser& parser) {
    StyleValue result = parsePrimaryExpr(parser);
    while (parser.currentToken.type == TokenType::Power) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("LHS of ** must be numeric.");
        parser.advanceTokens();
        StyleValue rhs = parsePrimaryExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("RHS of ** must be numeric.");

        // For power, the RHS must be unitless.
        if (!rhs.unit.empty()) {
            throw std::runtime_error("Exponent for the power operator (**) must be a unitless number.");
        }

        result.numeric_val = std::pow(result.numeric_val, rhs.numeric_val);
    }
    return result;
}

StyleValue parseMultiplicativeExpr(Parser& parser) {
    StyleValue result = parsePowerExpr(parser);
    while (parser.currentToken.type == TokenType::Asterisk || parser.currentToken.type == TokenType::Slash) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("LHS of * or / must be numeric.");
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parsePowerExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("RHS of * or / must be numeric.");

        // For multiplication and division, one of the operands must be unitless.
        if (!result.unit.empty() && !rhs.unit.empty()) {
            throw std::runtime_error("Cannot multiply or divide two values with units.");
        }

        if (op == TokenType::Asterisk) {
            result.numeric_val *= rhs.numeric_val;
        } else {
            if (rhs.numeric_val == 0) throw std::runtime_error("Division by zero.");
            result.numeric_val /= rhs.numeric_val;
        }

        // The result should adopt the unit of whichever operand had one.
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

        // If units are different and both are non-empty, generate a calc() expression.
        if (result.unit != rhs.unit && !result.unit.empty() && !rhs.unit.empty()) {
            std::string lhs_str = styleValueToString(result);
            std::string rhs_str = styleValueToString(rhs);
            std::string op_str = (op == TokenType::Plus) ? " + " : " - ";
            result = {StyleValue::STRING, 0.0, "", "calc(" + lhs_str + op_str + rhs_str + ")"};
        } else {
            // Otherwise, perform the calculation directly.
            if (op == TokenType::Plus) result.numeric_val += rhs.numeric_val;
            else result.numeric_val -= rhs.numeric_val;
            // If the original value had no unit, adopt the unit of the RHS.
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
    return lhs; // Not a relational expression, return the value.
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
        parser.advanceTokens(); // consume '?'

        StyleValue true_val = parseConditionalExpr(parser); // Recurse for nested conditionals

        if (parser.currentToken.type == TokenType::Colon) {
            parser.advanceTokens(); // consume ':'
            StyleValue false_val = parseConditionalExpr(parser);
            return condition.bool_val ? true_val : false_val;
        }

        return condition.bool_val ? true_val : StyleValue{StyleValue::EMPTY};
    }

    return condition; // Not a conditional, just return the value.
}

StyleValue parseStyleExpression(Parser& parser) {
    // A property value is a chain of conditional expressions, separated by commas.
    // The first one that evaluates to true wins.
    StyleValue finalValue{StyleValue::EMPTY};
    bool conditionMet = false;

    while (true) {
        StyleValue result = parseConditionalExpr(parser);

        if (!conditionMet) {
            if (result.type != StyleValue::EMPTY) {
                finalValue = result;
                // If the result was not a simple value, it must have been a successful conditional
                if (result.type != StyleValue::NUMERIC && result.type != StyleValue::STRING) {
                     conditionMet = true;
                }
            }
        }

        if (parser.currentToken.type == TokenType::Comma) {
            parser.advanceTokens();
            // If we've already found our value, we still need to parse the rest of the expression to consume the tokens.
            if (conditionMet) {
                parseStyleExpression(parser);
            }
        } else {
            break; // End of chain
        }
    }
    return finalValue;
}
