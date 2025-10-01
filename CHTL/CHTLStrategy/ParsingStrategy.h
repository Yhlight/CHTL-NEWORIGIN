#ifndef CHTL_PARSING_STRATEGY_H
#define CHTL_PARSING_STRATEGY_H

namespace CHTL {

class CHTLParser; // Forward-declaration

class ParsingStrategy {
public:
    virtual ~ParsingStrategy() = default;
    virtual void execute(CHTLParser& parser) = 0;
};

} // namespace CHTL

#endif // CHTL_PARSING_STRATEGY_H