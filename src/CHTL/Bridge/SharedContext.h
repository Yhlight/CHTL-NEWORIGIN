#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

// Represents a single binding between a responsive variable and a DOM element's property.
struct ResponsiveBinding {
    std::string elementId;
    std::string property; // e.g., "class" or "style.width"
    std::string unit;     // e.g., "px" for style properties
};

// Represents a binding for a property that depends on a dynamic conditional expression.
struct DynamicConditionalBinding {
    std::string targetElementId;
    std::string targetProperty;
    // The expression is now a simple string, to be parsed by the CHTL JS compiler.
    std::string expression;
};

// Represents a binding for a block of content whose rendering is controlled by a dynamic condition.
struct DynamicRenderingBinding {
    std::string elementId; // The ID of the wrapper div for the conditional content.
    // The expression is now a simple string.
    std::string expression;
};

// The "Salt Bridge": A shared context to pass data between compilers.
class SharedContext {
public:
    // Bindings for responsive values ($var$).
    std::map<std::string, std::vector<ResponsiveBinding>> responsiveBindings;

    // Bindings for dynamic conditional expressions (e.g., prop: {{...}}->... ? ... : ...).
    std::vector<DynamicConditionalBinding> dynamicConditionalBindings;

    // Bindings for dynamic conditional rendering (if blocks).
    std::vector<DynamicRenderingBinding> dynamicRenderingBindings;
};