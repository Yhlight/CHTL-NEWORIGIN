#ifndef CHTL_ISTYLE_VALUE_PARSE_STRATEGY_H
#define CHTL_ISTYLE_VALUE_PARSE_STRATEGY_H

#include <string>

// Forward declare Parser to avoid circular dependencies
namespace CHTL { class Parser; }

namespace CHTL
{
    namespace Strategy
    {
        class IStyleValueParseStrategy
        {
        public:
            virtual ~IStyleValueParseStrategy() = default;
            virtual std::string Parse(Parser& parser) = 0;

            // Static method to check if the strategy can handle the upcoming tokens
            // This is not a great design, a better one might be a factory. But for now, this works.
            // virtual static bool CanParse(Parser& parser) = 0; // static virtual is not allowed
        };
    }
}

#endif // CHTL_ISTYLE_VALUE_PARSE_STRATEGY_H
