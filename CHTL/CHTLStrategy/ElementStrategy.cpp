#include "ElementStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLStrategy/StyleStrategy.h"
#include "../CHTLStrategy/ScriptStrategy.h"
#include "../CHTLStrategy/TextNodeStrategy.h"
#include "../CHTLStrategy/AttributeStrategy.h"
#include "../CHTLStrategy/TemplateUsageStrategy.h"
#include "IfStrategy.h"
#include "../CHTLLexer/Token.h"
#include <stdexcept>

namespace CHTL {

std::unique_ptr<BaseNode> ElementStrategy::parse(CHTLParser& parser) {
    Token tag = parser.consume(TokenType::IDENTIFIER, "Expect element tag name.");
    auto element = std::make_unique<ElementNode>(tag.value);
    parser.consume(TokenType::L_BRACE, "Expect '{' after element tag name.");

    parser.pushNode(element.get());

    while (!parser.check(TokenType::R_BRACE) && !parser.isAtEnd()) {
        if (parser.match(TokenType::STYLE_KEYWORD)) {
            StyleStrategy strategy;
            element->setStyle(std::unique_ptr<StyleNode>(static_cast<StyleNode*>(strategy.parse(parser).release())));
        } else if (parser.match(TokenType::SCRIPT_KEYWORD)) {
            ScriptStrategy strategy;
            element->setScript(std::unique_ptr<ScriptNode>(static_cast<ScriptNode*>(strategy.parse(parser).release())));
        } else if (parser.match(TokenType::TEXT_KEYWORD)) {
            TextNodeStrategy strategy;
            element->addChild(strategy.parse(parser));
        } else if (parser.match(TokenType::IF_KEYWORD)) {
            IfStrategy strategy;
            element->addChild(strategy.parse(parser));
        } else if (parser.match(TokenType::AT_SIGN)) {
            TemplateUsageStrategy strategy;
            element->addChild(strategy.parse(parser));
        }
        else if (parser.check(TokenType::IDENTIFIER)) {
            if (parser.peekNext().type == TokenType::COLON || parser.peekNext().type == TokenType::EQUAL) {
                AttributeStrategy strategy;
                strategy.parse(parser); // Modifies the current node on the stack
            } else {
                ElementStrategy strategy; // Recursive call for nested elements
                element->addChild(strategy.parse(parser));
            }
        } else {
            throw std::runtime_error("Unexpected token in element block: " + parser.peek().value);
        }
    }

    parser.consume(TokenType::R_BRACE, "Expect '}' after element block.");

    parser.popNode();

    return element;
}

} // namespace CHTL