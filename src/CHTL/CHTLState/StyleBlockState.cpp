#include "StyleBlockState.h"

#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleValue.h"
#include "../Util/StyleUtil.h"
#include "../Util/ASTUtil.h"
#include <cmath>
#include <stdexcept>
#include <sstream>

std::unique_ptr<BaseNode> StyleBlockState::handle(Parser& parser) {
    if (!parser.contextNode) {
        throw std::runtime_error("StyleBlockState requires a context node.");
    }
    parser.expectToken(TokenType::Identifier);
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
            parser.contextNode->attributes["id"] = StyleValue("chtl-id-" + std::to_string(parser.elementIdCounter++));
        }
        std::string elementId = parser.contextNode->attributes["id"].string_val;
        ResponsiveBinding binding;
        binding.elementId = elementId;
        binding.property = "style." + key;
        binding.unit = value.unit;
        parser.sharedContext.responsiveBindings[value.responsive_var_name].push_back(binding);
    } else if (value.type == StyleValue::DYNAMIC_CONDITIONAL) {
        if (parser.contextNode->attributes.find("id") == parser.contextNode->attributes.end()) {
            parser.contextNode->attributes["id"] = StyleValue("chtl-id-" + std::to_string(parser.elementIdCounter++));
        }
        std::string elementId = parser.contextNode->attributes.at("id").string_val;
        DynamicConditionalBinding binding;
        binding.targetElementId = elementId;
        binding.targetProperty = "style." + key;
        binding.expression = value.dynamic_expr;
        parser.sharedContext.dynamicConditionalBindings.push_back(binding);
    } else if (value.type != StyleValue::EMPTY) {
        parser.contextNode->inlineStyles[key] = value;
    }

    if (parser.currentToken.type == TokenType::Semicolon) {
        parser.advanceTokens();
    }
}

