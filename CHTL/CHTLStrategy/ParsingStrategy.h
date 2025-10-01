#ifndef CHTL_PARSING_STRATEGY_H
#define CHTL_PARSING_STRATEGY_H

#include <memory>

namespace CHTL {

class CHTLParser; // Forward declaration
class BaseNode;

class ParsingStrategy {
public:
    virtual ~ParsingStrategy() = default;
    virtual std::unique_ptr<BaseNode> parse(CHTLParser& parser) = 0;
};

} // namespace CHTL

#endif // CHTL_PARSING_STRATEGY_H