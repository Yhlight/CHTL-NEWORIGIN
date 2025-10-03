#pragma once

#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../GlobalStylesheet/GlobalStylesheet.h"

class CssGenerator {
public:
    void generate(BaseNode* root, GlobalStylesheet& stylesheet);

private:
    void traverse(BaseNode* node, ElementNode* parentElement, GlobalStylesheet& stylesheet);
};