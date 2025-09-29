#include "AtomArg.h"
#include <algorithm>

AtomArg::AtomArg(const std::string& placeholder_text) : placeholder(placeholder_text) {
    // Basic parsing of placeholder flags
    is_optional = placeholder.find('?') != std::string::npos;
    is_required = placeholder.find('!') != std::string::npos;
    is_unordered = placeholder.find('_') != std::string::npos;
    is_variadic = placeholder.find("...") != std::string::npos;
}

void AtomArg::fillValue(const std::string& val) {
    this->value = val;
}

void AtomArg::bind(std::function<std::string(const std::string&)> func) {
    this->bind_function = func;
}