#include "GlobalState.h"
#include "../Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "ElementState.h"
#include <stdexcept>

namespace CHTL
{
    namespace State
    {
        void GlobalState::Handle(Parser& parser)
        {
            if (parser.Peek().type == TokenType::Identifier)
            {
                Token idToken = parser.Consume();
                auto elementNode = std::make_unique<AST::ElementNode>(idToken.value);
                parser.Expect(TokenType::OpenBrace, "Expected '{' after element name.");

                // Get a raw pointer before moving ownership
                AST::ElementNode* elementPtr = elementNode.get();

                // Add the completed node to the parser's list of root nodes
                parser.AddRootNode(std::move(elementNode));

                // Push a new state to parse the element's body
                parser.PushState(std::make_unique<ElementState>(elementPtr));
                return;
            }

            // If we are at the end, this state is done.
            if (parser.IsAtEnd())
            {
                parser.PopState();
                return;
            }

            throw std::runtime_error("Expected a top-level element identifier.");
        }
    }
}
