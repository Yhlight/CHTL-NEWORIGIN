#pragma once

#include "../../CHTL/CHTLNode/BaseNode.h"
#include <string>

class HtmlGenerator {
public:
    std::string generate(const BaseNode* root);

private:
    void traverse(const BaseNode* node, std::string& output, int depth = 0);
    std::string generateIndent(int depth);
};