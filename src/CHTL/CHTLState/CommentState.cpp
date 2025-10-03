#include "CommentState.h"
#include "../CHTLParser/CHTLParser.h"

#include <iostream>

void CommentState::handle(CHTLParser& parser, Token token) {
    // Placeholder implementation
    std::cout << "CommentState handling token: " << token.value << std::endl;
}