#include "Placeholder.h"
#include <stdexcept>

// Generates a new placeholder and stores its content.
std::string Placeholder::generate(PlaceholderType type, const std::string& content) {
    // Create a unique placeholder string.
    std::string placeholder = format_placeholder(type, placeholder_id_counter++);

    // Store the content associated with the placeholder.
    placeholder_map[placeholder] = content;

    return placeholder;
}

// Retrieves the content for a given placeholder.
const std::string& Placeholder::getContent(const std::string& placeholder) const {
    // Find the placeholder in the map.
    auto it = placeholder_map.find(placeholder);
    if (it != placeholder_map.end()) {
        // Return the found content.
        return it->second;
    }
    // If the placeholder is not found, throw an error.
    // This indicates a logic error in the compiler.
    throw std::runtime_error("Placeholder not found: " + placeholder);
}

// Formats the placeholder string based on its type and ID.
std::string Placeholder::format_placeholder(PlaceholderType type, int id) {
    std::string type_str;
    switch (type) {
        case PlaceholderType::JS:
            type_str = "_JS_CODE_PLACEHOLDER_";
            break;
        case PlaceholderType::CSS:
            type_str = "_CSS_CODE_PLACEHOLDER_";
            break;
        case PlaceholderType::CHTL_JS_BLOCK:
            type_str = "_CHTL_JS_BLOCK_PLACEHOLDER_";
            break;
    }
    return type_str + std::to_string(id) + "_";
}