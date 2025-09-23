#include "parser/Parser.hpp"
#include <stdexcept>

namespace chtl {

Parser::Parser(Lexer lexer)
	: lexer(lexer), current{}, hasCurrent(false), next{}, hasNext(false) {
	stateStack.push_back(ParserStateId::TopLevel);
	registerDefaultStrategies();
}

const Token& Parser::lookahead() {
	if (!hasCurrent) {
		if (hasNext) {
			current = next;
			hasCurrent = true;
			hasNext = false;
			return current;
		}
		current = lexer.nextToken();
		hasCurrent = true;
	}
	return current;
}

const Token& Parser::lookahead2() {
	(void)lookahead();
	if (!hasNext) {
		next = lexer.nextToken();
		hasNext = true;
	}
	return next;
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
	while (lookahead().type != TokenType::EndOfFile) {
		if (lookahead().type == TokenType::Identifier) {
			doc->nodes.emplace_back(parseElement());
			continue;
		}
		// skip unexpected tokens for now in incremental build
		consume();
	}
	return doc;
}

// === Minimal helpers ===
NodePtr Parser::parseElement() {
	auto elem = std::make_unique<ast::Element>();
	// tag name
	const Token& tagTok = lookahead();
	if (tagTok.type != TokenType::Identifier) {
		throw std::runtime_error("Element tag expected");
	}
	elem->tag = tagTok.lexeme;
	consume();

	// element block { ... }
	expect(TokenType::LBrace, "'{'");
	pushState(ParserStateId::Element);
	while (lookahead().type != TokenType::RBrace && lookahead().type != TokenType::EndOfFile) {
		// style block
		if (lookahead().type == TokenType::Identifier && lookahead().lexeme == "style") {
			elem->style = parseStyleBlock();
			continue;
		}
		// text block
		if (lookahead().type == TokenType::Identifier && lookahead().lexeme == "text") {
			auto t = parseTextBlock();
			elem->children.emplace_back(std::move(t));
			continue;
		}
		// nested element
		if (lookahead().type == TokenType::Identifier && lookahead2().type == TokenType::LBrace) {
			elem->children.emplace_back(parseElement());
			continue;
		}
		// attribute
		if (lookahead().type == TokenType::Identifier && (lookahead2().type == TokenType::Colon || lookahead2().type == TokenType::Equal)) {
			elem->attributes.emplace_back(parseAttribute());
			continue;
		}
		// fallback consume
		consume();
	}
	expect(TokenType::RBrace, "'}'");
	popState();
	return elem;
}

std::unique_ptr<ast::Attribute> Parser::parseAttribute() {
	auto attr = std::make_unique<ast::Attribute>();
	const Token& name = lookahead();
	attr->name = name.lexeme;
	consume();
	if (!(match(TokenType::Colon) || match(TokenType::Equal))) {
		throw std::runtime_error("':' or '=' expected after attribute name");
	}
	// value: string | identifier (unquoted literal) | number
	const Token& val = lookahead();
	switch (val.type) {
		case TokenType::String:
		case TokenType::Identifier:
		case TokenType::Number:
			attr->value = val.lexeme; consume(); break;
		default:
			attr->value.clear();
	}
	// optional semicolon
	match(TokenType::Semicolon);
	return attr;
}

std::unique_ptr<ast::Text> Parser::parseTextBlock() {
	// text { ... }
	expect(TokenType::Identifier, "'text'");
	expect(TokenType::LBrace, "'{'");
	auto t = std::make_unique<ast::Text>();
	// naive content capture until matching }
	std::string content;
	while (lookahead().type != TokenType::RBrace && lookahead().type != TokenType::EndOfFile) {
		const Token& tok = consume();
		content += tok.lexeme;
		if (tok.type == TokenType::String) {
			// strings already without quotes
		}
	}
	expect(TokenType::RBrace, "'}'");
	t->content = content;
	return t;
}

std::unique_ptr<ast::StyleBlock> Parser::parseStyleBlock() {
	// style { key: value; ... }
	expect(TokenType::Identifier, "'style'");
	expect(TokenType::LBrace, "'{'");
	auto sb = std::make_unique<ast::StyleBlock>();
	while (lookahead().type != TokenType::RBrace && lookahead().type != TokenType::EndOfFile) {
		if (lookahead().type == TokenType::Identifier && (lookahead2().type == TokenType::Colon || lookahead2().type == TokenType::Equal)) {
			auto decl = parseAttribute();
			sb->declarations.emplace_back(std::move(decl));
			continue;
		}
		// nested selectors and advanced rules will be handled later
		consume();
	}
	expect(TokenType::RBrace, "'}'");
	return sb;
}

void Parser::registerDefaultStrategies() {
	// Concrete strategies will be introduced as separate classes; the helpers above serve as initial strategies.
}

} // namespace chtl


