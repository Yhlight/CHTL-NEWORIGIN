#pragma once

#include <string>

namespace chtl {

enum class TokenType {
	Identifier,
	Number,
	String,
	LBrace,    // {
	RBrace,    // }
	LBracket,  // [
	RBracket,  // ]
	LParen,    // (
	RParen,    // )
	Colon,     // :
	Equal,     // =
	Semicolon, // ;
	Comma,     // ,
	At,        // @
	Hash,      // # (generator annotation)
	Slash,     // /
	Dot,       // .
	Plus,Minus,Star,SlashOp,Percent,Caret, // + - * / % ^ (use ^ as ** helper)
	Greater,Less,
	Question, // ?
	Pipe,     // |
	Ampersand,// &
	Arrow,    // -> for CHTL JS
	CEEqual,  // := CE对等式 (Colon=Equal) treated equivalent
	TextBlock, // text { ... } captured as a single token in early scaffold
	EndOfFile,
	Error
};

struct Token {
	TokenType type;
	std::string lexeme;
	int line;
	int column;
};

const char* toString(TokenType t);

class Lexer {
public:
	explicit Lexer(const std::string& source);
	Token nextToken();

private:
	const std::string& src;
	size_t index;
	int line;
	int column;

	char peek() const;
	char peekNext() const;
	char advance();
	bool match(char c);
	void skipWhitespaceAndComments();
	Token makeToken(TokenType t, const std::string& lexeme);
	Token identifierOrLiteral();
	Token number();
	Token string();
};

} // namespace chtl

