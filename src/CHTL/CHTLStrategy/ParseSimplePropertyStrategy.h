#pragma once

#include "CHTLStrategy/Strategy.h"

namespace CHTL {

/**
 * @class ParseSimplePropertyStrategy
 * @brief A concrete strategy for parsing simple 'key: value;' CSS properties.
 */
class ParseSimplePropertyStrategy : public Strategy {
public:
    void execute(CHTLContext& context) override;
};

} // namespace CHTL
