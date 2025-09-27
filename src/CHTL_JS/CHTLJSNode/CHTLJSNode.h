#pragma once

#include <memory>
#include <string>
#include <vector>

// Defines the different types of nodes in the CHTL JS Abstract Syntax Tree.
enum class CHTLJSNodeType {
    // Represents an enhanced selector, e.g., {{#my-id}}
    EnhancedSelector,
    // Represents a CHTL JS function call, e.g., ->Listen { ... }
    FunctionCall,
    // Represents a Listen block, e.g., ->Listen { ... }
    Listen,
    // Represents an event binding expression, e.g., &-> click: ...
    EventBinding,
    // Represents a Delegate block for event delegation.
    Delegate,
    // Represents an Animate block for creating animations.
    Animate,
    // Represents a ScriptLoader block for loading external scripts.
    ScriptLoader,
    // Represents a virtual object declaration, e.g., Vir myVar = ...
    VirtualObject,
    // Represents a property access on a virtual object, e.g., myVir->property
    VirtualObjectAccess,
    // Represents a Router block for defining client-side routes.
    Router,
    // Represents raw, plain JavaScript code that is not processed by the CHTL JS compiler.
    RawJavaScript,
    // More types will be added as the compiler is developed.
};

// Base class for all nodes in the CHTL JS AST.
class CHTLJSNode {
public:
    virtual ~CHTLJSNode() = default;
    virtual CHTLJSNodeType getType() const = 0;
};