void StyleBlockState::parseClassOrIdSelector(Parser& parser) {
    std::string selector;
    std::string selectorName;

    if (parser.currentToken.type == TokenType::Dot) {
        selector += ".";
        parser.advanceTokens();
        if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected identifier after '.' for class selector.");
        selectorName = parser.currentToken.value;
        if (!parser.configManager.getActiveConfig()->disableStyleAutoAddClass && !parser.contextNode->attributes.count("class")) {
            parser.contextNode->attributes["class"] = StyleValue(selectorName);
        }
    } else if (parser.currentToken.type == TokenType::Hash) {
        selector += "#";
        parser.advanceTokens();
        if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected identifier after '#' for ID selector.");
        selectorName = parser.currentToken.value;
        if (!parser.configManager.getActiveConfig()->disableStyleAutoAddId && !parser.contextNode->attributes.count("id")) {
            parser.contextNode->attributes["id"] = StyleValue(selectorName);
        }
    }
    selector += selectorName;
    parser.advanceTokens();

    std::string cssRules = parseCssRuleBlock(parser);
    parser.globalStyleContent += selector + " {\n" + cssRules + "}\n\n";

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

void StyleBlockState::parseAmpersandSelector(Parser& parser) {
    std::string baseSelector;
    if (parser.contextNode->attributes.count("class")) {
        baseSelector = "." + parser.contextNode->attributes.at("class").string_val;
    } else if (parser.contextNode->attributes.count("id")) {
        baseSelector = "#" + parser.contextNode->attributes.at("id").string_val;
    }

    parser.advanceTokens();

    std::string selectorSuffix;
    while(parser.currentToken.type != TokenType::OpenBrace && parser.currentToken.type != TokenType::EndOfFile) {
        selectorSuffix += parser.currentToken.value;
        parser.advanceTokens();
    }

    std::string finalSelector = selectorSuffix;
    std::string cssRules = parseCssRuleBlock(parser);
    std::string fullRule = finalSelector + " {\n" + cssRules + "}\n";

    if (baseSelector.empty()) {
        parser.deferredAmpersandRules.push_back(fullRule);
    } else {
        parser.globalStyleContent += baseSelector + fullRule + "\n\n";
    }
}

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

static bool isStyleValueTerminator(TokenType type) {
    switch (type) {
        case TokenType::Plus: case TokenType::Minus: case TokenType::Asterisk: case TokenType::Slash:
        case TokenType::Percent: case TokenType::DoubleAsterisk: case TokenType::QuestionMark:
        case TokenType::LogicalAnd: case TokenType::LogicalOr: case TokenType::EqualsEquals:
        case TokenType::NotEquals: case TokenType::GreaterThan: case TokenType::LessThan:
        case TokenType::GreaterThanEquals: case TokenType::LessThanEquals:
        case TokenType::Semicolon: case TokenType::Comma: case TokenType::CloseParen: case TokenType::CloseBrace:
        case TokenType::EndOfFile:
            return true;
        default:
            return false;
    }
}

Selector StyleBlockState::parseSelector(Parser& parser) {
    Selector selector;
    while (true) {
        if (parser.currentToken.type == TokenType::Dot && parser.peekToken.type == TokenType::Identifier) {
            if (isStyleValueTerminator(parser.peekToken2.type)) {
                break;
            }
        }

        if (parser.currentToken.type != TokenType::Identifier &&
            parser.currentToken.type != TokenType::Dot &&
            parser.currentToken.type != TokenType::Hash) {
            break;
        }

        selector.parts.emplace_back();
        SelectorPart& currentPart = selector.parts.back();

        if (parser.currentToken.type == TokenType::Identifier) {
            currentPart.tagName = parser.currentToken.value;
            parser.advanceTokens();
        } else if (parser.currentToken.type == TokenType::Dot) {
            parser.advanceTokens();
            if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected class name after '.'.");
            currentPart.className = parser.currentToken.value;
            parser.advanceTokens();
        } else if (parser.currentToken.type == TokenType::Hash) {
            parser.advanceTokens();
            if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected id name after '#'.");
            currentPart.id = parser.currentToken.value;
            parser.advanceTokens();
        }

        if (parser.currentToken.type == TokenType::OpenBracket) {
            parser.advanceTokens();
            if (parser.currentToken.type != TokenType::Number) throw std::runtime_error("Expected number inside index '[]'.");
            currentPart.index = std::stoi(parser.currentToken.value);
            parser.advanceTokens();
            parser.expectToken(TokenType::CloseBracket);
        }
    }
    return selector;
}

StyleValue StyleBlockState::parseReferencedProperty(Parser& parser) {
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

StyleValue StyleBlockState::parsePrimaryExpr(Parser& parser) {
    if (parser.currentToken.type == TokenType::Identifier) {
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

    if (parser.currentToken.type == TokenType::Dollar) {
        parser.advanceTokens();
        if (parser.currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected identifier after '$' for responsive value.");
        }
        std::string varName = parser.currentToken.value;
        parser.advanceTokens();
        parser.expectToken(TokenType::Dollar);

        StyleValue sv;
        sv.type = StyleValue::RESPONSIVE;
        sv.responsive_var_name = varName;

        if (parser.currentToken.type == TokenType::Identifier) {
            sv.unit = parser.currentToken.value;
            parser.advanceTokens();
        }
        return sv;
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

StyleValue StyleBlockState::parsePowerExpr(Parser& parser) {
    StyleValue result = parsePrimaryExpr(parser);
    while (parser.currentToken.type == TokenType::DoubleAsterisk) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("LHS of ** must be numeric.");
        parser.advanceTokens();
        StyleValue rhs = parsePrimaryExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("RHS of ** must be numeric.");
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
        } else {
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
    return lhs;
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

StyleValue StyleBlockState::parseDynamicConditionalExpression(Parser& parser) {
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
    const auto& opToken = parser.currentToken;
    if (opToken.type == TokenType::GreaterThan || opToken.type == TokenType::LessThan || opToken.type == TokenType::EqualsEquals ||
        opToken.type == TokenType::NotEquals || opToken.type == TokenType::GreaterThanEquals || opToken.type == TokenType::LessThanEquals) {
        expr->op = opToken.value;
        parser.advanceTokens();
    } else {
        throw std::runtime_error("Expected a comparison operator in dynamic expression.");
    }
    if(parser.currentToken.type != TokenType::Number) throw std::runtime_error("Expected number for comparison in dynamic expression");
    expr->value_to_compare = std::stod(parser.currentToken.value);
    parser.advanceTokens();
    parser.expectToken(TokenType::QuestionMark);
    expr->true_branch = std::make_unique<StyleValue>(parseConditionalExpr(parser));
    parser.expectToken(TokenType::Colon);
    expr->false_branch = std::make_unique<StyleValue>(parseConditionalExpr(parser));
    StyleValue result;
    result.type = StyleValue::DYNAMIC_CONDITIONAL;
    result.dynamic_expr = expr;
    return result;
}

StyleValue StyleBlockState::parseConditionalExpr(Parser& parser) {
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

StyleValue StyleBlockState::parseStyleExpression(Parser& parser) {
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

void StyleBlockState::applyStyleTemplateRecursive(
    Parser& parser,
    const std::string& ns,
    const std::string& templateName,
    std::map<std::string, std::string>& finalStyles,
    const std::vector<std::string>& deletedTemplates,
    std::vector<std::string>& visitedTemplates)
{
    for(const auto& visited : visitedTemplates) {
        if (visited == templateName) {
            throw std::runtime_error("Circular dependency detected in style template inheritance involving '" + templateName + "'.");
        }
    }
    visitedTemplates.push_back(templateName);

    for(const auto& deleted : deletedTemplates) {
        if (deleted == templateName) {
            return;
        }
    }

    StyleTemplateNode* tmpl = parser.templateManager.getStyleTemplate(ns, templateName);
    if (!tmpl) {
        throw std::runtime_error("Style template '" + templateName + "' not found in namespace '" + ns + "'.");
    }

    for (const auto& parentName : tmpl->parentNames) {
        applyStyleTemplateRecursive(parser, ns, parentName, finalStyles, deletedTemplates, visitedTemplates);
    }

    for (const auto& pair : tmpl->styles) {
        finalStyles[pair.first] = pair.second;
    }

    visitedTemplates.pop_back();
}

void StyleBlockState::parseStyleTemplateUsage(Parser& parser) {
    parser.expectToken(TokenType::At);
    if (parser.currentToken.value != "Style") throw std::runtime_error("Expected 'Style' after '@' for template usage.");
    parser.expectToken(TokenType::Identifier);

    std::string templateName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

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

    StyleTemplateNode* tmpl = parser.templateManager.getStyleTemplate(ns, templateName);
    if (!tmpl) throw std::runtime_error("Style template not found: " + templateName);

    std::map<std::string, std::string> finalStyles;
    std::vector<std::string> deletedTemplates;
    std::map<std::string, StyleValue> specializedStyles;
    std::vector<std::string> completedProperties;

    if (parser.currentToken.type == TokenType::OpenBrace) {
        parser.expectToken(TokenType::OpenBrace);
        while (parser.currentToken.type != TokenType::CloseBrace) {
            if (parser.tryExpectKeyword(TokenType::Delete, "KEYWORD_DELETE", "delete")) {
                while(parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::CloseBrace) {
                    if (parser.currentToken.type == TokenType::At) {
                        parser.advanceTokens();
                        if (parser.currentToken.value != "Style") throw std::runtime_error("Can only delete @Style templates.");
                        parser.advanceTokens();
                        deletedTemplates.push_back(parser.currentToken.value);
                        parser.advanceTokens();
                    } else {
                        specializedStyles[parser.currentToken.value] = StyleValue(StyleValue::DELETED);
                        parser.advanceTokens();
                    }

                    if (parser.currentToken.type == TokenType::Comma) {
                        parser.advanceTokens();
                    } else {
                        break;
                    }
                }
                parser.expectToken(TokenType::Semicolon);
            } else {
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

    std::vector<std::string> visited;
    applyStyleTemplateRecursive(parser, ns, templateName, finalStyles, deletedTemplates, visited);

    for (const auto& pair : specializedStyles) {
        if (pair.second.type == StyleValue::DELETED) {
            finalStyles.erase(pair.first);
        } else {
            finalStyles[pair.first] = styleValueToString(pair.second);
        }
    }

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

    for (const auto& pair : finalStyles) {
        StyleValue sv;
        sv.type = StyleValue::STRING;
        sv.string_val = pair.second;
        parser.contextNode->inlineStyles[pair.first] = sv;
    }
}