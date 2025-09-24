#pragma once

#include "IParsingStrategy.h"
#include "../CHTLNode/BaseNode.h"
#include <memory>

// Forward declarations
class Parser;
class ElementNode;

/**
 * @brief A strategy for parsing a CHTL element.
 *
 * This class encapsulates the logic for parsing a standard CHTL element,
 * including its tag name, attributes, and body content.
 */
class ElementParsingStrategy : public IParsingStrategy {
public:
    std::unique_ptr<BaseNode> parse(Parser& parser) override;

private:
    // Helper methods to parse specific parts of an element.
    // These are moved from StatementState.
    void parseElementBody(Parser& parser, ElementNode& element);
    void parseAttribute(Parser& parser, ElementNode& element);
    void parseExceptClause(Parser& parser, ElementNode& element);
};
