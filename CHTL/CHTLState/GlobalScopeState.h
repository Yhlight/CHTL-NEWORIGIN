#pragma once

#include "State.h"

namespace CHTL {

class Parser; // Forward declaration

// This state handles parsing of top-level constructs in a CHTL file,
// such as element definitions, templates, imports, and namespaces.
class GlobalScopeState : public State {
public:
    explicit GlobalScopeState(Parser* parser);

    // This method will contain the primary logic for parsing the global scope.
    void onUpdate(StateMachine* sm) override;

private:
    Parser* parser;
};

} // namespace CHTL
