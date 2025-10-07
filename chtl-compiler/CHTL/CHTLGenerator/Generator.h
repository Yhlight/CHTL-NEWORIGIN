#pragma once

#include "chtl-compiler/CHTL/CHTLNode/Node.h"
#include <string>
#include <sstream>

class Generator {
public:
    std::string generate(const NodeList& nodes);
    std::string generate(const NodePtr& node);

private:
    void visit(const NodePtr& node, std::stringstream& output);
};