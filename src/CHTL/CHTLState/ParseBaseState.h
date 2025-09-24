#pragma once

#include "CHTLState/State.h"

namespace CHTL {

/**
 * @class ParseBaseState
 * @brief Handles the top-level (base) of a CHTL document.
 *
 * This state is responsible for identifying top-level constructs,
 * primarily element definitions.
 */
class ParseBaseState : public State {
public:
    void handle(CHTLContext& context) override;
};

} // namespace CHTL
