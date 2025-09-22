#pragma once

#include <memory>
#include <vector>
#include <string>

namespace CHTL {

// Forward declare visitor for the accept method
class AstVisitor;

class BaseNode {
public:
    virtual ~BaseNode() = default;
    // Using a visitor pattern for generation and other tree walks
    virtual void accept(AstVisitor& visitor) = 0;
};

} // namespace CHTL
