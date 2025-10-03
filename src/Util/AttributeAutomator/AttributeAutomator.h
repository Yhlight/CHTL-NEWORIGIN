#pragma once

#include "../../CHTL/CHTLNode/BaseNode.h"

class AttributeAutomator {
public:
    void process(BaseNode* root);

private:
    void traverse(BaseNode* node);
};