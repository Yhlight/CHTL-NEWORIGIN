#include "SimpleValueStrategy.h"
#include "../Parser.h"

namespace CHTL
{
    namespace Strategy
    {
        std::string SimpleValueStrategy::Parse(Parser& parser)
        {
            std::string fullValue;
            while(parser.Peek().type != TokenType::Semicolon && !parser.IsAtEnd())
            {
                if (!fullValue.empty())
                {
                    fullValue += " ";
                }
                fullValue += parser.Consume().value;
            }
            return fullValue;
        }
    }
}
