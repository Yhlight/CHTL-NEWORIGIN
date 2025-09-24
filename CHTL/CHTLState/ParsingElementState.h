#pragma once

#include "State.h"
#include "CHTLNode/ElementNode.h"
#include <memory>

namespace CHTL {

class Parser; // Forward declaration

// This state handles parsing the content within an element's curly braces {},
// such as nested elements, attributes, and style blocks.
class ParsingElementState : public State {
public:
    explicit ParsingElementState(Parser* parser, std::shared_ptr<ElementNode> currentNode);

    // The primary logic for parsing inside an element.
    void onUpdate(StateMachine* sm) override;

private:
    Parser* parser;
    std::shared_ptr<ElementNode> currentNode; // The element node we are currently populating.
};

} // namespace CHTL
