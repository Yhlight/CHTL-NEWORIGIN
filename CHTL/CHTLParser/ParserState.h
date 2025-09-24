#pragma once

#include <string>

namespace CHTL {

// CHTL Parser States
enum class ParserState {
    INIT,           // Initial state
    PARSE_USE,      // Parsing use statement
    PARSE_HTML5,    // Parsing html5 declaration
    PARSE_ELEMENTS, // Parsing element definitions
    PARSE_ATTRIBUTES, // Parsing element attributes
    PARSE_STYLE,    // Parsing style blocks
    PARSE_SCRIPT,   // Parsing script blocks
    PARSE_TEMPLATE, // Parsing template definitions
    PARSE_CUSTOM,   // Parsing custom components
    PARSE_IMPORT,   // Parsing import statements
    PARSE_NAMESPACE, // Parsing namespace declarations
    PARSE_ORIGIN,   // Parsing origin blocks
    COMPLETE,       // Parsing completed successfully
    ERROR           // Error state
};

// Parser context data keys
namespace ParserContext {
    constexpr const char* CURRENT_ELEMENT = "current_element";
    constexpr const char* CURRENT_ATTRIBUTE = "current_attribute";
    constexpr const char* CURRENT_TEMPLATE = "current_template";
    constexpr const char* CURRENT_CUSTOM = "current_custom";
    constexpr const char* CURRENT_NAMESPACE = "current_namespace";
    constexpr const char* PARSED_ELEMENTS = "parsed_elements";
    constexpr const char* PARSED_TEMPLATES = "parsed_templates";
    constexpr const char* PARSED_CUSTOMS = "parsed_customs";
    constexpr const char* ERROR_MESSAGE = "error_message";
    constexpr const char* ERROR_LINE = "error_line";
    constexpr const char* ERROR_COLUMN = "error_column";
}

// Parser error types
enum class ParserErrorType {
    SYNTAX_ERROR,       // Invalid syntax
    MISSING_TOKEN,      // Expected token not found
    UNEXPECTED_TOKEN,   // Unexpected token found
    SEMANTIC_ERROR,     // Semantic error
    UNDEFINED_REFERENCE, // Reference to undefined element/template
    DUPLICATE_DEFINITION, // Duplicate definition
    CIRCULAR_DEPENDENCY  // Circular dependency
};

// Parser error information
struct ParserError {
    ParserErrorType type;
    std::string message;
    size_t line;
    size_t column;
    std::string context;
    
    ParserError(ParserErrorType t, const std::string& msg, size_t l = 0, size_t c = 0, const std::string& ctx = "")
        : type(t), message(msg), line(l), column(c), context(ctx) {}
};

} // namespace CHTL