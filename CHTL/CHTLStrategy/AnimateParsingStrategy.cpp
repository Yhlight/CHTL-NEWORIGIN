#include "AnimateParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/PropertyNode.h"
#include <stdexcept>

namespace CHTL {

    std::shared_ptr<BaseNode> AnimateParsingStrategy::parse(CHTLParserContext* context) {
        auto animateNode = std::make_shared<AnimateNode>();
        context->advance(); // Consume "Animate"

        if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE) {
             throw std::runtime_error("Expected '{' after 'Animate'");
        }
        context->advance(); // Consume "{"

        while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE) {
            if (context->getCurrentToken().type != TokenType::TOKEN_IDENTIFIER) {
                throw std::runtime_error("Expected identifier in Animate block");
            }

            std::string key = context->getCurrentToken().lexeme;
            context->advance();

            if (context->getCurrentToken().type != TokenType::TOKEN_COLON) {
                 throw std::runtime_error("Expected ':' after key in Animate block");
            }
            context->advance(); // consume ':'

            if (key == "target") {
                animateNode->target = context->getCurrentToken().lexeme;
                context->advance();
            } else if (key == "duration") {
                animateNode->duration = context->getCurrentToken().lexeme;
                context->advance();
            } else if (key == "easing") {
                animateNode->easing = context->getCurrentToken().lexeme;
                context->advance();
            } else if (key == "loop") {
                animateNode->loop = context->getCurrentToken().lexeme;
                context->advance();
            } else if (key == "direction") {
                animateNode->direction = context->getCurrentToken().lexeme;
                context->advance();
            } else if (key == "delay") {
                animateNode->delay = context->getCurrentToken().lexeme;
                context->advance();
            } else if (key == "callback") {
                animateNode->callback = context->getCurrentToken().lexeme;
                context->advance();
            }
            else if (key == "begin" || key == "end") {
                if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE) {
                     throw std::runtime_error("Expected '{' for begin/end block");
                }
                context->advance(); // consume '{'
                auto& properties = (key == "begin") ? animateNode->begin_properties : animateNode->end_properties;
                while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE) {
                    auto propNode = std::make_shared<PropertyNode>();
                    propNode->name = context->getCurrentToken().lexeme;
                    context->advance();
                    if (context->getCurrentToken().type != TokenType::TOKEN_COLON) {
                         throw std::runtime_error("Expected ':' for property");
                    }
                    context->advance(); // consume ':'
                    propNode->value = context->getCurrentToken().lexeme;
                    context->advance();
                    properties.push_back(propNode);
                    if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
                        context->advance();
                    }
                }
                context->advance(); // consume '}'
            }
            else if (key == "when") {
                if (context->getCurrentToken().type != TokenType::TOKEN_LBRACKET) {
                     throw std::runtime_error("Expected '[' for when block");
                }
                context->advance(); // consume '['
                while (context->getCurrentToken().type != TokenType::TOKEN_RBRACKET) {
                    if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE) {
                         throw std::runtime_error("Expected '{' for keyframe in when block");
                    }
                    context->advance(); // consume '{'
                    std::string at_value;
                    std::vector<std::shared_ptr<PropertyNode>> when_props;

                    while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE) {
                        std::string when_key = context->getCurrentToken().lexeme;
                        context->advance();
                        if (context->getCurrentToken().type != TokenType::TOKEN_COLON) {
                             throw std::runtime_error("Expected ':' for keyframe property");
                        }
                        context->advance(); // consume ':'
                        if (when_key == "at") {
                            at_value = context->getCurrentToken().lexeme;
                            context->advance();
                        } else {
                            auto propNode = std::make_shared<PropertyNode>();
                            propNode->name = when_key;
                            propNode->value = context->getCurrentToken().lexeme;
                            context->advance();
                            when_props.push_back(propNode);
                        }
                        if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
                            context->advance();
                        }
                    }
                    animateNode->when_keyframes.push_back({at_value, when_props});
                    context->advance(); // consume '}'
                    if (context->getCurrentToken().type == TokenType::TOKEN_COMMA) { // Keyframes are separated by commas
                        context->advance();
                    }
                }
                context->advance(); // consume ']'
            } else {
                throw std::runtime_error("Unknown key in Animate block: " + key);
            }

            if (context->getCurrentToken().type == TokenType::TOKEN_COMMA) {
                context->advance();
            }
        }

        context->advance(); // consume '}'
        return animateNode;
    }

}