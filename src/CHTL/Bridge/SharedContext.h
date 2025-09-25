#pragma once

#include <string>
#include <vector>
#include <map>

// Represents a single binding between a responsive variable and a DOM element's property.
struct ResponsiveBinding {
    std::string elementId;
    std::string property; // e.g., "class" or "style.width"
    std::string unit;     // e.g., "px" for style properties
};

// The "Salt Bridge": A shared context to pass data from the CHTL compiler
// to the (future) CHTL JS compiler.
class SharedContext {
public:
    // A map where the key is the responsive variable name (e.g., "myWidth")
    // and the value is a list of all the DOM elements bound to it.
    std::map<std::string, std::vector<ResponsiveBinding>> responsiveBindings;
};