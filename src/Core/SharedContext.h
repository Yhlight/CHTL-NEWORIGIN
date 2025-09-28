#pragma once

#include <string>
#include <map>

// This class simulates the "Salt Bridge" between the CHTL and CHTL JS compilers.
// In a real implementation, this would be a more complex object populated by both.
class SharedContext {
public:
    // Maps a responsive variable name (e.g., "myVar") to the unique ID
    // of the HTML element it's bound to.
    std::map<std::string, std::string> responsive_variable_map;
};