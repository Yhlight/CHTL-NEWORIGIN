#pragma once

#include <string>

namespace chtl::core {

struct AstNode;

class Generator {
public:
    std::string generateHtml(const AstNode& root) const;
};

}
