#include "ElementParsingState.h"
#include "BodyParsingState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLStrategy/ElementStrategy.h"

namespace CHTL {

void ElementParsingState::handle(CHTLParser& parser) {
    // Delegate the parsing logic to the ElementStrategy.
    ElementStrategy strategy;
    strategy.execute(parser);

    // After the strategy has run, transition to the next state.
    parser.setState(std::make_unique<BodyParsingState>());
}

} // namespace CHTL