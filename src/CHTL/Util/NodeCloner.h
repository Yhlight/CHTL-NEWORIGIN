#pragma once

#include "../CHTLNode/BaseNode.h"
#include <memory>
#include <stdexcept>

// A utility class with a static method to perform a deep copy of an AST node
// and its children. This is essential for instantiating element templates,
// as the same template can be used multiple times in a document.
class NodeCloner {
public:
    static std::unique_ptr<BaseNode> clone(const BaseNode* node);

    template <typename T>
    static std::unique_ptr<T> clone_unique(const T* node) {
        auto base_clone = NodeCloner::clone(node);
        T* derived_ptr = dynamic_cast<T*>(base_clone.release());
        if (!derived_ptr) {
            // This should not happen if the clone logic is correct and the input node is of the correct type
            throw std::runtime_error("Failed to cast cloned node during clone_unique.");
        }
        return std::unique_ptr<T>(derived_ptr);
    }
};