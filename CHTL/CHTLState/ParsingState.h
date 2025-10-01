#ifndef CHTL_PARSING_STATE_H
#define CHTL_PARSING_STATE_H

#include "../CHTLParser/CHTLParser.h"

namespace CHTL {

class CHTLParser; // Forward declaration

class ParsingState {
public:
    virtual ~ParsingState() = default;
    virtual void handle(CHTLParser& parser) = 0;
};

} // namespace CHTL

#endif // CHTL_PARSING_STATE_H