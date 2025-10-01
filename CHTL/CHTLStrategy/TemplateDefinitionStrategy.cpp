#include "TemplateDefinitionStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLStrategy/ElementStrategy.h"
#include "../CHTLStrategy/TextNodeStrategy.h"
#include "../CHTLStrategy/TemplateUsageStrategy.h"
#include "../CHTLLexer/Token.h"
#include <stdexcept>

namespace CHTL {

std::unique_ptr<BaseNode> TemplateDefinitionStrategy::parse(CHTLParser& parser) {
    parser.consume(TokenType::L_BRACKET, "Expect '[' at the start of a template definition.");
    Token keyword = parser.consume(TokenType::IDENTIFIER, "Expect 'Template' keyword.");
    if (keyword.value != "Template") throw std::runtime_error("Expected 'Template' keyword.");
    parser.consume(TokenType::R_BRACKET, "Expect ']' after 'Template'.");
    parser.consume(TokenType::AT_SIGN, "Expect '@' after '[Template]'.");

    Token typeToken = parser.consume(TokenType::IDENTIFIER, "Expect template type (Style, Element, or Var).");
    TemplateType type;
    if (typeToken.value == "Style") type = TemplateType::STYLE;
    else if (typeToken.value == "Element") type = TemplateType::ELEMENT;
    else if (typeToken.value == "Var") type = TemplateType::VAR;
    else throw std::runtime_error("Unknown template type: " + typeToken.value);

    Token nameToken = parser.consume(TokenType::IDENTIFIER, "Expect template name.");
    auto templateNode = std::make_unique<TemplateDefinitionNode>(type, nameToken.value);

    parser.consume(TokenType::L_BRACE, "Expect '{' after template definition.");

    parser.pushNode(templateNode.get());

    while (!parser.check(TokenType::R_BRACE) && !parser.isAtEnd()) {
        if (type == TemplateType::ELEMENT) {
            if (parser.check(TokenType::IDENTIFIER)) {
                ElementStrategy strategy;
                templateNode->addChild(strategy.parse(parser));
            } else if (parser.match(TokenType::TEXT_KEYWORD)) {
                TextNodeStrategy strategy;
                templateNode->addChild(strategy.parse(parser));
            } else if (parser.match(TokenType::AT_SIGN)) {
                TemplateUsageStrategy strategy;
                templateNode->addChild(strategy.parse(parser));
            } else {
                 throw std::runtime_error("Unexpected token in element template block: " + parser.peek().value);
            }
        } else if (type == TemplateType::STYLE || type == TemplateType::VAR) {
            Token key = parser.consume(TokenType::IDENTIFIER, "Expect property or variable key.");
            parser.consume(TokenType::COLON, "Expect ':' after key.");

            std::unique_ptr<ExpressionNode> valueNode;
            if (parser.check(TokenType::HASH)) {
                Token hash = parser.advance();
                std::string hex_value_str;
                // Greedily consume adjacent identifier/number tokens to form the hex value
                while (!parser.isAtEnd() && (parser.check(TokenType::IDENTIFIER) || parser.check(TokenType::NUMBER))) {
                    hex_value_str += parser.advance().value;
                }

                if (hex_value_str.empty()) {
                    throw std::runtime_error("Expect hex value after '#'.");
                }

                valueNode = std::make_unique<LiteralNode>(Token{TokenType::STRING_LITERAL, hash.value + hex_value_str}, "");
            } else {
                valueNode = parser.parseExpression();
            }

            templateNode->addChild(std::make_unique<StylePropertyNode>(key.value, std::move(valueNode)));
            parser.consume(TokenType::SEMICOLON, "Expect ';' after value.");
        }
    }

    parser.consume(TokenType::R_BRACE, "Expect '}' after template block.");

    parser.popNode();

    return templateNode;
}

} // namespace CHTL