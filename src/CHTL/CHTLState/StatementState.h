#pragma once

#include "ParserState.h"

// The state for parsing a single CHTL statement.
// This state is the main dispatcher that determines the type of statement
// (e.g., element, text block, comment) and contains the logic to parse it.
class StatementState : public ParserState {
public:
    // The handle method orchestrates the parsing of one full statement.
    std::unique_ptr<BaseNode> handle(Parser& parser) override;

private:
    // Helper methods to parse specific types of statements.
    // These contain logic moved from the original Parser class.
    std::unique_ptr<BaseNode> parseElement(Parser& parser);
    std::unique_ptr<BaseNode> parseTextElement(Parser& parser);
    std::unique_ptr<BaseNode> parseComment(Parser& parser);

    // Helpers for parsing content within an element.
    void parseElementBody(Parser& parser, ElementNode& element);
    void parseAttribute(Parser& parser, ElementNode& element);
    void parseStyleBlock(Parser& parser, ElementNode& element);

    // Expression parsing for style values
    std::string parseStyleExpression(Parser& parser);
    std::pair<double, std::string> parseAdditiveExpr(Parser& parser);
    std::pair<double, std::string> parseMultiplicativeExpr(Parser& parser);
    std::pair<double, std::string> parsePrimaryExpr(Parser& parser);
};
