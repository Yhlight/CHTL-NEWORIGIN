#include "ElementState.h"
#include "../Parser.h"
#include "../CHTLNode/AttributeNode.h"
#include <stdexcept>

namespace CHTL
{
    namespace State
    {
        ElementState::ElementState(AST::ElementNode* ownerNode) : m_ownerNode(ownerNode)
        {
            if (!ownerNode)
            {
                throw std::invalid_argument("ElementState must be initialized with a valid ElementNode.");
            }
        }

        void ElementState::Handle(Parser& parser)
        {
            while (!parser.IsAtEnd())
            {
                // If we see the closing brace, the element is done.
                if (parser.Match(TokenType::CloseBrace))
                {
                    parser.PopState(); // Pop this ElementState.
                    return;
                }

                if (parser.Peek().type != TokenType::Identifier)
                {
                    throw std::runtime_error("Expected identifier for attribute or nested element.");
                }

                if (parser.Peek(1).type == TokenType::Colon)
                {
                    ParseAttribute(parser);
                }
                else if (parser.Peek(1).type == TokenType::OpenBrace)
                {
                    Token idToken = parser.Consume();
                    auto nestedElement = std::make_unique<AST::ElementNode>(idToken.value);
                    parser.Expect(TokenType::OpenBrace, "Expected '{' for nested element.");

                    AST::ElementNode* nestedElementPtr = nestedElement.get();
                    m_ownerNode->children.push_back(std::move(nestedElement));

                    // Push a new state to handle the nested element's body.
                    // This replaces the current state in the next iteration of the main loop.
                    parser.PushState(std::make_unique<ElementState>(nestedElementPtr));
                    return;
                }
                else
                {
                    throw std::runtime_error("Unexpected identifier in element body.");
                }
            }
        }

        void ElementState::ParseAttribute(Parser& parser)
        {
            Token nameToken = parser.Consume();
            parser.Expect(TokenType::Colon, "Expected ':' after attribute name.");

            Token valueToken = parser.Peek();
            if (valueToken.type == TokenType::StringLiteral ||
                valueToken.type == TokenType::NumberLiteral ||
                valueToken.type == TokenType::Identifier)
            {
                parser.Consume();
                m_ownerNode->attributes.push_back(std::make_unique<AST::AttributeNode>(nameToken.value, valueToken.value));
            }
            else
            {
                throw std::runtime_error("Expected attribute value.");
            }

            parser.Expect(TokenType::Semicolon, "Expected ';' after attribute.");
        }
    }
}
