#include "TagState.h"
#include "../CHTLParser/CHTLParser.h"

#include <iostream>

void TagState::handle(CHTLParser& parser, Token token) {
    // Placeholder implementation
    std::cout << "TagState handling token: " << token.value << std::endl;
}