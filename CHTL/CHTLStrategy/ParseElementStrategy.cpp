#include "ParseElementStrategy.h"
#include "../CHTLContext/Context.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

namespace CHTL {

// Forward declarations
std::string ParseValue(Context* context, Parser& parser, const std::map<std::string, std::string>& computedStyles);
std::string ParseSingleValueComponent(Context* context, Parser& parser);
std::string ParseArithmeticValue(Context* context, Parser& parser);
std::string ParseConditionalValue(Context* context, Parser& parser, const std::map<std::string, std::string>& computedStyles);

std::string ParseSingleValueComponent(Context* context, Parser& parser) {
    if (parser.CurrentToken().type == TokenType::HASH) {
        parser.ConsumeToken();
        std::string hexValue = "#" + parser.CurrentToken().lexeme;
        parser.ConsumeToken();
        return hexValue;
    }
    if (parser.CurrentToken().type == TokenType::IDENTIFIER && parser.PeekNextToken().type == TokenType::LEFT_PAREN) {
        std::string varGroupName = parser.CurrentToken().lexeme;
        parser.ConsumeToken(); parser.ConsumeToken();
        std::string varName = parser.CurrentToken().lexeme;
        parser.ConsumeToken();
        if (parser.CurrentToken().type == TokenType::RIGHT_PAREN) parser.ConsumeToken();

        auto ns_it = context->namespaces.find(context->currentNamespace);
        if (ns_it != context->namespaces.end()) {
            auto group_it = ns_it->second.varTemplates.find(varGroupName);
            if (group_it != ns_it->second.varTemplates.end()) {
                auto var_it = group_it->second.GetVar(varName);
                if (var_it) return *var_it;
            }
        }
        if (context->currentNamespace != GLOBAL_NAMESPACE) {
            auto& global_ns = context->namespaces[GLOBAL_NAMESPACE];
            auto group_it = global_ns.varTemplates.find(varGroupName);
            if (group_it != global_ns.varTemplates.end()) {
                auto var_it = group_it->second.GetVar(varName);
                if (var_it) return *var_it;
            }
        }
        return "VAR_NOT_FOUND";
    }
    if (parser.CurrentToken().type == TokenType::STRING || parser.CurrentToken().type == TokenType::IDENTIFIER || parser.CurrentToken().type == TokenType::NUMBER) {
        std::string value = parser.CurrentToken().lexeme;
        parser.ConsumeToken();
        return value;
    }
    return "";
}

struct Term { double value = 0.0; std::string unit = ""; };

Term ParseTerm(Context* context, Parser& parser) {
    Term term;
    std::string valueStr = ParseSingleValueComponent(context, parser);
    size_t i = 0;
    while (i < valueStr.length() && (isdigit(valueStr[i]) || valueStr[i] == '.')) i++;
    term.value = std::atof(valueStr.substr(0, i).c_str());
    term.unit = valueStr.substr(i);
    return term;
}

std::string ParseArithmeticValue(Context* context, Parser& parser) {
    Term left = ParseTerm(context, parser);
    while (parser.CurrentToken().type == TokenType::PLUS || parser.CurrentToken().type == TokenType::MINUS ||
           parser.CurrentToken().type == TokenType::STAR || parser.CurrentToken().type == TokenType::SLASH) {
        TokenType op = parser.CurrentToken().type;
        parser.ConsumeToken();
        Term right = ParseTerm(context, parser);
        if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) return "ERROR_UNIT_MISMATCH";
        std::string new_unit = !left.unit.empty() ? left.unit : right.unit;
        switch(op) {
            case TokenType::PLUS:  left.value += right.value; break;
            case TokenType::MINUS: left.value -= right.value; break;
            case TokenType::STAR:  left.value *= right.value; break;
            case TokenType::SLASH: left.value = (right.value != 0) ? (left.value / right.value) : 0; break;
            default: break;
        }
        left.unit = new_unit;
    }
    std::stringstream result;
    result << left.value << left.unit;
    return result.str();
}

