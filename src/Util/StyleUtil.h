#pragma once

#include "CHTL/CHTLNode/StyleValue.h"
#include <string>

// Converts a StyleValue object to its final string representation for CSS.
std::string styleValueToString(const StyleValue& sv);
