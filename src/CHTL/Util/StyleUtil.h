#pragma once

#include "../CHTLNode/StyleValue.h"
#include <string>

// Converts a StyleValue object to its final string representation for CSS.
std::string styleValueToString(const StyleValue& sv);

// Attempts to extract the first valid numeric value from a multi-word string.
// Returns a NUMERIC StyleValue on success, or an EMPTY StyleValue on failure.
StyleValue decoupleNumericValue(const std::string& s);