std::string ParseConditionalValue(Context* context, Parser& parser, const std::map<std::string, std::string>& computedStyles) {
    std::string lhs_prop_name = parser.CurrentToken().lexeme;
    parser.ConsumeToken();
    if (parser.CurrentToken().type != TokenType::GREATER) return "ERROR_CONDITIONAL_SYNTAX";
    parser.ConsumeToken();
    Term rhs_term = ParseTerm(context, parser);
    if (parser.CurrentToken().type != TokenType::QUESTION) return "ERROR_CONDITIONAL_SYNTAX";
    parser.ConsumeToken();
    std::string true_val = ParseSingleValueComponent(context, parser);
    if (parser.CurrentToken().type != TokenType::COLON) return "ERROR_CONDITIONAL_SYNTAX";
    parser.ConsumeToken();
    std::string false_val = ParseSingleValueComponent(context, parser);

    auto it = computedStyles.find(lhs_prop_name);
    if (it == computedStyles.end()) return "ERROR_UNKNOWN_PROPERTY";
    double lhs_val = std::atof(it->second.c_str());
    return (lhs_val > rhs_term.value) ? true_val : false_val;
}

std::string ParseValue(Context* context, Parser& parser, const std::map<std::string, std::string>& computedStyles) {
    // Lookahead to decide what kind of expression this is.
    int temp_idx = 0;
    bool isConditional = false;
    bool isArithmetic = false;
    while(true) {
        const Token& t = parser.PeekTokenAt(temp_idx);
        if (t.type == TokenType::SEMICOLON || t.type == TokenType::RIGHT_BRACE || t.type == TokenType::END_OF_FILE) break;
        if (t.type == TokenType::QUESTION) { isConditional = true; break; }
        if (t.type == TokenType::PLUS || t.type == TokenType::MINUS || t.type == TokenType::STAR || t.type == TokenType::SLASH) {
            isArithmetic = true;
        }
        temp_idx++;
    }

    if (isConditional) {
        return ParseConditionalValue(context, parser, computedStyles);
    } else if (isArithmetic) {
        return ParseArithmeticValue(context, parser);
    } else {
        return ParseSingleValueComponent(context, parser);
    }
}

void ParseAttribute(Context* context, Parser& parser) {
    std::string attrName = parser.CurrentToken().lexeme;
    parser.ConsumeToken();
    if (parser.CurrentToken().type != TokenType::COLON) return;
    parser.ConsumeToken();
    std::string attrValue = ParseSingleValueComponent(context, parser);
    if (parser.CurrentToken().type != TokenType::SEMICOLON) return;
    parser.ConsumeToken();
    static_cast<ElementNode*>(context->GetCurrentParent())->attributes[attrName] = attrValue;
}

void ParseStyleBlock(Context* context, Parser& parser) {
    parser.ConsumeToken();
    if (parser.CurrentToken().type != TokenType::LEFT_BRACE) return;
    parser.ConsumeToken();
    std::map<std::string, std::string> computedStyles;
    std::stringstream styleString;
    while (parser.CurrentToken().type != TokenType::RIGHT_BRACE && !parser.IsAtEnd()) {
        if (parser.CurrentToken().type == TokenType::IDENTIFIER && parser.PeekNextToken().type == TokenType::COLON) {
            std::string prop = parser.CurrentToken().lexeme;
            parser.ConsumeToken(); parser.ConsumeToken();
            std::string value = ParseValue(context, parser, computedStyles);
            computedStyles[prop] = value;
            styleString << prop << ": " << value << "; ";
            if (parser.CurrentToken().type == TokenType::SEMICOLON) parser.ConsumeToken();
        } else if (parser.CurrentToken().type == TokenType::AT) {
             parser.ConsumeToken();
             if (parser.CurrentToken().lexeme == "Style") {
                 parser.ConsumeToken();
                 std::string templateName = parser.CurrentToken().lexeme;
                 parser.ConsumeToken();
                 if (parser.CurrentToken().type == TokenType::SEMICOLON) parser.ConsumeToken();

                 const TemplateStyleGroup* foundTemplate = nullptr;
                 auto ns_it = context->namespaces.find(context->currentNamespace);
                 if (ns_it != context->namespaces.end()) {
                    auto it = ns_it->second.styleTemplates.find(templateName);
                    if (it != ns_it->second.styleTemplates.end()) foundTemplate = &it->second;
                 }
                if (!foundTemplate && context->currentNamespace != GLOBAL_NAMESPACE) {
                    auto& global_ns = context->namespaces[GLOBAL_NAMESPACE];
                    auto it = global_ns.styleTemplates.find(templateName);
                    if (it != global_ns.styleTemplates.end()) foundTemplate = &it->second;
                }

                 if (foundTemplate) {
                     for (const auto& tmpl_prop : foundTemplate->GetProperties()) {
                         styleString << tmpl_prop.name << ": " << tmpl_prop.value << "; ";
                         computedStyles[tmpl_prop.name] = tmpl_prop.value;
                     }
                 } else {
                    std::cerr << "Error: Style template '" << templateName << "' not found." << std::endl;
                 }
             }
        } else {
            parser.ConsumeToken();
        }
    }
    if (parser.CurrentToken().type == TokenType::RIGHT_BRACE) parser.ConsumeToken();
    static_cast<ElementNode*>(context->GetCurrentParent())->attributes["style"] = styleString.str();
}

