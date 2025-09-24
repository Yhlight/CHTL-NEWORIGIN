#pragma once

#include "CHTLStrategy/Strategy.h"

namespace CHTL {

/**
 * @class ParseSelectorStrategy
 * @brief A concrete strategy for parsing nested CSS selectors like '.class' or '&:hover'.
 */
class ParseSelectorStrategy : public Strategy {
public:
    void execute(CHTLContext& context) override;
};

} // namespace CHTL
