#pragma once

#include "CHTLState/State.h"

namespace CHTL {

/**
 * @class ParseElementState
 * @brief Handles the content within an element's braces (`{...}`).
 *
 * This state is responsible for identifying attributes, nested elements,
 * and other constructs that can appear inside an element block.
 */
class ParseElementState : public State {
public:
    void handle(CHTLContext& context) override;
};

} // namespace CHTL
