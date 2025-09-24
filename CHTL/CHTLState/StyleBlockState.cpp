#include "StyleBlockState.h"
#include "../Parser.h"
#include "../CHTLNode/CssPropertyNode.h"
#include "../CHTLStrategy/SimpleValueStrategy.h"
#include "../CHTLStrategy/ArithmeticExpressionStrategy.h"
#include <stdexcept>
#include <memory>

namespace CHTL
{
    namespace State
    {
        StyleBlockState::StyleBlockState(AST::StyleNode* ownerNode) : m_ownerNode(ownerNode)
        {
            if (!ownerNode)
            {
                throw std::invalid_argument("StyleBlockState must be initialized with a valid StyleNode.");
            }
        }

        void StyleBlockState::Handle(Parser& parser)
        {
            while (!parser.IsAtEnd())
            {
                if (parser.Match(TokenType::CloseBrace))
                {
                    parser.PopState();
                    return;
                }

                // Expect a property name (identifier)
                if (parser.Peek().type != TokenType::Identifier)
                {
                    throw std::runtime_error("Expected a CSS property name.");
                }
                Token propToken = parser.Consume();

                // Expect a colon or equals sign
                if (!parser.Match(TokenType::Colon) && !parser.Match(TokenType::Equals))
                {
                    throw std::runtime_error("Expected ':' or '=' after CSS property name.");
                }

                // Look ahead to determine if the value is an arithmetic expression
                bool isArithmetic = false;
                size_t lookaheadIndex = 0;
                while(true) {
                    TokenType type = parser.Peek(lookaheadIndex).type;
                    if (type == TokenType::Semicolon || type == TokenType::EndOfFile) {
                        break;
                    }
                    if (type == TokenType::Plus || type == TokenType::Minus ||
                        type == TokenType::Asterisk || type == TokenType::Slash) {
                        isArithmetic = true;
                        break;
                    }
                    lookaheadIndex++;
                }

                // Strategy Selection
                std::unique_ptr<Strategy::IStyleValueParseStrategy> strategy;
                if (isArithmetic) {
                    strategy = std::make_unique<Strategy::ArithmeticExpressionStrategy>();
                } else {
                    strategy = std::make_unique<Strategy::SimpleValueStrategy>();
                }

                std::string value = strategy->Parse(parser);
                m_ownerNode->properties.push_back(std::make_unique<AST::CssPropertyNode>(propToken.value, value));

                // Expect a semicolon
                parser.Expect(TokenType::Semicolon, "Expected ';' after CSS property.");
            }
        }
    }
}
