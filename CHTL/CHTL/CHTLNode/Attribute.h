#ifndef CHTL_ATTRIBUTE_H
#define CHTL_ATTRIBUTE_H

#include <string>
#include "../CHTLLexer/Token.h"

namespace CHTL {

struct Attribute {
    Token token; // The token for the attribute key (e.g., 'id')
    std::string key;
    std::string value;
};

} // namespace CHTL

#endif // CHTL_ATTRIBUTE_H
