#pragma once

#include <string>

// Forward declaration to avoid circular dependencies with the full Parser header.
class Parser;

class CSSCompiler {
public:
    // Takes a string of CSS that may contain CHTL features and a
    // parser context to resolve references.
    // Returns a string of standard, valid CSS.
    std::string compile(const std::string& css_with_chtl, Parser& context);
};