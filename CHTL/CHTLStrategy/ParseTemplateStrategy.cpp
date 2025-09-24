#include "ParseTemplateStrategy.h"
#include "../CHTLContext/Context.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/TemplateNode.h"
#include <iostream>
#include <sstream>

namespace CHTL {

void ParseTemplateStrategy::Execute(Context* context, Parser& parser) {
    if (parser.CurrentToken().type != TokenType::LEFT_BRACKET) return;
    parser.ConsumeToken();

    if (parser.CurrentToken().type != TokenType::IDENTIFIER || parser.CurrentToken().lexeme != "Template") return;
    parser.ConsumeToken();

    if (parser.CurrentToken().type != TokenType::RIGHT_BRACKET) return;
    parser.ConsumeToken();

    if (parser.CurrentToken().type != TokenType::AT) return;
    parser.ConsumeToken();

    if (parser.CurrentToken().type != TokenType::IDENTIFIER) return;
    std::string templateType = parser.CurrentToken().lexeme;
    parser.ConsumeToken();

    if (parser.CurrentToken().type != TokenType::IDENTIFIER) return;
    std::string templateName = parser.CurrentToken().lexeme;
    parser.ConsumeToken();

    if (parser.CurrentToken().type != TokenType::LEFT_BRACE) return;
    parser.ConsumeToken();

    if (templateType == "Style") {
        TemplateStyleGroup styleGroup(templateName);
        while (parser.CurrentToken().type != TokenType::RIGHT_BRACE && !parser.IsAtEnd()) {
            if (parser.CurrentToken().type == TokenType::IDENTIFIER && parser.PeekNextToken().type == TokenType::COLON) {
                std::string prop = parser.CurrentToken().lexeme;
                parser.ConsumeToken(); parser.ConsumeToken(); // prop and :

                std::stringstream valueBuilder;
                while(parser.CurrentToken().type != TokenType::SEMICOLON && parser.CurrentToken().type != TokenType::RIGHT_BRACE && !parser.IsAtEnd()) {
                    valueBuilder << parser.CurrentToken().lexeme;
                    parser.ConsumeToken();
                }
                if (parser.CurrentToken().type == TokenType::SEMICOLON) parser.ConsumeToken();
                styleGroup.AddProperty({prop, valueBuilder.str()});
            } else {
                 parser.ConsumeToken(); // Skip unexpected
            }
        }
        context->namespaces[context->currentNamespace].styleTemplates.emplace(templateName, styleGroup);

    } else if (templateType == "Var") {
        TemplateVarGroup varGroup(templateName);
        while (parser.CurrentToken().type != TokenType::RIGHT_BRACE && !parser.IsAtEnd()) {
            if (parser.CurrentToken().type == TokenType::IDENTIFIER && parser.PeekNextToken().type == TokenType::COLON) {
                std::string key = parser.CurrentToken().lexeme;
                parser.ConsumeToken(); parser.ConsumeToken(); // key and :

                std::stringstream valueBuilder;
                while(parser.CurrentToken().type != TokenType::SEMICOLON && parser.CurrentToken().type != TokenType::RIGHT_BRACE && !parser.IsAtEnd()) {
                    valueBuilder << parser.CurrentToken().lexeme;
                    parser.ConsumeToken();
                }
                if (parser.CurrentToken().type == TokenType::SEMICOLON) parser.ConsumeToken();
                varGroup.AddVar(key, valueBuilder.str());
            } else {
                parser.ConsumeToken(); // Skip unexpected
            }
        }
        context->namespaces[context->currentNamespace].varTemplates.emplace(templateName, varGroup);
    } else if (templateType == "Element") {
        TemplateElementGroup elementGroup(templateName);
        std::vector<Token> elementTokens;
        int braceLevel = 1;
        while(braceLevel > 0 && !parser.IsAtEnd()) {
            if (parser.CurrentToken().type == TokenType::LEFT_BRACE) braceLevel++;
            if (parser.CurrentToken().type == TokenType::RIGHT_BRACE) braceLevel--;
            if (braceLevel > 0) { // Don't include the final closing brace
                elementTokens.push_back(parser.CurrentToken());
            }
            parser.ConsumeToken();
        }
        elementGroup.SetTokens(std::move(elementTokens));
        context->namespaces[context->currentNamespace].elementTemplates.emplace(templateName, elementGroup);
        // The final '}' is already consumed by the loop, so we don't consume it again outside.
        return; // Return early
    }
    else {
        std::cerr << "Error: Unknown template type '@" << templateType << "'." << std::endl;
        // Skip until }
        while(parser.CurrentToken().type != TokenType::RIGHT_BRACE && !parser.IsAtEnd()) parser.ConsumeToken();
    }

    if (parser.CurrentToken().type == TokenType::RIGHT_BRACE) {
        parser.ConsumeToken();
    }
}

} // namespace CHTL
