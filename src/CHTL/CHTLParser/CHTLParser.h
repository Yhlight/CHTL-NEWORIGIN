#pragma once

#include "CHTLNode/BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

/**
 * @class CHTLParser
 * @brief The main parser class that drives the compilation process.
 *
 * It takes source code, runs the state machine, and produces an AST.
 */
class CHTLParser {
public:
    CHTLParser() = default;

    std::unique_ptr<BaseNode> parse(const std::string& source);
};

} // namespace CHTL
