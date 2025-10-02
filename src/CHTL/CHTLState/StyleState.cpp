#include "StyleState.h"
#include "TagState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/StyleNode.h"
#include <iostream>

StyleState::StyleState() {}

void StyleState::handle(CHTLParser& parser, Token token) {
    // In raw content mode, we expect a single token with the block's content.
    if (token.type == TokenType::STRING_LITERAL) {
        parser.addNode(std::make_unique<StyleNode>(token.value));
    }
    // After processing the raw content, transition back to the TagState.
    parser.setState(std::make_unique<TagState>());
}