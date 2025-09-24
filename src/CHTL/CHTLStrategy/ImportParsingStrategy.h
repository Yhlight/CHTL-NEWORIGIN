#pragma once

#include "IParsingStrategy.h"

/**
 * @brief A strategy for parsing a CHTL [Import] statement.
 *
 * This class encapsulates the logic for parsing an import statement
 * and creating an ImportNode for the AST.
 */
class ImportParsingStrategy : public IParsingStrategy {
public:
    std::unique_ptr<BaseNode> parse(Parser& parser) override;
};
