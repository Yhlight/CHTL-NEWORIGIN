#include "StyleBlockState.h"

#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleValue.h" // Include the new StyleValue struct
#include "../Util/StyleUtil.h" // Include the new StyleUtil
#include "../Util/ASTUtil.h" // For findNodeBySelector
#include <cmath>
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
    // Handle the 'delete' keyword for inline properties.
    if (parser.tryExpectKeyword(TokenType::Delete, "KEYWORD_DELETE", "delete")) {
        std::string keyToDelete = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
        parser.contextNode->inlineStyles.erase(keyToDelete);
        parser.expectToken(TokenType::Semicolon);
        return;
    }

    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::Colon);

    StyleValue value = parseStyleExpression(parser);
    if (value.type == StyleValue::RESPONSIVE) {
        if (parser.contextNode->attributes.find("id") == parser.contextNode->attributes.end()) {
            parser.contextNode->attributes["id"] = {StyleValue::STRING, 0, "", "chtl-id-" + std::to_string(parser.elementIdCounter++)};
        }
        std::string elementId = parser.contextNode->attributes["id"].string_val;

        ResponsiveBinding binding;
        binding.elementId = elementId;
        binding.property = "style." + key;
        binding.unit = value.unit;

        parser.sharedContext.responsiveBindings[value.responsive_var_name].push_back(binding);

    } else if (value.type != StyleValue::EMPTY) {
        // If a conditional evaluates to false with no 'else', it returns an EMPTY value.
        // In this case, we don't add the property at all.
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
        // Check config flag before automatically adding the class attribute
        if (!parser.configManager.getActiveConfig()->disableStyleAutoAddClass && !parser.contextNode->attributes.count("class")) {
            parser.contextNode->attributes["class"] = {StyleValue::STRING, 0.0, "", selectorName};
        }
    } else if (parser.currentToken.type == TokenType::Hash) {
        selector += "#";
        parser.advanceTokens();
        if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected identifier after '#' for ID selector.");
        selectorName = parser.currentToken.value;
        // Check config flag before automatically adding the id attribute
        if (!parser.configManager.getActiveConfig()->disableStyleAutoAddId && !parser.contextNode->attributes.count("id")) {
            parser.contextNode->attributes["id"] = {StyleValue::STRING, 0.0, "", selectorName};
        }
    }
    selector += selectorName;
    parser.advanceTokens();

    std::string cssRules = parseCssRuleBlock(parser);
    parser.globalStyleContent += selector + " {\n" + cssRules + "}\n\n";

    // Now that a class/id might have been added, process any deferred rules.
    std::string baseSelector;
    if (parser.contextNode->attributes.count("class")) {
        baseSelector = "." + parser.contextNode->attributes.at("class").string_val;
    } else if (parser.contextNode->attributes.count("id")) {
        baseSelector = "#" + parser.contextNode->attributes.at("id").string_val;
    }

    if (!baseSelector.empty()) {
        for (const auto& deferredRule : parser.deferredAmpersandRules) {
            parser.globalStyleContent += baseSelector + deferredRule + "\n\n";
        }
        parser.deferredAmpersandRules.clear();
    }
}

