#include "DefaultState.h"
#include "TagState.h"
#include "ScriptState.h"
#include "StyleState.h"
#include "CommentState.h"
#include "TextState.h"
#include "../CHTLParser/CHTLParser.h"

#include <iostream>

void DefaultState::handle(CHTLParser& parser, Token token) {
    // Placeholder implementation
    std::cout << "DefaultState handling token: " << token.value << std::endl;

    // Example of state transition based on token type
    if (token.type == TokenType::IDENTIFIER) {
        // Potentially a tag name, transition to TagState
        // parser.setState(std::make_unique<TagState>());
    }
}