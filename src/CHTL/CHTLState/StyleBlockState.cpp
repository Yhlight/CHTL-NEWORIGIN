#include "StyleBlockState.h"

#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleValue.h"
#include "../Util/StyleUtil.h"
#include "../Util/ASTUtil.h"
#include <stdexcept>
#include <sstream>
#include <cmath>

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
    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::Colon);
    StyleValue value = parseStyleExpression(parser);
    if (value.type != StyleValue::EMPTY) {
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

void StyleBlockState::parseAmpersandSelector(Parser& parser) {
    std::string baseSelector;
    if (parser.contextNode->attributes.count("class")) {
        baseSelector = "." + parser.contextNode->attributes["class"];
    } else if (parser.contextNode->attributes.count("id")) {
        baseSelector = "#" + parser.contextNode->attributes["id"];
    } else {
        throw std::runtime_error("Cannot use '&' selector because the parent element has no class or id defined.");
    }
    parser.advanceTokens();
    std::string selectorSuffix;
    while(parser.currentToken.type != TokenType::OpenBrace && parser.currentToken.type != TokenType::EndOfFile) {
        selectorSuffix += parser.currentToken.value;
        parser.advanceTokens();
    }
    std::string finalSelector = baseSelector + selectorSuffix;
    std::string cssRules = parseCssRuleBlock(parser);
    parser.globalStyleContent += finalSelector + " {\n" + cssRules + "}\n\n";
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

Selector StyleBlockState::parseSelector(Parser& parser) {
    Selector selector;
    selector.parts.emplace_back();
    while (true) {
        SelectorPart& currentPart = selector.parts.back();
        if (parser.currentToken.type == TokenType::Dot) {
            parser.advanceTokens();
            if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected class name after '.'.");
            currentPart.className = parser.currentToken.value;
            parser.advanceTokens();
        } else if (parser.currentToken.type == TokenType::Hash) {
            parser.advanceTokens();
            if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected id name after '#'.");
            currentPart.id = parser.currentToken.value;
            parser.advanceTokens();
        } else if (parser.currentToken.type == TokenType::Identifier) {
            currentPart.tagName = parser.currentToken.value;
            parser.advanceTokens();
        }
        if (parser.currentToken.type == TokenType::OpenBracket) {
            parser.advanceTokens();
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
        auto it = parser.contextNode->inlineStyles.find(parser.currentToken.value);
        if (it != parser.contextNode->inlineStyles.end()) {
            parser.advanceTokens();
            return it->second;
        }
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
        if (isSpecialized) {
            return {StyleValue::STRING, 0.0, "", specializedValue};
        }
        std::string ns = parser.getCurrentNamespace();
        auto result = parser.templateManager.getVariableFromTemplate(ns, templateName, varName);
        if (!result.first) {
            throw std::runtime_error("Variable '" + varName + "' not found in template '" + templateName + "' or its parents.");
        }
        return {StyleValue::STRING, 0.0, "", result.second};
    }

    if (parser.currentToken.type == TokenType::Dot || parser.currentToken.type == TokenType::Hash ||
       (parser.currentToken.type == TokenType::Identifier && parser.contextNode->inlineStyles.find(parser.currentToken.value) == parser.contextNode->inlineStyles.end()))
    {
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
        auto result = parseStyleExpression(parser);
        parser.expectToken(TokenType::CloseParen);
        return result;
    }

    if (parser.currentToken.type == TokenType::Identifier || parser.currentToken.type == TokenType::String) {
        std::stringstream ss;
        ss << parser.currentToken.value;
        parser.advanceTokens();
        while (parser.currentToken.type == TokenType::Identifier || parser.currentToken.type == TokenType::Number || parser.currentToken.type == TokenType::String) {
             ss << " " << parser.currentToken.value;
             parser.advanceTokens();
        }
        std::string full_literal = ss.str();
        StyleValue decoupled = decoupleNumericValue(full_literal);
        if (decoupled.type == StyleValue::NUMERIC) {
            return decoupled;
        }
        return {StyleValue::STRING, 0.0, "", full_literal};
    }

    throw std::runtime_error("Unexpected token in expression: " + parser.currentToken.value);
}

StyleValue StyleBlockState::parsePowerExpr(Parser& parser) {
    StyleValue result = parsePrimaryExpr(parser);
    if (parser.currentToken.type == TokenType::Power) {
        if (result.type != StyleValue::NUMERIC) throw std::runtime_error("LHS of ** must be numeric.");
        parser.advanceTokens();
        StyleValue rhs = parsePowerExpr(parser);
        if (rhs.type != StyleValue::NUMERIC) throw std::runtime_error("RHS of ** must be numeric.");
        if (!rhs.unit.empty()) {
            throw std::runtime_error("The exponent of the power operator (**) must be a unitless number.");
        }
        result.numeric_val = std::pow(result.numeric_val, rhs.numeric_val);
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
        if (op == TokenType::Asterisk) {
            if (!result.unit.empty() && !rhs.unit.empty()) {
                throw std::runtime_error("Cannot multiply two values with units.");
            }
            result.numeric_val *= rhs.numeric_val;
            if (result.unit.empty()) {
                result.unit = rhs.unit;
            }
        } else if (op == TokenType::Slash) {
            if (!result.unit.empty() && !rhs.unit.empty()) {
                if (result.unit == rhs.unit) {
                    result.unit = "";
                } else {
                    throw std::runtime_error("Cannot divide values with different units.");
                }
            }
            if (rhs.numeric_val == 0) throw std::runtime_error("Division by zero.");
            result.numeric_val /= rhs.numeric_val;
            if (result.unit.empty()) {
                result.unit = rhs.unit;
            }
        } else if (op == TokenType::Percent) {
            if (rhs.numeric_val == 0) throw std::runtime_error("Modulo by zero.");
            if (!result.unit.empty() && !rhs.unit.empty() && result.unit != rhs.unit) {
                throw std::runtime_error("Cannot perform modulo with different units.");
            }
            result.numeric_val = std::fmod(result.numeric_val, rhs.numeric_val);
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
        return condition.bool_val ? true_val : StyleValue{StyleValue::EMPTY};
    }
    return condition;
}

StyleValue StyleBlockState::parseStyleExpression(Parser& parser) {
    StyleValue finalValue{StyleValue::EMPTY};
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
        ns = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
    }
    StyleTemplateNode* tmpl = parser.templateManager.getStyleTemplate(ns, templateName);
    if (!tmpl) throw std::runtime_error("Style template not found: " + templateName);
    std::map<std::string, std::string> finalStyles;
    std::vector<std::string> deletedTemplates;
    std::map<std::string, StyleValue> specializedStyles;
    std::vector<std::string> completedProperties;
    if (tmpl->isCustom && parser.currentToken.type == TokenType::OpenBrace) {
        parser.expectToken(TokenType::OpenBrace);
        while (parser.currentToken.type != TokenType::CloseBrace) {
            if (parser.currentToken.type == TokenType::Delete) {
                parser.advanceTokens();
                while(parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::CloseBrace) {
                    if (parser.currentToken.type == TokenType::At) {
                        parser.advanceTokens();
                        if (parser.currentToken.value != "Style") throw std::runtime_error("Can only delete @Style templates.");
                        parser.advanceTokens();
                        deletedTemplates.push_back(parser.currentToken.value);
                        parser.advanceTokens();
                    } else {
                        specializedStyles[parser.currentToken.value] = { StyleValue::DELETED };
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
