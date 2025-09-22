#include "Lexer.h"
#include <cctype>
#include <unordered_map>

// Helper to check if a character can be part of an identifier
bool isIdentifierChar(char ch) {
    return std::isalnum(ch) || ch == '_' || ch == '-';
}

// Keyword lookup table
std::unordered_map<std::string, TokenType> keywords = {
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    {"use", TokenType::USE},
    {"inherit", TokenType::INHERIT},
    {"delete", TokenType::DELETE},
    {"insert", TokenType::INSERT},
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"except", TokenType::EXCEPT},
    {"Custom", TokenType::CUSTOM},
    {"Template", TokenType::TEMPLATE},
    {"Origin", TokenType::ORIGIN},
    {"Import", TokenType::IMPORT},
    {"Namespace", TokenType::NAMESPACE},
    {"Configuration", TokenType::CONFIGURATION},
    {"Info", TokenType::INFO},
    {"Export", TokenType::EXPORT},
    {"Name", TokenType::NAME},
    {"OriginType", TokenType::ORIGINTYPE},
    {"Html", TokenType::HTML},
    {"JavaScript", TokenType::JAVASCRIPT},
    {"Chtl", TokenType::CHTL},
    {"CJmod", TokenType::CJMOD},
    {"Config", TokenType::CONFIG},
};

Lexer::Lexer(const std::string& input)
    : input(input), position(0), readPosition(0), ch(0), line(1), column(0), _skipWhitespace(true) {
    readChar();
}

void Lexer::readChar() {
    if (readPosition >= input.length()) {
        ch = 0; // EOF
    } else {
        ch = input[readPosition];
    }
    position = readPosition;
    readPosition++;

    if (ch == '\n') {
        line++;
        column = 0;
    } else {
        column++;
    }
}

char Lexer::peekChar() const {
    if (readPosition >= input.length()) {
        return 0;
    }
    return input[readPosition];
}

void Lexer::skipWhitespace() {
    while (std::isspace(ch)) {
        readChar();
    }
}

Token Lexer::NextToken() {
    Token tok;

    if (std::isspace(ch)) {
        if (_skipWhitespace) {
            skipWhitespace();
        } else {
            tok.line = line;
            tok.column = column;
            size_t start_pos = position;
            while (std::isspace(ch)) {
                readChar();
            }
            tok.literal = input.substr(start_pos, position - start_pos);
            tok.type = TokenType::WHITESPACE;
            return tok;
        }
    }

    tok.line = line;
    tok.column = column;

    switch (ch) {
        case '{': tok = {TokenType::LEFT_BRACE, "{", line, column}; break;
        case '}': tok = {TokenType::RIGHT_BRACE, "}", line, column}; break;
        case '(': tok = {TokenType::LEFT_PAREN, "(", line, column}; break;
        case ')': tok = {TokenType::RIGHT_PAREN, ")", line, column}; break;
        case '[': tok = {TokenType::LEFT_BRACKET, "[", line, column}; break;
        case ']': tok = {TokenType::RIGHT_BRACKET, "]", line, column}; break;
        case ':': tok = {TokenType::COLON, ":", line, column}; break;
        case ';': tok = {TokenType::SEMICOLON, ";", line, column}; break;
        case ',': tok = {TokenType::COMMA, ",", line, column}; break;
        case '.': tok = {TokenType::DOT, ".", line, column}; break;
        case '?': tok = {TokenType::QUESTION_MARK, "?", line, column}; break;
        case '@': tok = {TokenType::AT, "@", line, column}; break;
        case '#': tok = {TokenType::HASH, "#", line, column}; break;
        case '&': tok = {TokenType::AMPERSAND, "&", line, column}; break;
        case '+': tok = {TokenType::PLUS, "+", line, column}; break;
        case '-': tok = {TokenType::MINUS, "-", line, column}; break;
        case '%': tok = {TokenType::PERCENT, "%", line, column}; break;
        case '=': tok = {TokenType::EQUALS, "=", line, column}; break;

        case '*':
            if (peekChar() == '*') {
                readChar();
                tok = {TokenType::DOUBLE_ASTERISK, "**", line, column};
            } else {
                tok = {TokenType::ASTERISK, "*", line, column};
            }
            break;

        case '/':
            if (peekChar() == '/') {
                readChar();
                while (ch != '\n' && ch != 0) readChar();
                return NextToken();
            } else if (peekChar() == '*') {
                readChar(); readChar();
                while (ch != 0 && !(ch == '*' && peekChar() == '/')) readChar();
                if (ch != 0) { readChar(); readChar(); }
                return NextToken();
            } else {
                tok = {TokenType::SLASH, "/", line, column};
            }
            break;

        case '"':
        case '\'':
            return readStringLiteral();

        case 0:
            tok = {TokenType::END_OF_FILE, "", line, column};
            break;

        default:
            if (std::isalpha(ch) || ch == '_') {
                return readIdentifier();
            } else if (std::isdigit(ch)) {
                return readNumberLiteral();
            } else {
                tok = {TokenType::ILLEGAL, std::string(1, ch), line, column};
            }
            break;
    }

    readChar();
    return tok;
}

Token Lexer::readIdentifier() {
    Token tok;
    tok.line = line;
    tok.column = column;
    size_t start_pos = position;
    while (isIdentifierChar(ch)) {
        readChar();
    }
    tok.literal = input.substr(start_pos, position - start_pos);
    tok.type = lookupIdent(tok.literal);
    return tok;
}

Token Lexer::readStringLiteral() {
    Token tok;
    tok.line = line;
    tok.column = column;
    char quote_type = ch;
    readChar();
    size_t start_pos = position;
    while (ch != quote_type && ch != 0) {
        readChar();
    }
    tok.literal = input.substr(start_pos, position - start_pos);
    tok.type = TokenType::STRING_LITERAL;
    if (ch == quote_type) readChar();
    return tok;
}

Token Lexer::readNumberLiteral() {
    Token tok;
    tok.line = line;
    tok.column = column;
    size_t start_pos = position;
    bool has_dot = false;
    while (std::isdigit(ch) || (ch == '.' && !has_dot)) {
        if (ch == '.') has_dot = true;
        readChar();
    }
    tok.literal = input.substr(start_pos, position - start_pos);
    tok.type = TokenType::NUMBER_LITERAL;
    return tok;
}

TokenType Lexer::lookupIdent(const std::string& ident) {
    if (keywords.count(ident)) {
        return keywords.at(ident);
    }
    return TokenType::IDENTIFIER;
}

// Unquoted literals are just identifiers that the parser treats differently.
// This function is not currently called from the main switch.
Token Lexer::readUnquotedLiteral() {
    return readIdentifier();
}
