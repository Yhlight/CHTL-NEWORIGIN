#ifndef CHTL_BODY_PARSING_STATE_H
#define CHTL_BODY_PARSING_STATE_H

#include "ParsingState.h"

namespace CHTL {

class BodyParsingState : public ParsingState {
public:
    void handle(CHTLParser& parser) override;
};

} // namespace CHTL

#endif // CHTL_BODY_PARSING_STATE_H