#pragma once

#include "../../CHTL/CHTLNode/BaseNode.h"
#include <string>

class HtmlGenerator {
public:
    std::string generate(BaseNode* root);

private:
    void traverse(BaseNode* node, std::string& html, int indent = 0);
};