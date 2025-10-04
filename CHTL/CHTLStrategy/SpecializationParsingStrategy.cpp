#include "SpecializationParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/DeleteNode.h"
#include "../CHTLNode/InsertNode.h"
#include "ElementParsingStrategy.h"
#include <stdexcept>

namespace CHTL {

std::shared_ptr<BaseNode> SpecializationParsingStrategy::parse(CHTLParserContext* context) {
    if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE) {
        return nullptr;
    }
    context->advance(); // consume '{'

    auto specializationRoot = std::make_shared<ElementNode>("specialization-root");

    while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
        TokenType currentType = context->getCurrentToken().type;

        if (currentType == TokenType::TOKEN_DELETE) {
            context->advance(); // consume 'delete'
            auto deleteNode = std::make_shared<DeleteNode>();

            while(context->getCurrentToken().type != TokenType::TOKEN_SEMICOLON && !context->isAtEnd()){
                deleteNode->addTarget(context->getCurrentToken().lexeme);
                context->advance();
                if(context->getCurrentToken().type == TokenType::TOKEN_COMMA){
                    context->advance();
                }
            }
            if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
                context->advance();
            }
            specializationRoot->addChild(deleteNode);

        } else if (currentType == TokenType::TOKEN_INSERT) {
            context->advance(); // consume 'insert'
            auto insertNode = std::make_shared<InsertNode>();

            std::string position_str = context->getCurrentToken().lexeme;
            if (position_str == "after") insertNode->position = InsertPosition::AFTER;
            else if (position_str == "before") insertNode->position = InsertPosition::BEFORE;
            else if (position_str == "replace") insertNode->position = InsertPosition::REPLACE;
            else if (position_str == "at") {
                context->advance();
                std::string next_word = context->getCurrentToken().lexeme;
                if(next_word == "top") insertNode->position = InsertPosition::AT_TOP;
                else if(next_word == "bottom") insertNode->position = InsertPosition::AT_BOTTOM;
            }
            context->advance();

            if (insertNode->position == InsertPosition::AFTER || insertNode->position == InsertPosition::BEFORE || insertNode->position == InsertPosition::REPLACE) {
                std::string selector;
                while(context->getCurrentToken().type != TokenType::TOKEN_LBRACE && !context->isAtEnd()) {
                    selector += context->getCurrentToken().lexeme;
                    context->advance();
                }
                insertNode->target_selector = selector;
            }

            if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE) {
                throw std::runtime_error("Expected '{' for insert block.");
            }
            context->advance(); // consume '{'

            while(context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
                 context->setStrategy(std::make_unique<ElementParsingStrategy>());
                 insertNode->addChild(context->runCurrentStrategy());
            }

            if (context->getCurrentToken().type != TokenType::TOKEN_RBRACE) {
                throw std::runtime_error("Expected '}' to close insert block.");
            }
            context->advance(); // consume '}'

            specializationRoot->addChild(insertNode);

        } else {
            context->setStrategy(std::make_unique<ElementParsingStrategy>());
            specializationRoot->addChild(context->runCurrentStrategy());
        }
    }

    if (context->getCurrentToken().type != TokenType::TOKEN_RBRACE) {
        throw std::runtime_error("Expected '}' to close specialization block.");
    }
    context->advance(); // consume '}'

    return specializationRoot;
}

}