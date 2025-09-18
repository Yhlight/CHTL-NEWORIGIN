#pragma once

#include <string>

namespace CHTL_JS {

enum class CHTLJSTokenType {
    UNKNOWN,
    END_OF_FILE,

    // Syntax
    DOUBLE_LEFT_BRACE,  // {{
    DOUBLE_RIGHT_BRACE, // }}
    ARROW,              // ->
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    COLON,              // :
    COMMA,              // ,

    // Literals
    IDENTIFIER,         // e.g., my_var, click, mouseenter
    FUNCTION_BODY,      // The raw string content of a function, e.g., () => { ... }

    // Keywords
    KEYWORD_LISTEN,

    // Other
    RAW_JS,             // A block of standard JavaScript that isn't otherwise tokenized
};

struct CHTLJSToken {
    CHTLJSTokenType type = CHTLJSTokenType::UNKNOWN;
    std::string value;
};

} // namespace CHTL_JS
