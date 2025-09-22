#ifndef CHTL_STYLE_PROPERTY_H
#define CHTL_STYLE_PROPERTY_H

#include <string>
#include "../CHTLLexer/Token.h"

namespace CHTL {

struct StyleProperty {
    Token token; // The token for the property key (e.g., 'width')
    std::string key;
    std::string value;
};

} // namespace CHTL

#endif // CHTL_STYLE_PROPERTY_H
