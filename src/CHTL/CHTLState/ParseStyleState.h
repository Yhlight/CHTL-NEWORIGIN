#pragma once

#include "CHTLState/State.h"

namespace CHTL {

/**
 * @class ParseStyleState
 * @brief Handles the content within a style { ... } block.
 *
 * This state will use the Strategy pattern to parse different kinds of
 * style rules (e.g., simple properties, nested selectors).
 */
class ParseStyleState : public State {
public:
    void handle(CHTLContext& context) override;
};

} // namespace CHTL
