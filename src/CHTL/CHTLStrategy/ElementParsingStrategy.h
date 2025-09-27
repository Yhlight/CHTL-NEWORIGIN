#pragma once

#include "ParsingStrategy.h"

class ElementNode; // Forward declaration

class ElementParsingStrategy : public ParsingStrategy {
public:
    std::unique_ptr<BaseNode> parse(Parser& parser) override;

    // Helpers for parsing content within an element.
    void parseElementBody(Parser& parser, ElementNode& element);
    void parseAttribute(Parser& parser, ElementNode& element);
    void parseExceptClause(Parser& parser, ElementNode& element);
};