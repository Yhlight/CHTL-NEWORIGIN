#include "StyleBlockState.h"

#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleValue.h" // Include the new StyleValue struct
#include "../Util/StyleUtil.h" // Include the new StyleUtil
#include "../Util/ASTUtil.h" // For findNodeBySelector
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

// Parses an inline property like 'color: red;'.
void StyleBlockState::parseInlineProperty(Parser& parser) {
    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::Colon);

    StyleValue value = parseStyleExpression(parser);
    // If a conditional evaluates to false with no 'else', it returns an EMPTY value.
    // In this case, we don't add the property at all.
    if (value.type != StyleValue::EMPTY) {
        parser.contextNode->inlineStyles[key] = value;
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
        // Use the new utility function
        std::string value = styleValueToString(sv);
        cssRules += "  " + key + ": " + value + ";\n";
        if(parser.currentToken.type == TokenType::Semicolon) parser.advanceTokens();
    }
    parser.expectToken(TokenType::CloseBrace);
    return cssRules;
}

// --- Start of Expression Parsing Implementation ---

StyleValue StyleBlockState::parseReferencedProperty(Parser& parser) {
    std::string selector;
    if (parser.currentToken.type == TokenType::Dot || parser.currentToken.type == TokenType::Hash) {
        selector += parser.currentToken.value;
        parser.advanceTokens();
        selector += parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
    } else {
        throw std::runtime_error("Unsupported selector type for property reference.");
    }

    parser.expectToken(TokenType::Dot);
    std::string propertyName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    if (!parser.parsedNodes) {
        throw std::runtime_error("Parser context does not have access to parsed nodes for lookup.");
    }
    const ElementNode* referencedNode = findNodeBySelector(*parser.parsedNodes, selector);
    if (!referencedNode) {
        throw std::runtime_error("Could not find element with selector: " + selector);
    }
    auto it = referencedNode->inlineStyles.find(propertyName);
    if (it == referencedNode->inlineStyles.end()) {
        throw std::runtime_error("Property '" + propertyName + "' not found on element '" + selector + "'.");
    }
    return it->second;
}

StyleValue StyleBlockState::parsePrimaryExpr(Parser& parser) {
    if (parser.currentToken.type == TokenType::Identifier) {
        // Could be a self-referential property or a string literal.
        // Check for self-reference by looking up the identifier in the current node's styles.
        auto it = parser.contextNode->inlineStyles.find(parser.currentToken.value);
        if (it != parser.contextNode->inlineStyles.end()) {
            parser.advanceTokens();
            return it->second;
        }
    }

    if ((parser.currentToken.type == TokenType::Dot || parser.currentToken.type == TokenType::Hash) &&
        parser.peekToken.type == TokenType::Identifier &&
        parser.peekToken2.type == TokenType::Dot) {
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
            if (parser.currentToken.type != TokenType::String && parser.currentToken.type != TokenType::Identifier && parser.currentToken.type != TokenType::Number) {
                 throw std::runtime_error("Invalid value for variable specialization.");
            }
            specializedValue = parser.currentToken.value;
            isSpecialized = true;
            parser.advanceTokens();
        }

        parser.expectToken(TokenType::CloseParen);

        if (isSpecialized) {
            return {StyleValue::STRING, 0.0, "", specializedValue};
        }

        // Handle non-specialized case
        std::string ns = parser.getCurrentNamespace();
        VarTemplateNode* varTmpl = parser.templateManager.getVarTemplate(ns, templateName);
        if (!varTmpl) throw std::runtime_error("Variable template not found: " + templateName);

        auto it = varTmpl->variables.find(varName);
        if (it == varTmpl->variables.end()) throw std::runtime_error("Variable '" + varName + "' not found in template '" + templateName + "'.");

        return {StyleValue::STRING, 0.0, "", it->second};
    }

    if (parser.currentToken.type == TokenType::Identifier || parser.currentToken.type == TokenType::String) {
        std::string val = parser.currentToken.value;
        parser.advanceTokens();
        return {StyleValue::STRING, 0.0, "", val};
    }

    throw std::runtime_error("Unexpected token in expression: " + parser.currentToken.value);
}

