#pragma once

#include "../CHTLNode/StyleValue.h"
#include <string>

// Forward declarations
class Parser;

// Converts a StyleValue object to its final string representation for CSS.
std::string styleValueToString(const StyleValue& sv);

// The main entry point for parsing a style value expression.
// It requires the parser context to access the token stream and other state.
StyleValue parseStyleExpression(Parser& parser);
