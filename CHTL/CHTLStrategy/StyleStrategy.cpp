#include "StyleStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/SelectorBlockNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLStrategy/TemplateUsageStrategy.h"
#include "../CHTLLexer/Token.h"
#include <stdexcept>

namespace CHTL {

std::unique_ptr<BaseNode> StyleStrategy::parse(CHTLParser& parser) {
    auto styleNode = std::make_unique<StyleNode>();
    parser.consume(TokenType::L_BRACE, "Expect '{' after 'style' keyword.");

    BaseNode* current_node = parser.currentNode();
    if (!current_node) {
        throw std::runtime_error("Style block must be inside an element.");
    }
    auto* parent = dynamic_cast<ElementNode*>(current_node);
     if (!parent) {
        throw std::runtime_error("Style block must be inside an element.");
    }

    std::string last_selector;

    while (!parser.check(TokenType::R_BRACE) && !parser.isAtEnd()) {
        if (parser.match(TokenType::AT_SIGN)) {
            TemplateUsageStrategy strategy;
            styleNode->addTemplateUsage(
                 std::unique_ptr<TemplateUsageNode>(static_cast<TemplateUsageNode*>(strategy.parse(parser).release()))
             );
        }
        else if (parser.match(TokenType::DOT)) {
            Token name = parser.consume(TokenType::IDENTIFIER, "Expect class name after '.'.");
            last_selector = "." + name.value;
            parent->addAttribute("class", name.value);
            auto selectorBlock = std::make_unique<SelectorBlockNode>(last_selector);
            parser.consume(TokenType::L_BRACE, "Expect '{' after selector name.");
            while (!parser.check(TokenType::R_BRACE) && !parser.isAtEnd()) {
                Token key = parser.consume(TokenType::IDENTIFIER, "Expect style property key.");
                parser.consume(TokenType::COLON, "Expect ':' after style property key.");
                selectorBlock->addProperty(std::make_unique<StylePropertyNode>(key.value, parser.parseExpression()));
                parser.consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            }
            parser.consume(TokenType::R_BRACE, "Expect '}' after selector block.");
            styleNode->addSelectorBlock(std::move(selectorBlock));
        } else if (parser.match(TokenType::HASH)) {
            Token name = parser.consume(TokenType::IDENTIFIER, "Expect id name after '#'.");
            last_selector = "#" + name.value;
            parent->addAttribute("id", name.value);
            auto selectorBlock = std::make_unique<SelectorBlockNode>(last_selector);
            parser.consume(TokenType::L_BRACE, "Expect '{' after selector name.");
            while (!parser.check(TokenType::R_BRACE) && !parser.isAtEnd()) {
                Token key = parser.consume(TokenType::IDENTIFIER, "Expect style property key.");
                parser.consume(TokenType::COLON, "Expect ':' after style property key.");
                selectorBlock->addProperty(std::make_unique<StylePropertyNode>(key.value, parser.parseExpression()));
                parser.consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            }
            parser.consume(TokenType::R_BRACE, "Expect '}' after selector block.");
            styleNode->addSelectorBlock(std::move(selectorBlock));
        } else if (parser.match(TokenType::AMPERSAND)) {
            if (last_selector.empty()) throw std::runtime_error("Found '&' without a preceding selector context.");
            std::string pseudo_selector_part;
            while (!parser.check(TokenType::L_BRACE) && !parser.isAtEnd()) {
                pseudo_selector_part += parser.advance().value;
            }
            std::string full_selector = last_selector + pseudo_selector_part;
            auto selectorBlock = std::make_unique<SelectorBlockNode>(full_selector);
            parser.consume(TokenType::L_BRACE, "Expect '{' after selector.");
            while (!parser.check(TokenType::R_BRACE) && !parser.isAtEnd()) {
                Token key = parser.consume(TokenType::IDENTIFIER, "Expect style property key.");
                parser.consume(TokenType::COLON, "Expect ':' after style property key.");
                selectorBlock->addProperty(std::make_unique<StylePropertyNode>(key.value, parser.parseExpression()));
                parser.consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            }
            parser.consume(TokenType::R_BRACE, "Expect '}' after selector block.");
            styleNode->addSelectorBlock(std::move(selectorBlock));
        }
        else if (parser.check(TokenType::IDENTIFIER)) {
            Token key = parser.consume(TokenType::IDENTIFIER, "Expect style property key.");
            parser.consume(TokenType::COLON, "Expect ':' after style property key.");
            auto value_expr = parser.parseExpression();
            styleNode->addProperty(std::make_unique<StylePropertyNode>(key.value, std::move(value_expr)));
            parser.consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
        } else {
            throw std::runtime_error("Unexpected token in style block: " + parser.peek().value);
        }
    }

    parser.consume(TokenType::R_BRACE, "Expect '}' after style block.");
    return styleNode;
}

} // namespace CHTL