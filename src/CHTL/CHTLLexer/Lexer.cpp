#include "Lexer.h"
#include "../../Util/StringUtil/StringUtil.h"

namespace CHTL {

Lexer::Lexer(const String& source) 
    : source(source), current(0) {
    currentPos = Position(1, 1, 0);
}

Token Lexer::nextToken() {
    if (cachedToken.has_value()) {
        Token token = cachedToken.value();
        cachedToken.reset();
        return token;
    }
    
    return scanToken();
}

Token Lexer::peekToken() {
    if (!cachedToken.has_value()) {
        cachedToken = scanToken();
    }
    return cachedToken.value();
}

Vector<Token> Lexer::tokenize() {
    Vector<Token> tokens;
    
    while (!isAtEnd()) {
        Token token = nextToken();
        if (token.getType() != TokenType::EndOfFile) {
            tokens.push_back(token);
        }
    }
    
    tokens.push_back(makeToken(TokenType::EndOfFile, ""));
    return tokens;
}

char Lexer::advance() {
    if (isAtEnd()) {
        return '\0';
    }
    
    char c = source[current++];
    currentPos.offset++;
    
    if (c == '\n') {
        currentPos.line++;
        currentPos.column = 1;
    } else {
        currentPos.column++;
    }
    
    return c;
}

char Lexer::peek() const {
    if (isAtEnd()) {
        return '\0';
    }
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.length()) {
        return '\0';
    }
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) {
        return false;
    }
    advance();
    return true;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (isWhitespace(c)) {
            advance();
        } else {
            break;
        }
    }
}

Token Lexer::scanToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return makeToken(TokenType::EndOfFile, "");
    }
    
    tokenStart = currentPos;
    char c = advance();
    
    // 注释
    if (c == '/') {
        if (peek() == '/' || peek() == '*') {
            // 回退一个字符
            current--;
            currentPos.offset--;
            currentPos.column--;
            return scanComment();
        }
        return makeToken(TokenType::Slash, "/");
    }
    
    if (c == '#') {
        if (peek() == ' ') {
            // 回退一个字符
            current--;
            currentPos.offset--;
            currentPos.column--;
            return scanGeneratorComment();
        }
        return makeToken(TokenType::Hash, "#");
    }
    
    // 字符串字面量
    if (c == '"' || c == '\'') {
        // 回退一个字符
        current--;
        currentPos.offset--;
        currentPos.column--;
        return scanStringLiteral(c);
    }
    
    // 数字字面量
    if (isDigit(c) || (c == '-' && isDigit(peek()))) {
        // 回退一个字符
        current--;
        currentPos.offset--;
        currentPos.column--;
        return scanNumberLiteral();
    }
    
    // 标识符或关键字
    if (isIdentifierStart(c)) {
        // 回退一个字符
        current--;
        currentPos.offset--;
        currentPos.column--;
        return scanIdentifierOrKeyword();
    }
    
    // 增强选择器 {{...}}
    if (c == '{' && peek() == '{') {
        // 回退一个字符
        current--;
        currentPos.offset--;
        currentPos.column--;
        return scanEnhancedSelector();
    }
    
    // 方括号（可能是关键字）
    if (c == '[') {
        size_t savedCurrent = current;
        Position savedPos = currentPos;
        
        String bracket = "[";
        while (!isAtEnd() && peek() != ']') {
            bracket += advance();
        }
        if (!isAtEnd() && peek() == ']') {
            bracket += advance();
            
            auto type = globalMap.getKeywordType(bracket);
            if (type.has_value()) {
                return makeToken(type.value(), bracket);
            }
        }
        
        // 回退
        current = savedCurrent;
        currentPos = savedPos;
        return makeToken(TokenType::LeftBracket, "[");
    }
    
    // 单字符Token
    switch (c) {
        case '{': return makeToken(TokenType::LeftBrace, "{");
        case '}': return makeToken(TokenType::RightBrace, "}");
        case ']': return makeToken(TokenType::RightBracket, "]");
        case '(': return makeToken(TokenType::LeftParen, "(");
        case ')': return makeToken(TokenType::RightParen, ")");
        case ';': return makeToken(TokenType::Semicolon, ";");
        case ',': return makeToken(TokenType::Comma, ",");
        case '.': return makeToken(TokenType::Dot, ".");
        case '?': return makeToken(TokenType::Question, "?");
        case '$': return makeToken(TokenType::Dollar, "$");
        
        // 可能是多字符的Token
        case ':': return makeToken(TokenType::Colon, ":");
        case '=':
            if (match('=')) return makeToken(TokenType::EqualEqual, "==");
            return makeToken(TokenType::Equal, "=");
        case '+': return makeToken(TokenType::Plus, "+");
        case '%': return makeToken(TokenType::Percent, "%");
        case '*':
            if (match('*')) return makeToken(TokenType::Power, "**");
            return makeToken(TokenType::Star, "*");
        case '!':
            if (match('=')) return makeToken(TokenType::NotEqual, "!=");
            return makeToken(TokenType::Not, "!");
        case '<':
            if (match('=')) return makeToken(TokenType::LessEqual, "<=");
            return makeToken(TokenType::Less, "<");
        case '>':
            if (match('=')) return makeToken(TokenType::GreaterEqual, ">=");
            return makeToken(TokenType::Greater, ">");
        case '&':
            if (match('&')) return makeToken(TokenType::And, "&&");
            if (match('-') && match('>')) return makeToken(TokenType::EventBind, "&->");
            return makeToken(TokenType::Ampersand, "&");
        case '|':
            if (match('|')) return makeToken(TokenType::Or, "||");
            return makeToken(TokenType::Unknown, "|");
        case '-':
            if (match('>')) return makeToken(TokenType::Arrow, "->");
            return makeToken(TokenType::Minus, "-");
        case '@':
            // @后面跟标识符，组成关键字
            if (isIdentifierStart(peek())) {
                String keyword = "@";
                while (!isAtEnd() && isIdentifierChar(peek())) {
                    keyword += advance();
                }
                auto type = globalMap.getKeywordType(keyword);
                if (type.has_value()) {
                    return makeToken(type.value(), keyword);
                }
                // 自定义原始嵌入类型
                return makeToken(TokenType::Identifier, keyword);
            }
            return makeToken(TokenType::At, "@");
    }
    
    // 无修饰字面量（CSS样式的值）
    if (!isWhitespace(c)) {
        current--;
        currentPos.offset--;
        currentPos.column--;
        return scanUnquotedLiteral();
    }
    
    error("Unexpected character: " + String(1, c));
    return makeToken(TokenType::Unknown, String(1, c));
}

