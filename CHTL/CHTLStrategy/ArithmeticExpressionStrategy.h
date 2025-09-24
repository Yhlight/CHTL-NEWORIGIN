#ifndef CHTL_ARITHMETIC_EXPRESSION_STRATEGY_H
#define CHTL_ARITHMETIC_EXPRESSION_STRATEGY_H

#include "IStyleValueParseStrategy.h"

namespace CHTL
{
    namespace Strategy
    {
        class ArithmeticExpressionStrategy : public IStyleValueParseStrategy
        {
        public:
            std::string Parse(Parser& parser) override;

        private:
            // Helper methods for a simple recursive descent parser
            double ParseExpression(Parser& parser);
            double ParseTerm(Parser& parser);
            double ParseFactor(Parser& parser);
        };
    }
}

#endif // CHTL_ARITHMETIC_EXPRESSION_STRATEGY_H
