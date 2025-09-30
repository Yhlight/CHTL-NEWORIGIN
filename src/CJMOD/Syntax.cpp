#include "Syntax.h"
#include <sstream> // Required for std::stringstream

// Analyzes a pattern string (e.g., "$ ** $") and returns an Arg object
// representing the structure of that pattern.
std::unique_ptr<Arg> Syntax::analyze(const std::string& pattern) {
    auto arg_obj = std::make_unique<Arg>();
    std::stringstream ss(pattern);
    std::string placeholder;

    while (ss >> placeholder) {
        arg_obj->arguments.push_back(std::make_unique<AtomArg>(placeholder));
    }

    return arg_obj;
}

// Utility functions to identify code structures.
// These are placeholders for a more advanced implementation.
bool Syntax::isObject(const std::string& code) {
    return !code.empty() && code.front() == '{' && code.back() == '}';
}

bool Syntax::isFunction(const std::string& code) {
    return code.rfind("function", 0) == 0;
}

bool Syntax::isArray(const std::string& code) {
    return !code.empty() && code.front() == '[' && code.back() == ']';
}

bool Syntax::isCHTLJSFunction(const std::string& code) {
    // A real implementation would be more sophisticated.
    return code.find('{') != std::string::npos && code.find('}') != std::string::npos;
}