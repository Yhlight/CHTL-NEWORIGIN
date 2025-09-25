#include "Parser.h"
#include "CHTL/CHTLState/StatementState.h" // Include the concrete state
#include "CHTL/CHTLState/ParserState.h" // Include full definition for destructor
#include <stdexcept>
#include <sstream>

// The constructor now initializes the token stream and sets the initial state.
Parser::Parser(Lexer& lexer)
    : lexer(lexer),
      currentToken({TokenType::Unexpected, ""}),
      peekToken({TokenType::Unexpected, ""}),
      peekToken2({TokenType::Unexpected, ""}),
      contextNode(nullptr) {
    // Initialize the token stream to fill all three lookahead tokens.
    advanceTokens();
    advanceTokens();
    advanceTokens();
    // Set the initial state for parsing.
    currentState = std::make_unique<StatementState>();
}

// The main parse loop now delegates to the current state's handle method.
std::vector<std::unique_ptr<BaseNode>> Parser::parse() {
    std::vector<std::unique_ptr<BaseNode>> statements;
    this->parsedNodes = &statements; // Point to the vector we are building.

    while (currentToken.type != TokenType::EndOfFile) {
        if (!currentState) {
            throw std::runtime_error("Parser is not in a valid state.");
        }
        // Delegate parsing to the current state.
        auto node = currentState->handle(*this);
        if (node) { // The state might not return a node (e.g., for a template definition)
             statements.push_back(std::move(node));
        }
    }

    this->parsedNodes = nullptr; // Clean up the pointer.
    return statements;
}

// Allows changing the parser's state.
void Parser::setState(std::unique_ptr<ParserState> newState) {
    currentState = std::move(newState);
}

// --- Public methods for states to use ---

void Parser::advanceTokens() {
    currentToken = peekToken;
    peekToken = peekToken2;
    peekToken2 = lexer.getNextToken();
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

bool Parser::tryExpectKeyword(TokenType type, const std::string& internalName, const std::string& defaultValue) {
    const auto& aliases = configManager.getKeywordAliases(internalName);

    if (!aliases.empty()) {
        // User has defined aliases. Check only against them.
        for (const auto& alias : aliases) {
            if (currentToken.value == alias) {
                advanceTokens();
                return true;
            }
        }
        // If aliases are defined, we don't fall back to the default string value.
        return false;
    }

    // No aliases defined. Check against the default string value.
    if (currentToken.value == defaultValue) {
        advanceTokens();
        return true;
    }
    // As a final fallback for non-string-based tokens, check the token type.
    if (currentToken.type == type) {
        advanceTokens();
        return true;
    }

    return false;
}

void Parser::expectKeyword(TokenType type, const std::string& internalName, const std::string& defaultValue) {
    if (!tryExpectKeyword(type, internalName, defaultValue)) {
        const auto& aliases = configManager.getKeywordAliases(internalName);
        std::string expectedMessage;
        if (!aliases.empty()) {
            std::stringstream ss;
            ss << "one of [";
            for (size_t i = 0; i < aliases.size(); ++i) {
                ss << "'" << aliases[i] << "'" << (i < aliases.size() - 1 ? ", " : "");
            }
            ss << "]";
            expectedMessage = ss.str();
        } else {
            expectedMessage = "'" + defaultValue + "'";
        }
        throw std::runtime_error("Unexpected token: '" + currentToken.value + "'. Expected keyword " + expectedMessage + ".");
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

Parser::~Parser() = default;
