#pragma once

#include <string>
#include <vector>

// Defines the different types of targets for a constraint rule.
enum class ConstraintTargetType {
    // A constraint on a specific tag name, e.g., `except span;`
    TAG_NAME,

    // A constraint on a specific template or custom element, e.g., `except [Custom] @Element Box;`
    QUALIFIED_NAME,

    // A constraint on a whole category of items.
    HTML_CATEGORY,         // e.g., `except @Html;`
    TEMPLATE_CATEGORY,     // e.g., `except [Template];`
    CUSTOM_CATEGORY,       // e.g., `except [Custom];`
    TEMPLATE_VAR_CATEGORY, // e.g., `except [Template] @Var;`
};

// Represents a single constraint rule defined by an `except` clause.
struct Constraint {
    ConstraintTargetType type;

    // For TAG_NAME and QUALIFIED_NAME, this holds the identifier (e.g., "span", "Box").
    std::string name;

    // For QUALIFIED_NAME, this holds the meta qualifier (e.g., "[Custom]", "[Template]").
    std::string meta_qualifier;

    // For QUALIFIED_NAME, this holds the type qualifier (e.g., "@Element", "@Style").
    // For TEMPLATE_VAR_CATEGORY, this holds "@Var".
    std::string type_qualifier;
};