#include "StyleBlockState.h"

#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleValue.h"
#include "../CHTLNode/DynamicStyleNode.h"
#include "../CHTLNode/StaticStyleNode.h"
#include "../Util/StyleUtil.h"
#include "../Util/ASTUtil.h"
#include <cmath>
#include <stdexcept>
#include <sstream>

std::unique_ptr<BaseNode> StyleBlockState::handle(Parser& parser) {
    if (!parser.contextNode) {
        throw std::runtime_error("StyleBlockState requires a context node.");
    }
    parser.expectToken(TokenType::Identifier); // "style"
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

    auto valueNode = parseStyleExpression(parser);
    parser.contextNode->inlineStyles[key] = std::move(valueNode);

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
            parser.contextNode->attributes["class"] = std::make_unique<StaticStyleNode>(selectorName);
        }
    } else if (parser.currentToken.type == TokenType::Hash) {
        selector += "#";
        parser.advanceTokens();
        if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected identifier after '#' for ID selector.");
        selectorName = parser.currentToken.value;
        if (!parser.configManager.getActiveConfig()->disableStyleAutoAddId && !parser.contextNode->attributes.count("id")) {
            parser.contextNode->attributes["id"] = std::make_unique<StaticStyleNode>(selectorName);
        }
    }
    selector += selectorName;
    parser.advanceTokens();

    std::string cssRules = parseCssRuleBlock(parser);
    parser.globalStyleContent += selector + " {\n" + cssRules + "}\n\n";

    std::string baseSelector;
    if (parser.contextNode->attributes.count("class")) {
        baseSelector = "." + parser.contextNode->attributes.at("class")->toString();
    } else if (parser.contextNode->attributes.count("id")) {
        baseSelector = "#" + parser.contextNode->attributes.at("id")->toString();
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
        baseSelector = "." + parser.contextNode->attributes.at("class")->toString();
    } else if (parser.contextNode->attributes.count("id")) {
        baseSelector = "#" + parser.contextNode->attributes.at("id")->toString();
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
        auto sv = parseStyleExpression(parser);
        std::string value = sv ? sv->toString() : "";
        cssRules += "  " + key + ": " + value + ";\n";
        if(parser.currentToken.type == TokenType::Semicolon) parser.advanceTokens();
    }
    parser.expectToken(TokenType::CloseBrace);
    return cssRules;
}

static bool isStyleValueTerminator(TokenType type) {
    switch (type) {
        case TokenType::Semicolon: case TokenType::Comma: case TokenType::CloseParen: case TokenType::CloseBrace:
        case TokenType::EndOfFile:
            return true;
        default:
            return false;
    }
}


std::unique_ptr<StyleValue> StyleBlockState::parsePrimaryExpr(Parser& parser) {
    if (parser.currentToken.type == TokenType::OpenParen) {
        parser.advanceTokens();
        auto result = parseStyleExpression(parser);
        parser.expectToken(TokenType::CloseParen);
        return result;
    }

    std::stringstream ss;
    while (!isStyleValueTerminator(parser.currentToken.type)) {
        ss << parser.currentToken.value;
        parser.advanceTokens();
        if (!isStyleValueTerminator(parser.currentToken.type)) {
            ss << " ";
        }
    }
    return std::make_unique<StaticStyleNode>(ss.str());
}

std::unique_ptr<StyleValue> StyleBlockState::parseStyleExpression(Parser& parser) {
    if (parser.currentToken.type == TokenType::OpenDoubleBrace) {
        std::stringstream expr_ss;
        int brace_level = 0;

        do {
            if (parser.currentToken.type == TokenType::OpenDoubleBrace) brace_level++;
            if (parser.currentToken.type == TokenType::CloseDoubleBrace) brace_level--;
            expr_ss << parser.currentToken.value << " ";
            parser.advanceTokens();
        } while (brace_level > 0 && parser.currentToken.type != TokenType::EndOfFile && parser.currentToken.type != TokenType::Semicolon);

        if (brace_level != 0) {
            throw std::runtime_error("Mismatched '{{' and '}}' in dynamic style expression.");
        }

        std::string final_expr = expr_ss.str();
        trim(final_expr);

        return std::make_unique<DynamicStyleNode>(final_expr);
    }

    return parsePrimaryExpr(parser);
}

void StyleBlockState::parseStyleTemplateUsage(Parser& parser) {
    // This function needs to be adapted to the new std::unique_ptr<StyleValue> map.
    // We'll just consume tokens to avoid breaking the parser for now.
    parser.advanceTokens(); // @
    parser.advanceTokens(); // Style
    parser.advanceTokens(); // templateName
    if (parser.currentToken.type == TokenType::Semicolon) {
        parser.advanceTokens();
    } else if (parser.currentToken.type == TokenType::OpenBrace) {
        parser.advanceTokens();
        while(parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
            parser.advanceTokens();
        }
        parser.expectToken(TokenType::CloseBrace);
    }
}