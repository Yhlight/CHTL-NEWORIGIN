#include "Parser.h"
#include "CHTL/CHTLState/StatementState.h" // Include the concrete state
#include "CHTL/CHTLState/ParserState.h"    // Include full definition for destructor
#include "../CHTLLexer/Token.h"               // Include full Token definition for snapshot
#include <stdexcept>

Parser::~Parser() = default;

ParserStateSnapshot Parser::saveState() {
    return {currentToken, peekToken, lexer.getPosition()};
}

void Parser::restoreState(const ParserStateSnapshot& snapshot) {
    lexer.setPosition(snapshot.lexerPosition);
    currentToken = snapshot.currentToken;
    peekToken = snapshot.peekToken;
}

// The constructor now initializes the token stream and sets the initial state.
Parser::Parser(Lexer& lexer)
    : lexer(lexer),
      currentToken({TokenType::Unexpected, ""}),
      peekToken({TokenType::Unexpected, ""}),
      contextNode(nullptr) {
    // Initialize the token stream.
    advanceTokens();
    advanceTokens();
    // Set the initial state for parsing.
    currentState = std::make_unique<StatementState>();
}

// The main parse loop now delegates to the current state's handle method.
std::vector<std::unique_ptr<BaseNode>> Parser::parse() {
    std::vector<std::unique_ptr<BaseNode>> statements;
    while (currentToken.type != TokenType::EndOfFile) {
        if (!currentState) {
            throw std::runtime_error("Parser is not in a valid state.");
        }
        // Delegate parsing to the current state.
        statements.push_back(currentState->handle(*this));
    }
    return statements;
}

// Allows changing the parser's state.
void Parser::setState(std::unique_ptr<ParserState> newState) {
    currentState = std::move(newState);
}

// --- Public methods for states to use ---

void Parser::advanceTokens() {
    currentToken = peekToken;
    peekToken = lexer.getNextToken();
}

void Parser::expectToken(TokenType type) {
    if (currentToken.type == type) {
        advanceTokens();
    } else {
        throw std::runtime_error("Unexpected token: '" + currentToken.value + "' at line " +
                                 std::to_string(currentToken.line) + ", column " +
                                 std::to_string(currentToken.column) + ". Expected type " +
                                 std::to_string(static_cast<int>(type)));
    }
}

std::string Parser::getCurrentNamespace() const {
    if (namespaceStack.empty()) {
        return "_global"; // Default namespace for top-level items
    }
    std::string result;
    for (size_t i = 0; i < namespaceStack.size(); ++i) {
        result += namespaceStack[i];
        if (i < namespaceStack.size() - 1) {
            result += ".";
        }
    }
    return result;
}
