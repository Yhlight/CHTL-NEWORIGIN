#include "StyleState.h"
#include "../CHTLParser/CHTLParser.h"

#include <iostream>

void StyleState::handle(CHTLParser& parser, Token token) {
    // Placeholder implementation
    std::cout << "StyleState handling token: " << token.value << std::endl;
}