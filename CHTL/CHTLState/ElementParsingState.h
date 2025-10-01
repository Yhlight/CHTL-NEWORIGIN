#ifndef CHTL_ELEMENT_PARSING_STATE_H
#define CHTL_ELEMENT_PARSING_STATE_H

#include "ParsingState.h"

namespace CHTL {

class ElementParsingState : public ParsingState {
public:
    void handle(CHTLParser& parser) override;
};

} // namespace CHTL

#endif // CHTL_ELEMENT_PARSING_STATE_H