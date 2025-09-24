#include "ElementState.h"
#include "StyleBlockState.h"
#include "../Parser.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
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

                Token identifier = parser.Peek();

                if (identifier.value == "style")
                {
                    parser.Consume(); // consume 'style'
                    parser.Expect(TokenType::OpenBrace, "Expected '{' after 'style' keyword.");

                    auto styleNode = std::make_unique<AST::StyleNode>();
                    m_ownerNode->children.push_back(std::move(styleNode));
                    AST::StyleNode* styleNodePtr = dynamic_cast<AST::StyleNode*>(m_ownerNode->children.back().get());

                    parser.PushState(std::make_unique<State::StyleBlockState>(styleNodePtr));
                    return;
                }
                else if (identifier.value == "text")
                {
                    parser.Consume(); // consume 'text'
                    if (parser.Match(TokenType::Colon)) // text: "value";
                    {
                        Token valueToken = parser.Consume();
                        if (valueToken.type != TokenType::StringLiteral && valueToken.type != TokenType::Identifier)
                            throw std::runtime_error("Expected string or unquoted literal for text value.");
                        std::unique_ptr<AST::BaseNode> textNode = std::make_unique<AST::TextNode>(valueToken.value);
                        m_ownerNode->children.push_back(std::move(textNode));
                        parser.Expect(TokenType::Semicolon, "Expected ';' after text attribute.");
                    }
                    else if (parser.Match(TokenType::OpenBrace)) // text { ... }
                    {
                        std::string textContent;
                        while(parser.Peek().type != TokenType::CloseBrace && !parser.IsAtEnd())
                        {
                            textContent += parser.Consume().value + " ";
                        }
                        // Trim trailing space
                        if (!textContent.empty()) {
                            textContent.pop_back();
                        }

                        std::unique_ptr<AST::BaseNode> textNode = std::make_unique<AST::TextNode>(textContent);
                        m_ownerNode->children.push_back(std::move(textNode));
                        parser.Expect(TokenType::CloseBrace, "Expected '}' to close text block.");
                    }
                    else
                    {
                        throw std::runtime_error("Invalid syntax for 'text'. Expected ':' or '{'.");
                    }
                }
                else if (parser.Peek(1).type == TokenType::Colon || parser.Peek(1).type == TokenType::Equals)
                {
                    ParseAttribute(parser);
                }
                else if (parser.Peek(1).type == TokenType::OpenBrace)
                {
                    Token idToken = identifier;
                    parser.Consume(); // Consume identifier
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
            if (!parser.Match(TokenType::Colon) && !parser.Match(TokenType::Equals))
            {
                throw std::runtime_error("Expected ':' or '=' after attribute name.");
            }

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
