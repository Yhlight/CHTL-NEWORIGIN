#include "lexer/Lexer.hpp"
#include <cctype>

namespace chtl {

static const char* TOKEN_NAMES[] = {
	"Identifier","Number","String","LBrace","RBrace","LBracket","RBracket","LParen","RParen","Colon","Equal","Semicolon","Comma","At","Hash","Slash","Dot","Plus","Minus","Star","SlashOp","Percent","Caret","Greater","Less","Question","Pipe","Ampersand","Arrow","CEEqual","TextBlock","EndOfFile","Error"
};

const char* toString(TokenType t) {
	return TOKEN_NAMES[static_cast<int>(t)];
}

Lexer::Lexer(const std::string& source)
	: src(source), index(0), line(1), column(1) {}

char Lexer::peek() const {
	if (index >= src.size()) return '\0';
	return src[index];
}

char Lexer::peekNext() const {
	if (index + 1 >= src.size()) return '\0';
	return src[index + 1];
}

char Lexer::advance() {
	if (index >= src.size()) return '\0';
	char c = src[index++];
	if (c == '\n') {
		line++;
		column = 1;
	} else {
		column++;
	}
	return c;
}

bool Lexer::match(char c) {
	if (peek() == c) { advance(); return true; }
	return false;
}

Token Lexer::makeToken(TokenType t, const std::string& lexeme) {
	return Token{t, lexeme, line, column};
}

void Lexer::skipWhitespaceAndComments() {
	for (;;) {
		char c = peek();
		// whitespace
		while (c == ' ' || c == '\t' || c == '\r' || c == '\n') { advance(); c = peek(); }
		// line comment //...
		if (c == '/' && peekNext() == '/') {
			while (peek() != '\n' && peek() != '\0') advance();
			continue;
		}
		// block comment /* ... */
		if (c == '/' && peekNext() == '*') {
			advance(); advance();
			while (!(peek() == '*' && peekNext() == '/') && peek() != '\0') advance();
			if (peek() == '*') { advance(); advance(); }
			continue;
		}
		break;
	}
}

Token Lexer::identifierOrLiteral() {
	std::string buffer;
	char c = peek();
	// allow unicode? For now ASCII letters, underscore, at, hash handled separately
	while (std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-' ) {
		buffer.push_back(advance());
		c = peek();
	}
	return makeToken(TokenType::Identifier, buffer);
}

Token Lexer::number() {
	std::string buffer;
	char c = peek();
	while (std::isdigit(static_cast<unsigned char>(c))) {
		buffer.push_back(advance());
		c = peek();
	}
	// optional decimal
	if (c == '.') {
		buffer.push_back(advance());
		c = peek();
		while (std::isdigit(static_cast<unsigned char>(c))) {
			buffer.push_back(advance());
			c = peek();
		}
	}
	return makeToken(TokenType::Number, buffer);
}

Token Lexer::string() {
	char quote = advance(); // consume ' or "
	std::string buffer;
	for (;;) {
		char c = peek();
		if (c == '\0') return makeToken(TokenType::Error, "Unterminated string");
		if (c == quote) { advance(); break; }
		if (c == '\\') {
			advance();
			char esc = advance();
			switch (esc) {
				case 'n': buffer.push_back('\n'); break;
				case 't': buffer.push_back('\t'); break;
				case 'r': buffer.push_back('\r'); break;
				case '\\': buffer.push_back('\\'); break;
				case '\'': buffer.push_back('\''); break;
				case '"': buffer.push_back('"'); break;
				default: buffer.push_back(esc); break;
			}
		} else {
			buffer.push_back(advance());
		}
	}
	return makeToken(TokenType::String, buffer);
}

Token Lexer::nextToken() {
	skipWhitespaceAndComments();
	char c = peek();
	if (c == '\0') return makeToken(TokenType::EndOfFile, "");

	// strings
	if (c == '"' || c == '\'') return string();

	// numbers
	if (std::isdigit(static_cast<unsigned char>(c))) return number();

	// identifiers or unquoted literals (letters, underscore, hyphen)
	if (std::isalpha(static_cast<unsigned char>(c)) || c == '_' ) return identifierOrLiteral();

	// single/double char tokens
	switch (advance()) {
		case '{': return makeToken(TokenType::LBrace, "{");
		case '}': return makeToken(TokenType::RBrace, "}");
		case '[': return makeToken(TokenType::LBracket, "[");
		case ']': return makeToken(TokenType::RBracket, "]");
		case '(': return makeToken(TokenType::LParen, "(");
		case ')': return makeToken(TokenType::RParen, ")");
		case ':':
			if (match('=')) return makeToken(TokenType::CEEqual, ":=");
			return makeToken(TokenType::Colon, ":");
		case '=': return makeToken(TokenType::Equal, "=");
		case ';': return makeToken(TokenType::Semicolon, ";");
		case ',': return makeToken(TokenType::Comma, ",");
		case '@': return makeToken(TokenType::At, "@");
		case '#': return makeToken(TokenType::Hash, "#");
		case '/': return makeToken(TokenType::Slash, "/");
		case '.': return makeToken(TokenType::Dot, ".");
		case '+': return makeToken(TokenType::Plus, "+");
		case '-':
			if (match('>')) return makeToken(TokenType::Arrow, "->");
			return makeToken(TokenType::Minus, "-");
		case '*': return makeToken(TokenType::Star, "*");
		case '%': return makeToken(TokenType::Percent, "%");
		case '^': return makeToken(TokenType::Caret, "^");
		case '>': return makeToken(TokenType::Greater, ">");
		case '<': return makeToken(TokenType::Less, "<");
		case '?': return makeToken(TokenType::Question, "?");
		case '|': return makeToken(TokenType::Pipe, "|");
		case '&': return makeToken(TokenType::Ampersand, "&");
	}

	return makeToken(TokenType::Error, std::string("Unexpected char: ") + c);
}

} // namespace chtl

