#pragma once

#include "Node.h"
#include "../CHTLLexer/Token.h"
#include <vector>

class RawBodyNode : public Node {
public:
    RawBodyNode(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

    void accept(Visitor& visitor) override {
        // Not visited by generator
    }

    const std::vector<Token>& getTokens() const { return tokens; }

private:
    std::vector<Token> tokens;
};
