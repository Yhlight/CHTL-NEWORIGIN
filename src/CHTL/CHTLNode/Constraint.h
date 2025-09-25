#pragma once

#include <string>
#include <vector>

enum class ConstraintType {
    TagName,         // e.g., span
    CustomElement,   // e.g., [Custom] @Element Box
    TemplateElement, // e.g., [Template] @Element Box
    TemplateStyle,   // e.g., [Template] @Style Theme
    TemplateVar,     // e.g., [Template] @Var Colors
    Html,            // e.g., @Html (all html tags)
    Custom,          // e.g., [Custom] (all custom types)
    Template         // e.g., [Template] (all template types)
};

struct Constraint {
    ConstraintType type;
    // The name of the item being constrained, e.g., "span", "Box", "Theme".
    // This may be empty for broad type constraints like @Html.
    std::string identifier;
};