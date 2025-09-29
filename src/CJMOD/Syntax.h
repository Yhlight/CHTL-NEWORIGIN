#pragma once

#include "Arg.h"
#include <string>
#include <vector>
#include <memory>

// The Syntax class provides static methods to analyze CHTL JS syntax patterns
// and to perform utility checks on JS/CHTL JS code snippets.
class Syntax {
public:
    // Analyzes a pattern string (e.g., "$ ** $") and returns an Arg object
    // representing the structure of that pattern.
    static std::unique_ptr<Arg> analyze(const std::string& pattern);

    // Utility functions to identify code structures.
    // These are placeholders for a more advanced implementation.
    static bool isObject(const std::string& code);
    static bool isFunction(const std::string& code);
    static bool isArray(const std::string& code);
    static bool isCHTLJSFunction(const std::string& code);
};