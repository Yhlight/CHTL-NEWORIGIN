#pragma once

#include "../CHTLNode/BaseNode.h"
#include <memory>

// A utility class with a static method to perform a deep copy of an AST node
// and its children. This is essential for instantiating element templates,
// as the same template can be used multiple times in a document.
class NodeCloner {
public:
    static std::unique_ptr<BaseNode> clone(const BaseNode* node);
};
