#pragma once

#include <string>
#include <vector>
#include <map>

// Using an enum to define the different types of placeholders.
// This makes the code more readable and easier to maintain.
enum class PlaceholderType {
    JS,
    CSS,
    CHTL_JS_BLOCK, // For entire blocks of CHTL JS
    // Add other types as needed
};

// The Placeholder class manages the creation and tracking of placeholders.
class Placeholder {
public:
    // Generates a new, unique placeholder string for a given type.
    // The placeholder will be stored internally.
    std::string generate(PlaceholderType type, const std::string& content);

    // Retrieves the original content associated with a placeholder.
    const std::string& getContent(const std::string& placeholder) const;

    // A map to store the placeholder and its original content.
    // The key is the placeholder string, and the value is the content.
    std::map<std::string, std::string> placeholder_map;

private:
    // A counter to ensure that each placeholder is unique.
    int placeholder_id_counter = 0;

    // Helper function to create the placeholder string format.
    std::string format_placeholder(PlaceholderType type, int id);
};