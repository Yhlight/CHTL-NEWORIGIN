#include "ScriptState.h"
#include "../CHTLParser/CHTLParser.h"

#include <iostream>

void ScriptState::handle(CHTLParser& parser, Token token) {
    // Placeholder implementation
    std::cout << "ScriptState handling token: " << token.value << std::endl;
}