#pragma once

#include <memory>
#include <vector>
#include <string>
#include "lexer/Lexer.hpp"
#include "ast/Ast.hpp"

namespace chtl {

// Forward declarations for AST
using NodePtr = std::unique_ptr<ast::Node>;

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
	const Token& lookahead2();
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
	Token next;
	bool hasNext;
	std::vector<ParserStateId> stateStack;
	std::vector<std::unique_ptr<IParseStrategy>> strategies;

	void registerDefaultStrategies();

	// helpers used by strategies
	NodePtr parseElement();
	std::unique_ptr<ast::Attribute> parseAttribute();
	std::unique_ptr<ast::Text> parseTextBlock();
	std::unique_ptr<ast::StyleBlock> parseStyleBlock();
};

} // namespace chtl

