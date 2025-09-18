#pragma once

#include <string>

namespace CHTL_JS {

enum class CHTLJSTokenType {
    UNKNOWN,
    END_OF_FILE,

    DOUBLE_LEFT_BRACE,  // {{
    DOUBLE_RIGHT_BRACE, // }}
    SELECTOR_CONTENT,   // .my-class inside {{...}}

    ARROW,              // ->

    RAW_JS,             // A block of standard JavaScript
};

struct CHTLJSToken {
    CHTLJSTokenType type = CHTLJSTokenType::UNKNOWN;
    std::string value;
};

} // namespace CHTL_JS
