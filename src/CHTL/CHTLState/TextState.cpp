#include "TextState.h"
#include "../CHTLParser/CHTLParser.h"

#include <iostream>

void TextState::handle(CHTLParser& parser, Token token) {
    // Placeholder implementation
    std::cout << "TextState handling token: " << token.value << std::endl;
}