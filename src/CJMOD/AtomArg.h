#pragma once

#include <string>
#include <functional>

// Represents a single atomic argument within a syntax pattern.
class AtomArg {
public:
    // The raw text of the placeholder (e.g., "$", "$?_").
    std::string placeholder;
    // The actual value this argument holds after parsing real code.
    std::string value;

    // Flags derived from the placeholder.
    bool is_optional = false;
    bool is_required = false;
    bool is_unordered = false;
    bool is_variadic = false;

    // A function that can be bound to this argument to process its value.
    std::function<std::string(const std::string&)> bind_function;

    explicit AtomArg(const std::string& placeholder_text);

    void fillValue(const std::string& val);
    void bind(std::function<std::string(const std::string&)> func);
};