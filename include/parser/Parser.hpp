#pragma once

#include <memory>
#include <vector>
#include <string>
#include "lexer/Lexer.hpp"

namespace chtl {

// Forward declarations for AST
struct Node { virtual ~Node() = default; };
using NodePtr = std::unique_ptr<Node>;

// Parser driven by state machine + strategies
enum class ParserStateId {
	TopLevel,
	Element,
	Attributes,
	Block,
	StyleBlock,
	ScriptBlock,
	TextBlock,
};

class IParseStrategy {
public:
	virtual ~IParseStrategy() = default;
	virtual bool matches(ParserStateId state, const Token& lookahead) const = 0;
	virtual NodePtr parse(class Parser& p) = 0;
};

class Parser {
public:
	explicit Parser(Lexer lexer);
	NodePtr parse();

	// helpers
	const Token& lookahead();
	Token consume();
	bool match(TokenType t);
	bool expect(TokenType t, const char* what);

	void pushState(ParserStateId);
	void popState();
	ParserStateId currentState() const;

private:
	Lexer lexer;
	Token current;
	bool hasCurrent;
	std::vector<ParserStateId> stateStack;
	std::vector<std::unique_ptr<IParseStrategy>> strategies;

	void registerDefaultStrategies();
};

} // namespace chtl

