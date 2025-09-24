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

Selector StyleBlockState::parseSelector(Parser& parser) {
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

StyleValue StyleBlockState::parseReferencedProperty(Parser& parser) {
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

// Helper to parse a raw number token string into a StyleValue
static StyleValue parseNumberToken(const std::string& s) {
    if (s.empty()) {
        throw std::runtime_error("Cannot parse an empty string into a StyleValue.");
    }
    size_t i = 0;
    if (s[0] == '-') i = 1;
    while (i < s.length() && (isdigit(s[i]) || s[i] == '.')) {
        i++;
    }
    std::string num_part = s.substr(0, i);
    std::string unit_part = s.substr(i);
    if (num_part.empty()) {
         throw std::runtime_error("No numeric part found in style value string: " + s);
    }
    try {
        StyleValue result;
        result.type = StyleValue::NUMERIC;
        result.numeric_val = std::stod(num_part);
        result.unit = unit_part;
        return result;
    } catch (const std::invalid_argument& ia) {
        throw std::runtime_error("Invalid number format in style value string: " + s);
    }
}

// Helper function to perform arithmetic operations on StyleValues
static StyleValue applyOp(const StyleValue& left, const TokenType op, const StyleValue& right) {
    if (left.type != StyleValue::NUMERIC || right.type != StyleValue::NUMERIC) {
        throw std::runtime_error("Arithmetic operations can only be performed on numeric values.");
    }

    std::string resultUnit;
    double leftValue = left.numeric_val;
    double rightValue = right.numeric_val;

    if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
        throw std::runtime_error("Mismatched units in style expression: '" + left.unit + "' and '" + right.unit + "'.");
    }
    resultUnit = !left.unit.empty() ? left.unit : right.unit;

    StyleValue result;
    result.type = StyleValue::NUMERIC;
    result.unit = resultUnit;

    switch (op) {
        case TokenType::Plus: result.numeric_val = leftValue + rightValue; break;
        case TokenType::Minus: result.numeric_val = leftValue - rightValue; break;
        case TokenType::Asterisk: result.numeric_val = leftValue * rightValue; break;
        case TokenType::Slash:
            if (rightValue == 0) throw std::runtime_error("Division by zero in style expression.");
            result.numeric_val = leftValue / rightValue;
            break;
        default: throw std::runtime_error("Unsupported operator in style expression.");
    }
    return result;
}


StyleValue StyleBlockState::parsePrimaryExpr(Parser& parser) {
    // First, check for a self-referential property.
    if (parser.currentToken.type == TokenType::Identifier) {
        auto it = parser.contextNode->inlineStyles.find(parser.currentToken.value);
        if (it != parser.contextNode->inlineStyles.end()) {
            // Found it. Return the stored value.
            parser.advanceTokens();
            return it->second;
        }
    }

    // Check if the expression is a property reference to another element.
    if (parser.currentToken.type == TokenType::Dot || parser.currentToken.type == TokenType::Hash) {
        return parseReferencedProperty(parser);
    }
    if (parser.currentToken.type == TokenType::Identifier && parser.peekToken.type == TokenType::Dot) {
        return parseReferencedProperty(parser);
    }

    if (parser.currentToken.type == TokenType::Number) {
        StyleValue result = parseNumberToken(parser.currentToken.value);
        parser.advanceTokens();
        return result;
    }
    if (parser.currentToken.type == TokenType::OpenParen) {
        parser.advanceTokens(); // Consume '('
        StyleValue result = parseStyleExpression(parser);
        parser.expectToken(TokenType::CloseParen); // Consume ')'
        return result;
    }

    if (parser.currentToken.type == TokenType::Identifier && parser.peekToken.type == TokenType::OpenParen) {
        // This is a variable template usage, e.g., MyTheme(primaryColor)
        std::string templateName = parser.currentToken.value;
        parser.advanceTokens(); // consume template name
        parser.expectToken(TokenType::OpenParen);

        std::string varName = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);

        // NOTE: Specialization like `Var(name = "value")` is not re-implemented yet.

        parser.expectToken(TokenType::CloseParen);

        // Handle optional 'from <namespace>' clause
        std::string ns = parser.getCurrentNamespace();
        if (parser.tryExpectKeyword(TokenType::From, "KEYWORD_FROM", "from")) {
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

        VarTemplateNode* varTmpl = parser.templateManager.getVarTemplate(ns, templateName);
        if (!varTmpl) throw std::runtime_error("Variable template '" + templateName + "' not found in namespace '" + ns + "'.");

        auto it = varTmpl->variables.find(varName);
        if (it == varTmpl->variables.end()) throw std::runtime_error("Variable '" + varName + "' not found in template '" + templateName + "'.");

        // The stored value is a string. We need to parse it as a new expression
        // to handle cases like `width: MyVars(someWidth) * 2;`
        // For now, we'll just return it as a string value. A more advanced implementation
        // would involve creating a new mini-parser here.
        StyleValue result;
        result.type = StyleValue::STRING;
        result.string_val = it->second;
        return result;
    }

    // For now, any other token is treated as a simple string value.
    StyleValue result;
    result.type = StyleValue::STRING;
    result.string_val = parser.currentToken.value;
    parser.advanceTokens();
    return result;
}

StyleValue StyleBlockState::parseMultiplicativeExpr(Parser& parser) {
    StyleValue left = parsePrimaryExpr(parser);

    while (parser.currentToken.type == TokenType::Asterisk || parser.currentToken.type == TokenType::Slash) {
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue right = parsePrimaryExpr(parser);
        left = applyOp(left, op, right);
    }

    return left;
}

StyleValue StyleBlockState::parseAdditiveExpr(Parser& parser) {
    StyleValue left = parseMultiplicativeExpr(parser);

    while (parser.currentToken.type == TokenType::Plus || parser.currentToken.type == TokenType::Minus) {
        TokenType op_type = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue right = parseMultiplicativeExpr(parser);

        // If units are different and both are non-empty, generate a calc() expression.
        if (left.type == StyleValue::NUMERIC && right.type == StyleValue::NUMERIC &&
            !left.unit.empty() && !right.unit.empty() && left.unit != right.unit)
        {
            std::string lhs_str = styleValueToString(left);
            std::string rhs_str = styleValueToString(right);
            std::string op_str = (op_type == TokenType::Plus) ? " + " : " - ";
            left = {StyleValue::STRING, 0.0, "", "calc(" + lhs_str + op_str + rhs_str + ")"};
        } else {
            // Otherwise, perform the calculation directly.
            left = applyOp(left, op_type, right);
        }
    }

    return left;
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
            // One side might be a self-reference that hasn't been evaluated yet.
            // For now, we'll throw, but a more advanced implementation might handle this.
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
    // A conditional expression can also be a simple value, so we start by parsing that.
    // The relational expression parser is the entry point for something that could be a bool or a value.
    StyleValue condition_or_value = parseBooleanOrExpr(parser);

    if (parser.currentToken.type == TokenType::QuestionMark) {
        if (condition_or_value.type != StyleValue::BOOL) {
            throw std::runtime_error("Expression before '?' must evaluate to a boolean.");
        }
        parser.advanceTokens(); // consume '?'

        StyleValue true_val = parseConditionalExpr(parser); // Recurse for nested conditionals

        if (parser.currentToken.type == TokenType::Colon) {
            parser.advanceTokens(); // consume ':'
            StyleValue false_val = parseConditionalExpr(parser);
            return condition_or_value.bool_val ? true_val : false_val;
        }

        // Handle optional "else" case from CHTL.md
        return condition_or_value.bool_val ? true_val : StyleValue{StyleValue::EMPTY};
    }

    return condition_or_value; // Not a conditional, just return the parsed value.
}

StyleValue StyleBlockState::parseStyleExpression(Parser& parser) {
    // A property value can be a chain of conditional expressions, separated by commas.
    // The first one that evaluates to true wins.
    // The entry point to the chain is parseConditionalExpr.
    return parseConditionalExpr(parser);
}

void StyleBlockState::applyStyleTemplateRecursive(
    Parser& parser,
    const std::string& ns,
    const std::string& templateName,
    std::map<std::string, std::string>& finalStyles,
    const std::vector<std::string>& deletedTemplates,
    std::vector<std::string>& visitedTemplates)
{
    // Check for circular dependencies.
    for(const auto& visited : visitedTemplates) {
        if (visited == templateName) {
            throw std::runtime_error("Circular dependency detected in style template inheritance involving '" + templateName + "'.");
        }
    }
    visitedTemplates.push_back(templateName);

    // Check if this template was deleted in the specialization block.
    for(const auto& deleted : deletedTemplates) {
        if (deleted == templateName) {
            return; // Do not apply this template or its parents.
        }
    }

    StyleTemplateNode* tmpl = parser.templateManager.getStyleTemplate(ns, templateName);
    if (!tmpl) {
        throw std::runtime_error("Style template '" + templateName + "' not found in namespace '" + ns + "'.");
    }

    // Recursively apply parent styles first.
    for (const auto& parentName : tmpl->parentNames) {
        applyStyleTemplateRecursive(parser, ns, parentName, finalStyles, deletedTemplates, visitedTemplates);
    }

    // Apply this template's own styles, overwriting any parent styles.
    for (const auto& pair : tmpl->styles) {
        finalStyles[pair.first] = pair.second;
    }

    visitedTemplates.pop_back(); // Backtrack
}


void StyleBlockState::parseStyleTemplateUsage(Parser& parser) {
    parser.expectToken(TokenType::At);
    if (parser.currentToken.value != "Style") throw std::runtime_error("Expected 'Style' after '@' for template usage.");
    parser.expectToken(TokenType::Identifier); // consume "Style"

    std::string templateName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

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

    StyleTemplateNode* tmpl = parser.templateManager.getStyleTemplate(ns, templateName);
    if (!tmpl) throw std::runtime_error("Style template not found: " + templateName);

    std::map<std::string, std::string> finalStyles;
    std::vector<std::string> deletedTemplates;
    std::map<std::string, StyleValue> specializedStyles;
    std::vector<std::string> completedProperties;


    // If the template is custom and a specialization block is provided, parse it first.
    if (tmpl->isCustom && parser.currentToken.type == TokenType::OpenBrace) {
        parser.expectToken(TokenType::OpenBrace);
        while (parser.currentToken.type != TokenType::CloseBrace) {
            if (parser.currentToken.type == TokenType::Delete) {
                parser.advanceTokens(); // consume 'delete'

                while(parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::CloseBrace) {
                    if (parser.currentToken.type == TokenType::At) {
                        parser.advanceTokens(); // consume '@'
                        if (parser.currentToken.value != "Style") throw std::runtime_error("Can only delete @Style templates.");
                        parser.advanceTokens(); // consume 'Style'
                        deletedTemplates.push_back(parser.currentToken.value);
                        parser.advanceTokens();
                    } else {
                        // This is a property deletion, store it to apply later.
                        specializedStyles[parser.currentToken.value] = { StyleValue::DELETED };
                        parser.advanceTokens();
                    }

                    if (parser.currentToken.type == TokenType::Comma) {
                        parser.advanceTokens(); // Consume comma and continue loop
                    } else {
                        break; // No comma, so the list of items to delete is done.
                    }
                }
                parser.expectToken(TokenType::Semicolon);

            } else { // Handle property completion/override
                std::string key = parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
                parser.expectToken(TokenType::Colon);
                specializedStyles[key] = parseStyleExpression(parser);
                completedProperties.push_back(key);
                if(parser.currentToken.type == TokenType::Semicolon) parser.advanceTokens();
            }
        }
        parser.expectToken(TokenType::CloseBrace);
    } else {
        if (tmpl->isCustom && !tmpl->valuelessProperties.empty()) {
            throw std::runtime_error("Custom style template with valueless properties requires a specialization block '{...}'.");
        }
        parser.expectToken(TokenType::Semicolon);
    }

    // --- Apply styles ---
    // 1. Recursively apply all parent and template styles.
    std::vector<std::string> visited;
    applyStyleTemplateRecursive(parser, ns, templateName, finalStyles, deletedTemplates, visited);

    // 2. Apply specialized styles from the { ... } block.
    for (const auto& pair : specializedStyles) {
        if (pair.second.type == StyleValue::DELETED) {
            finalStyles.erase(pair.first);
        } else {
            finalStyles[pair.first] = styleValueToString(pair.second);
        }
    }

    // 3. Check if all valueless properties were completed
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

    // 4. Apply the final, specialized styles to the element's inlineStyles map.
    for (const auto& pair : finalStyles) {
        StyleValue sv;
        sv.type = StyleValue::STRING; // For now, treat all template results as strings
        sv.string_val = pair.second;
        parser.contextNode->inlineStyles[pair.first] = sv;
    }
}
