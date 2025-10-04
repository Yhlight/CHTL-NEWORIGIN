#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLState/ParserState.h"
#include "../CHTLStrategy/ParsingStrategy.h"
#include "../CHTLNode/ElementNode.h"
#include <vector>
#include <memory>

namespace CHTL {

    class CHTLParserContext {
    public:
        CHTLParserContext(const std::vector<Token>& tokens);
        std::shared_ptr<BaseNode> parse();
        std::shared_ptr<BaseNode> runCurrentStrategy();
        void setState(std::unique_ptr<ParserState> newState);
        void setStrategy(std::unique_ptr<ParsingStrategy> newStrategy);
        Token& getCurrentToken();
        Token& peek(size_t offset);
        void advance();
        bool isAtEnd();

        void setCurrentElement(std::shared_ptr<ElementNode> element) { currentElement = element; }
        std::shared_ptr<ElementNode> getCurrentElement() { return currentElement; }

    private:
        std::vector<Token> tokens;
        size_t position;
        std::unique_ptr<ParserState> currentState;
        std::unique_ptr<ParsingStrategy> currentStrategy;
        std::shared_ptr<ElementNode> currentElement;
    };

}