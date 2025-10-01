#include "StyleStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include <stdexcept>

namespace CHTL {

void StyleStrategy::execute(CHTLParser& parser) {
    // The state has already consumed the 'style' keyword.
    parser.consume(TokenType::L_BRACE, "Expected '{' after 'style' keyword.");

    auto styleNode = std::make_unique<StyleNode>();

    while (!parser.isAtEnd() && parser.peek().type != TokenType::R_BRACE) {
        // Parse a style property (e.g., "color: red;")
        Token key = parser.consume(TokenType::IDENTIFIER, "Expected style property key.");
        parser.consume(TokenType::COLON, "Expected ':' after style property key.");

        auto valueExpression = parser.parseExpression();

        parser.consume(TokenType::SEMICOLON, "Expected ';' after style property value.");

        auto propertyNode = std::make_unique<StylePropertyNode>(key.value, std::move(valueExpression));
        styleNode->addProperty(std::move(propertyNode));
    }

    parser.consume(TokenType::R_BRACE, "Expected '}' after style block.");

    // Attach the completed StyleNode to the current element.
    ElementNode* currentNode = parser.getCurrentNode();
    if (currentNode) {
        currentNode->setStyle(std::move(styleNode));
    } else {
        throw std::runtime_error("Cannot add style block, no current element node.");
    }
}

} // namespace CHTL