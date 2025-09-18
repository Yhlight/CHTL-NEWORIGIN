#pragma once

#include <memory>
#include <string>
#include <vector>

namespace CHTL_JS {

// Forward declarations
class EnhancedSelectorNode;
class RawJSNode;
class ListenNode;
class CHTLJS_AstVisitor;

// Base class for all nodes in a CHTL JS AST.
class CHTLJS_BaseNode {
public:
    virtual ~CHTLJS_BaseNode() = default;
    virtual void accept(CHTLJS_AstVisitor& visitor) = 0;
};

// The visitor interface for the CHTL JS AST.
class CHTLJS_AstVisitor {
public:
    virtual ~CHTLJS_AstVisitor() = default;
    virtual void visit(EnhancedSelectorNode& node) = 0;
    virtual void visit(RawJSNode& node) = 0;
    virtual void visit(ListenNode& node) = 0;
};


} // namespace CHTL_JS