Token Lexer::scanIdentifierOrKeyword() {
    String value;
    
    while (!isAtEnd() && isIdentifierChar(peek())) {
        value += advance();
    }
    
    // 检查是否是多词关键字（如 "at top", "at bottom"）
    if (value == "at") {
        skipWhitespace();
        size_t savedCurrent = current;
        Position savedPos = currentPos;
        
        String next;
        while (!isAtEnd() && isAlpha(peek())) {
            next += advance();
        }
        
        if (next == "top") {
            return makeToken(TokenType::AtTopKeyword, "at top");
        } else if (next == "bottom") {
            return makeToken(TokenType::AtBottomKeyword, "at bottom");
        }
        
        // 回退
        current = savedCurrent;
        currentPos = savedPos;
    }
    
    // 检查是否是关键字
    auto type = globalMap.getKeywordType(value);
    if (type.has_value()) {
        return makeToken(type.value(), value);
    }
    
    return makeToken(TokenType::Identifier, value);
}

Token Lexer::scanStringLiteral(char /* quote */) {
    char openQuote = advance();  // 消耗引号
    String value;
    
    while (!isAtEnd() && peek() != openQuote) {
        if (peek() == '\\') {
            advance();  // 消耗反斜杠
            if (!isAtEnd()) {
                value += advance();  // 消耗转义字符
            }
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        error("Unterminated string literal");
    } else {
        advance();  // 消耗结束引号
    }
    
    return makeToken(TokenType::StringLiteral, value);
}

Token Lexer::scanNumberLiteral() {
    String value;
    
    // 处理负号
    if (peek() == '-') {
        value += advance();
    }
    
    // 整数部分
    while (!isAtEnd() && isDigit(peek())) {
        value += advance();
    }
    
    // 小数部分
    if (peek() == '.' && isDigit(peekNext())) {
        value += advance();  // 消耗.
        while (!isAtEnd() && isDigit(peek())) {
            value += advance();
        }
    }
    
    // CSS单位（可选）- 支持字母单位和百分号
    if (!isAtEnd() && (isAlpha(peek()) || peek() == '%')) {
        while (!isAtEnd() && (isAlpha(peek()) || peek() == '%')) {
            value += advance();
        }
    }
    
    return makeToken(TokenType::NumberLiteral, value);
}

Token Lexer::scanUnquotedLiteral() {
    String value;
    
    // 扫描直到遇到分隔符、空白或特殊字符
    while (!isAtEnd()) {
        char c = peek();
        if (isWhitespace(c) || c == ';' || c == ',' || c == '{' || c == '}' 
            || c == '(' || c == ')' || c == '[' || c == ']') {
            break;
        }
        value += advance();
    }
    
    return makeToken(TokenType::UnquotedLiteral, value);
}

Token Lexer::scanComment() {
    String value;
    advance();  // 消耗第一个/
    
    if (peek() == '/') {
        // 单行注释
        advance();  // 消耗第二个/
        while (!isAtEnd() && peek() != '\n') {
            value += advance();
        }
        return makeToken(TokenType::LineComment, value);
    } else if (peek() == '*') {
        // 块注释
        advance();  // 消耗*
        while (!isAtEnd()) {
            if (peek() == '*' && peekNext() == '/') {
                advance();  // 消耗*
                advance();  // 消耗/
                break;
            }
            value += advance();
        }
        return makeToken(TokenType::BlockComment, value);
    }
    
    error("Invalid comment");
    return makeToken(TokenType::Unknown, value);
}

Token Lexer::scanGeneratorComment() {
    String value;
    advance();  // 消耗#
    advance();  // 消耗空格
    
    while (!isAtEnd() && peek() != '\n') {
        value += advance();
    }
    
    return makeToken(TokenType::GeneratorComment, value);
}

Token Lexer::scanEnhancedSelector() {
    String value;
    advance();  // 消耗第一个{
    advance();  // 消耗第二个{
    
    while (!isAtEnd()) {
        if (peek() == '}' && peekNext() == '}') {
            advance();  // 消耗第一个}
            advance();  // 消耗第二个}
            break;
        }
        value += advance();
    }
    
    return makeToken(TokenType::EnhancedSelector, value);
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isIdentifierStart(char c) const {
    return isAlpha(c) || c == '_' || c == '$';
}

bool Lexer::isIdentifierChar(char c) const {
    return isAlphaNumeric(c) || c == '_' || c == '$' || c == '-';
}

Token Lexer::makeToken(TokenType type, const String& value) {
    SourceRange range(tokenStart, currentPos);
    return Token(type, value, range);
}

Token Lexer::makeToken(TokenType type) {
    return makeToken(type, "");
}

void Lexer::error(const String& message) {
    throw CompileError(message, currentPos);
}

} // namespace CHTL
