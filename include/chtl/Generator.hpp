#pragma once
#include "chtl/AST.hpp"
#include <sstream>

namespace chtl {

class Generator {
public:
    std::string generateHTML(const Document& doc);
private:
    void genNode(const Node* node, std::ostringstream& out);
};

} // namespace chtl
