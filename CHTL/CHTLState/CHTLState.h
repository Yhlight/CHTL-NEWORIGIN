#pragma once

#include <stack>

enum class ParseState {
    // Initial state, expecting a top-level element or directive
    GLOBAL_SCOPE,

    // Inside an element block `... { ... }`, expecting attributes or nested elements
    IN_ELEMENT_BODY,

    // After an attribute name and colon, e.g., `id:`, expecting a value
    EXPECTING_ATTRIBUTE_VALUE,

    // Inside a text block, `text { ... }`
    IN_TEXT_BLOCK,

    // Inside a style block, `style { ... }`
    IN_STYLE_BLOCK
};

class CHTLState {
public:
    CHTLState();

    void enter(ParseState state);
    void leave();
    ParseState getCurrentState() const;
    bool isInState(ParseState state) const;
    size_t getDepth() const;

private:
    std::stack<ParseState> stateStack;
};