StyleValue StyleBlockState::parseMultiplicativeExpr(Parser& parser) {
    StyleValue result = parsePrimaryExpr(parser);
    while (parser.currentToken.type == TokenType::Asterisk || parser.currentToken.type == TokenType::Slash) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("LHS of * or / must be numeric.");
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parsePrimaryExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("RHS of * or / must be numeric.");
        if (!result.unit.empty() && !rhs.unit.empty()) throw std::runtime_error("Cannot operate on two values with units.");
        if (op == TokenType::Asterisk) result.numeric_val *= rhs.numeric_val;
        else {
            if (rhs.numeric_val == 0) throw std::runtime_error("Division by zero.");
            result.numeric_val /= rhs.numeric_val;
        }
        if (result.unit.empty()) result.unit = rhs.unit;
    }
    return result;
}

StyleValue StyleBlockState::parseAdditiveExpr(Parser& parser) {
    StyleValue result = parseMultiplicativeExpr(parser);
    while (parser.currentToken.type == TokenType::Plus || parser.currentToken.type == TokenType::Minus) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("LHS of + or - must be numeric.");
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parseMultiplicativeExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("RHS of + or - must be numeric.");
        if (result.unit != rhs.unit) throw std::runtime_error("Mismatched units for + or -.");
        if (op == TokenType::Plus) result.numeric_val += rhs.numeric_val;
        else result.numeric_val -= rhs.numeric_val;
    }
    return result;
}

StyleValue StyleBlockState::parseBooleanRelationalExpr(Parser& parser) {
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

StyleValue StyleBlockState::parseBooleanAndExpr(Parser& parser) {
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

StyleValue StyleBlockState::parseBooleanOrExpr(Parser& parser) {
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

StyleValue StyleBlockState::parseConditionalExpr(Parser& parser) {
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

StyleValue StyleBlockState::parseStyleExpression(Parser& parser) {
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

        std::vector<std::string> completedProperties;

        while (parser.currentToken.type != TokenType::CloseBrace) {
            if (parser.currentToken.type == TokenType::Delete) {
                parser.advanceTokens(); // consume 'delete'
                while(parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::CloseBrace) {
                    if (parser.currentToken.type == TokenType::Identifier) {
                        finalStyles.erase(parser.currentToken.value);
                        parser.advanceTokens();
                    }
                    if (parser.currentToken.type == TokenType::Comma) {
                        parser.advanceTokens();
                    } else {
                        // break if there is no comma, e.g. delete prop1;
                        break;
                    }
                }
                parser.expectToken(TokenType::Semicolon); // Consume the semicolon
            } else {
                // Handle property completion
                std::string key = parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
                parser.expectToken(TokenType::Colon);
                StyleValue sv = parseStyleExpression(parser);
                finalStyles[key] = styleValueToString(sv); // Add or overwrite
                completedProperties.push_back(key);
                if(parser.currentToken.type == TokenType::Semicolon) parser.advanceTokens();
            }
        }
        parser.expectToken(TokenType::CloseBrace);

        // Check if all valueless properties were completed
        for (const auto& valueless : tmpl->valuelessProperties) {
            bool found = false;
            for (const auto& completed : completedProperties) {
                if (valueless == completed) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                throw std::runtime_error("Valueless property '" + valueless + "' was not assigned a value in custom template usage.");
            }
        }

    } else {
        if (tmpl->isCustom && !tmpl->valuelessProperties.empty()) {
            throw std::runtime_error("Custom style template with valueless properties requires a specialization block '{...}'.");
        }
        parser.expectToken(TokenType::Semicolon);
    }

    // Apply the final, specialized styles to the element's inlineStyles map.
    for (const auto& pair : finalStyles) {
        if (!pair.second.empty()) { // Don't add valueless properties that weren't completed
            // This is a bit tricky, as the template stores raw strings.
            // For now, we'll just create a STRING-type StyleValue.
            // A future refactor could make templates store StyleValues directly.
            StyleValue sv;
            sv.type = StyleValue::STRING;
            sv.string_val = pair.second;
            parser.contextNode->inlineStyles[pair.first] = sv;
        }
    }
}