void ParseElementStrategy::Execute(Context* context, Parser& parser) {
    if (parser.CurrentToken().type != TokenType::IDENTIFIER) return;
    std::string tagName = parser.CurrentToken().lexeme;
    parser.ConsumeToken();
    if (tagName == "text") {
        if (parser.CurrentToken().type == TokenType::LEFT_BRACE) {
            parser.ConsumeToken();
            auto textNode = std::make_unique<TextNode>(ParseSingleValueComponent(context, parser));
            static_cast<ElementNode*>(context->GetCurrentParent())->children.push_back(std::move(textNode));
            if (parser.CurrentToken().type == TokenType::RIGHT_BRACE) parser.ConsumeToken();
        }
        return;
    }
    auto elementNode = std::make_unique<ElementNode>(tagName);
    Node* newNodePtr = elementNode.get();
    static_cast<ElementNode*>(context->GetCurrentParent())->children.push_back(std::move(elementNode));
    if (parser.CurrentToken().type != TokenType::LEFT_BRACE) return;
    parser.ConsumeToken();
    context->PushNode(newNodePtr);
    while (parser.CurrentToken().type != TokenType::RIGHT_BRACE && !parser.IsAtEnd()) {
        if (parser.CurrentToken().type == TokenType::IDENTIFIER) {
            if (parser.PeekNextToken().type == TokenType::COLON) {
                ParseAttribute(context, parser);
            } else if (parser.CurrentToken().lexeme == "style") {
                ParseStyleBlock(context, parser);
            }
            else {
                context->Request(parser);
            }
        } else if (parser.CurrentToken().type == TokenType::COMMENT) {
            auto commentNode = std::make_unique<CommentNode>(parser.CurrentToken().lexeme);
            static_cast<ElementNode*>(context->GetCurrentParent())->children.push_back(std::move(commentNode));
            parser.ConsumeToken();
        } else if (parser.CurrentToken().type == TokenType::AT) {
             parser.ConsumeToken();
             if (parser.CurrentToken().lexeme == "Element") {
                 parser.ConsumeToken();
                 std::string templateName = parser.CurrentToken().lexeme;
                 parser.ConsumeToken();
                 if (parser.CurrentToken().type == TokenType::SEMICOLON) parser.ConsumeToken();
                 const TemplateElementGroup* foundTemplate = nullptr;
                 auto ns_it = context->namespaces.find(context->currentNamespace);
                 if (ns_it != context->namespaces.end()) {
                    auto it = ns_it->second.elementTemplates.find(templateName);
                    if (it != ns_it->second.elementTemplates.end()) foundTemplate = &it->second;
                 }
                if (!foundTemplate && context->currentNamespace != GLOBAL_NAMESPACE) {
                    auto& global_ns = context->namespaces[GLOBAL_NAMESPACE];
                    auto it = global_ns.elementTemplates.find(templateName);
                    if (it != global_ns.elementTemplates.end()) foundTemplate = &it->second;
                }
                 if (foundTemplate) {
                     parser.InjectTokens(foundTemplate->GetTokens());
                 } else {
                    std::cerr << "Error: Element template '" << templateName << "' not found." << std::endl;
                 }
             }
        } else {
            parser.ConsumeToken();
        }
    }
    if (parser.CurrentToken().type == TokenType::RIGHT_BRACE) parser.ConsumeToken();
    context->PopNode();
}

} // namespace CHTL
