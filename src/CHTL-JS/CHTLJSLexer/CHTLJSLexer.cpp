#include "CHTLJSLexer.h"

namespace CHTL {
namespace JS {

CHTLJSLexer::CHTLJSLexer(const String& source)
    : source(source), current(0) {
    currentPos = Position(1, 1, 0);
    initializeKeywords();
}

void CHTLJSLexer::initializeKeywords() {
    // CHTL JS关键字
    keywords["Vir"] = JSTokenType::Vir;
    keywords["Listen"] = JSTokenType::Listen;
    keywords["Delegate"] = JSTokenType::Delegate;
    keywords["Animate"] = JSTokenType::Animate;
    keywords["Router"] = JSTokenType::Router;
    keywords["ScriptLoader"] = JSTokenType::ScriptLoader;
    
    // JavaScript基础关键字
    keywords["const"] = JSTokenType::Const;
    keywords["let"] = JSTokenType::Let;
    keywords["var"] = JSTokenType::Var;
    keywords["function"] = JSTokenType::Function;
    keywords["return"] = JSTokenType::Return;
    keywords["if"] = JSTokenType::If;
    keywords["else"] = JSTokenType::Else;
    keywords["for"] = JSTokenType::For;
    keywords["while"] = JSTokenType::While;
    keywords["break"] = JSTokenType::Break;
    keywords["continue"] = JSTokenType::Continue;
}

JSToken CHTLJSLexer::nextToken() {
    if (cachedToken.has_value()) {
        JSToken token = cachedToken.value();
        cachedToken.reset();
        return token;
    }
    
    return scanToken();
}

JSToken CHTLJSLexer::peekToken() {
    if (!cachedToken.has_value()) {
        cachedToken = scanToken();
    }
    return cachedToken.value();
}

Vector<JSToken> CHTLJSLexer::tokenize() {
    Vector<JSToken> tokens;
    
    while (!isAtEnd()) {
        JSToken token = nextToken();
        if (token.getType() != JSTokenType::EndOfFile) {
            tokens.push_back(token);
        }
    }
    
    tokens.push_back(makeToken(JSTokenType::EndOfFile, ""));
    return tokens;
}

char CHTLJSLexer::advance() {
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

char CHTLJSLexer::peek() const {
    if (isAtEnd()) {
        return '\0';
    }
    return source[current];
}

char CHTLJSLexer::peekNext() const {
    if (current + 1 >= source.length()) {
        return '\0';
    }
    return source[current + 1];
}

bool CHTLJSLexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) {
        return false;
    }
    advance();
    return true;
}

void CHTLJSLexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (isWhitespace(c)) {
            advance();
        } else {
            break;
        }
    }
}

JSToken CHTLJSLexer::scanToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return makeToken(JSTokenType::EndOfFile, "");
    }
    
    tokenStart = currentPos;
    char c = advance();
    
    // 字符串字面量
    if (c == '"' || c == '\'') {
        current--;
        currentPos.offset--;
        currentPos.column--;
        return scanStringLiteral(c);
    }
    
    // 数字字面量
    if (isDigit(c)) {
        current--;
        currentPos.offset--;
        currentPos.column--;
        return scanNumberLiteral();
    }
    
    // 增强选择器 {{...}}
    if (c == '{' && peek() == '{') {
        current--;
        currentPos.offset--;
        currentPos.column--;
        return scanEnhancedSelector();
    }
    
    // 标识符或关键字
    if (isIdentifierStart(c)) {
        current--;
        currentPos.offset--;
        currentPos.column--;
        return scanIdentifierOrKeyword();
    }
    
    // 单字符Token
    switch (c) {
        case '{': return makeToken(JSTokenType::LeftBrace, "{");
        case '}': return makeToken(JSTokenType::RightBrace, "}");
        case '[': return makeToken(JSTokenType::LeftBracket, "[");
        case ']': return makeToken(JSTokenType::RightBracket, "]");
        case '(': return makeToken(JSTokenType::LeftParen, "(");
        case ')': return makeToken(JSTokenType::RightParen, ")");
        case ';': return makeToken(JSTokenType::Semicolon, ";");
        case ':': return makeToken(JSTokenType::Colon, ":");
        case ',': return makeToken(JSTokenType::Comma, ",");
        case '.': return makeToken(JSTokenType::Dot, ".");
        case '=': return makeToken(JSTokenType::Equal, "=");
        case '+': return makeToken(JSTokenType::Plus, "+");
        case '*': return makeToken(JSTokenType::Star, "*");
        case '/': return makeToken(JSTokenType::Slash, "/");
        case '%': return makeToken(JSTokenType::Percent, "%");
        case '?': return makeToken(JSTokenType::Question, "?");
        case '-':
            if (match('>')) return makeToken(JSTokenType::Arrow, "->");
            return makeToken(JSTokenType::Minus, "-");
        case '&':
            if (match('-') && match('>')) return makeToken(JSTokenType::EventBind, "&->");
            return makeToken(JSTokenType::Ampersand, "&");
    }
    
    error("Unexpected character: " + String(1, c));
    return makeToken(JSTokenType::Unknown, String(1, c));
}

JSToken CHTLJSLexer::scanIdentifierOrKeyword() {
    String value;
    
    while (!isAtEnd() && isIdentifierChar(peek())) {
        value += advance();
    }
    
    // 检查是否是关键字
    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return makeToken(it->second, value);
    }
    
    return makeToken(JSTokenType::Identifier, value);
}

JSToken CHTLJSLexer::scanStringLiteral(char /* quote */) {
    char openQuote = advance();
    String value;
    
    while (!isAtEnd() && peek() != openQuote) {
        if (peek() == '\\') {
            advance();
            if (!isAtEnd()) {
                value += advance();
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
    
    return makeToken(JSTokenType::StringLiteral, value);
}

JSToken CHTLJSLexer::scanNumberLiteral() {
    String value;
    
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
    
    return makeToken(JSTokenType::NumberLiteral, value);
}

JSToken CHTLJSLexer::scanEnhancedSelector() {
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
    
    return makeToken(JSTokenType::EnhancedSelector, value);
}

bool CHTLJSLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool CHTLJSLexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool CHTLJSLexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool CHTLJSLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLJSLexer::isIdentifierStart(char c) const {
    return isAlpha(c) || c == '_' || c == '$';
}

bool CHTLJSLexer::isIdentifierChar(char c) const {
    return isAlphaNumeric(c) || c == '_' || c == '$';
}

JSToken CHTLJSLexer::makeToken(JSTokenType type, const String& value) {
    return JSToken(type, value, tokenStart);
}

JSToken CHTLJSLexer::makeToken(JSTokenType type) {
    return makeToken(type, "");
}

void CHTLJSLexer::error(const String& message) {
    throw CompileError(message, currentPos);
}

} // namespace JS
} // namespace CHTL
