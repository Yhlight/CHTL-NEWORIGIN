#include "parser/Parser.hpp"
#include <stdexcept>

namespace chtl {

Parser::Parser(Lexer lexer)
	: lexer(lexer), current{}, hasCurrent(false) {
	stateStack.push_back(ParserStateId::TopLevel);
	registerDefaultStrategies();
}

const Token& Parser::lookahead() {
	if (!hasCurrent) {
		current = lexer.nextToken();
		hasCurrent = true;
	}
	return current;
}

Token Parser::consume() {
	const Token& la = lookahead();
	hasCurrent = false;
	return la;
}

bool Parser::match(TokenType t) {
	if (lookahead().type == t) { consume(); return true; }
	return false;
}

bool Parser::expect(TokenType t, const char* what) {
	if (!match(t)) {
		throw std::runtime_error(std::string("Expected ") + what);
	}
	return true;
}

void Parser::pushState(ParserStateId s) { stateStack.push_back(s); }
void Parser::popState() { if (stateStack.size() > 1) stateStack.pop_back(); }
ParserStateId Parser::currentState() const { return stateStack.back(); }

NodePtr Parser::parse() {
	auto doc = std::make_unique<ast::Document>();
	// Minimal loop to consume tokens for now
	while (lookahead().type != TokenType::EndOfFile) {
		consume();
	}
	return doc;
}

void Parser::registerDefaultStrategies() {
	// TODO: Register strategies for elements, blocks, attributes
}

} // namespace chtl