// Parses a selector starting with '&', e.g., '&:hover { ... }'.
void StyleBlockState::parseAmpersandSelector(Parser& parser) {
    std::string baseSelector;
    if (parser.contextNode->attributes.count("class")) {
        baseSelector = "." + parser.contextNode->attributes.at("class").string_val;
    } else if (parser.contextNode->attributes.count("id")) {
        baseSelector = "#" + parser.contextNode->attributes.at("id").string_val;
    }

    parser.advanceTokens(); // consume '&'

    std::string selectorSuffix;
    while(parser.currentToken.type != TokenType::OpenBrace && parser.currentToken.type != TokenType::EndOfFile) {
        selectorSuffix += parser.currentToken.value;
        parser.advanceTokens();
    }

    std::string finalSelector = selectorSuffix;
    std::string cssRules = parseCssRuleBlock(parser);
    std::string fullRule = finalSelector + " {\n" + cssRules + "}\n";

    if (baseSelector.empty()) {
        // If the base selector isn't defined yet, defer this rule.
        parser.deferredAmpersandRules.push_back(fullRule);
    } else {
        parser.globalStyleContent += baseSelector + fullRule + "\n\n";
    }
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

StyleValue StyleBlockState::parsePrimaryExpr(Parser& parser) {
    if (parser.currentToken.type == TokenType::Identifier) {
        // Could be a self-referential property, an attribute, or a string literal.
        // Check for self-reference in both inline styles and attributes.
        const std::string& key = parser.currentToken.value;
        auto style_it = parser.contextNode->inlineStyles.find(key);
        if (style_it != parser.contextNode->inlineStyles.end()) {
            parser.advanceTokens();
            return style_it->second;
        }
        auto attr_it = parser.contextNode->attributes.find(key);
        if (attr_it != parser.contextNode->attributes.end()) {
            parser.advanceTokens();
            return attr_it->second;
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

    if (parser.currentToken.type == TokenType::Dollar) {
        parser.advanceTokens(); // consume '$'
        if (parser.currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected identifier after '$' for responsive value.");
        }
        std::string varName = parser.currentToken.value;
        parser.advanceTokens();
        parser.expectToken(TokenType::Dollar); // consume closing '$'

        StyleValue sv;
        sv.type = StyleValue::RESPONSIVE;
        sv.responsive_var_name = varName;

        // Check for an optional unit identifier immediately following the responsive value
        if (parser.currentToken.type == TokenType::Identifier) {
            sv.unit = parser.currentToken.value;
            parser.advanceTokens();
        }

        return sv;
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

StyleValue StyleBlockState::parsePowerExpr(Parser& parser) {
    StyleValue result = parsePrimaryExpr(parser);
    while (parser.currentToken.type == TokenType::DoubleAsterisk) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("LHS of ** must be numeric.");
        parser.advanceTokens();
        StyleValue rhs = parsePrimaryExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("RHS of ** must be numeric.");

        // For power, the exponent must be unitless.
        if (!rhs.unit.empty()) {
            throw std::runtime_error("Exponent for ** operator cannot have a unit.");
        }

        result.numeric_val = pow(result.numeric_val, rhs.numeric_val);
    }
    return result;
}

StyleValue StyleBlockState::parseMultiplicativeExpr(Parser& parser) {
    StyleValue result = parsePowerExpr(parser);
    while (parser.currentToken.type == TokenType::Asterisk || parser.currentToken.type == TokenType::Slash || parser.currentToken.type == TokenType::Percent) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("LHS of *, /, or % must be numeric.");
        TokenType op = parser.currentToken.type;
        parser.advanceTokens();
        StyleValue rhs = parsePowerExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("RHS of *, /, or % must be numeric.");

        if (op == TokenType::Percent) {
            if (!result.unit.empty() && !rhs.unit.empty() && result.unit != rhs.unit) {
                throw std::runtime_error("Cannot perform modulo on values with different units.");
            }
            if (rhs.numeric_val == 0) throw std::runtime_error("Modulo by zero.");
            result.numeric_val = fmod(result.numeric_val, rhs.numeric_val);
        } else { // Multiplication and Division
            if (!result.unit.empty() && !rhs.unit.empty()) {
                throw std::runtime_error("Cannot multiply or divide two values with units.");
            }
            if (op == TokenType::Asterisk) {
                result.numeric_val *= rhs.numeric_val;
            } else { // Slash
                if (rhs.numeric_val == 0) throw std::runtime_error("Division by zero.");
                result.numeric_val /= rhs.numeric_val;
            }
            if (result.unit.empty()) {
                result.unit = rhs.unit;
            }
        }
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


    // If a specialization block is provided, parse it.
    // This is allowed for both [Custom] and [Template] styles.
    if (parser.currentToken.type == TokenType::OpenBrace) {
        parser.expectToken(TokenType::OpenBrace);
        while (parser.currentToken.type != TokenType::CloseBrace) {
            if (parser.tryExpectKeyword(TokenType::Delete, "KEYWORD_DELETE", "delete")) {
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
