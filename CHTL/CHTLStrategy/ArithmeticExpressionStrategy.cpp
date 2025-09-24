#include "ArithmeticExpressionStrategy.h"
#include "../Parser.h"
#include <stdexcept>
#include <string>

namespace CHTL
{
    namespace Strategy
    {
        // Main entry point for the strategy
        std::string ArithmeticExpressionStrategy::Parse(Parser& parser)
        {
            double result = ParseExpression(parser);
            // For now, assume the result is always in pixels.
            return std::to_string(result) + "px";
        }

        // Handles + and -
        double ArithmeticExpressionStrategy::ParseExpression(Parser& parser)
        {
            double left = ParseTerm(parser);

            while (parser.Peek().type == TokenType::Plus || parser.Peek().type == TokenType::Minus)
            {
                Token op = parser.Consume();
                double right = ParseTerm(parser);
                if (op.type == TokenType::Plus)
                {
                    left += right;
                }
                else
                {
                    left -= right;
                }
            }
            return left;
        }

        // Handles * and /
        double ArithmeticExpressionStrategy::ParseTerm(Parser& parser)
        {
            double left = ParseFactor(parser);

            while (parser.Peek().type == TokenType::Asterisk || parser.Peek().type == TokenType::Slash)
            {
                Token op = parser.Consume();
                double right = ParseFactor(parser);
                if (op.type == TokenType::Asterisk)
                {
                    left *= right;
                }
                else
                {
                    if (right == 0) throw std::runtime_error("Division by zero in style expression.");
                    left /= right;
                }
            }
            return left;
        }

        // Handles numbers and parentheses
        double ArithmeticExpressionStrategy::ParseFactor(Parser& parser)
        {
            if (parser.Peek().type == TokenType::NumberLiteral)
            {
                // Note: This simple parser ignores units like 'px' on the input numbers.
                // A more robust implementation would parse and handle them.
                Token numToken = parser.Consume();
                // Also consume the unit if it exists
                if (parser.Peek().type == TokenType::Identifier) {
                    parser.Consume(); // consume 'px', 'em', etc.
                }
                return std::stod(numToken.value);
            }

            // Handle parentheses for grouping
            if (parser.Match(TokenType::OpenParen))
            {
                double result = ParseExpression(parser);
                parser.Expect(TokenType::CloseParen, "Expected ')' after expression in parentheses.");
                return result;
            }

            throw std::runtime_error("Unexpected token in style expression.");
        }
    }
}
