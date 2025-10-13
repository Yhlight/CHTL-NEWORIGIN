#pragma once

#include "../Common.hpp"
#include "Token.hpp"

namespace CHTL {

class Lexer {
public:
    explicit Lexer(String source)
        : source_(std::move(source)), position_(0), line_(1), column_(1) {}
    
    // 获取下一个token
    Token nextToken() {
        skipWhitespace();
        
        if (isAtEnd()) {
            return Token(TokenType::EndOfFile, "", line_, column_);
        }
        
        char current = peek();
        
        // 注释处理
        if (current == '/' && peekNext() == '/') {
            return scanComment();
        }
        if (current == '/' && peekNext() == '*') {
            return scanMultiLineComment();
        }
        if (current == '#' && peekNext() == ' ') {
            return scanGeneratorComment();
        }
        
        // 字符串
        if (current == '"' || current == '\'') {
            return scanString();
        }
        
        // 数字
        if (std::isdigit(current)) {
            return scanNumber();
        }
        
        // 标识符和关键字
        if (std::isalpha(current) || current == '_') {
            return scanIdentifier();
        }
        
        // @ 符号开头的特殊标识符
        if (current == '@') {
            return scanAtIdentifier();
        }
        
        // [ 符号开头的关键字
        if (current == '[') {
            if (std::isalpha(peekNext())) {
                return scanBracketKeyword();
            }
        }
        
        // 操作符和分隔符
        return scanOperator();
    }
    
    // 获取所有tokens
    Vec<Token> tokenize() {
        Vec<Token> tokens;
        while (true) {
            Token token = nextToken();
            tokens.push_back(token);
            if (token.is(TokenType::EndOfFile)) {
                break;
            }
        }
        return tokens;
    }
    
private:
    String source_;
    size_t position_;
    int line_;
    int column_;
    
    bool isAtEnd() const {
        return position_ >= source_.length();
    }
    
    char peek() const {
        if (isAtEnd()) return '\0';
        return source_[position_];
    }
    
    char peekNext() const {
        if (position_ + 1 >= source_.length()) return '\0';
        return source_[position_ + 1];
    }
    
    char advance() {
        char c = source_[position_++];
        if (c == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        return c;
    }
    
    void skipWhitespace() {
        while (!isAtEnd() && std::isspace(peek())) {
            advance();
        }
    }
    
    Token scanComment() {
        int startLine = line_;
        int startColumn = column_;
        String value;
        
        advance(); // /
        advance(); // /
        
        while (!isAtEnd() && peek() != '\n') {
            value += advance();
        }
        
        return Token(TokenType::Comment, value, startLine, startColumn);
    }
    
    Token scanMultiLineComment() {
        int startLine = line_;
        int startColumn = column_;
        String value;
        
        advance(); // /
        advance(); // *
        
        while (!isAtEnd()) {
            if (peek() == '*' && peekNext() == '/') {
                advance(); // *
                advance(); // /
                break;
            }
            value += advance();
        }
        
        return Token(TokenType::Comment, value, startLine, startColumn);
    }
    
    Token scanGeneratorComment() {
        int startLine = line_;
        int startColumn = column_;
        String value;
        
        advance(); // #
        advance(); // space
        
        while (!isAtEnd() && peek() != '\n') {
            value += advance();
        }
        
        return Token(TokenType::GeneratorComment, value, startLine, startColumn);
    }
    
    Token scanString() {
        int startLine = line_;
        int startColumn = column_;
        char quote = advance(); // " or '
        String value;
        
        while (!isAtEnd() && peek() != quote) {
            if (peek() == '\\') {
                advance();
                if (!isAtEnd()) {
                    char escaped = advance();
                    switch (escaped) {
                        case 'n': value += '\n'; break;
                        case 't': value += '\t'; break;
                        case 'r': value += '\r'; break;
                        case '\\': value += '\\'; break;
                        case '"': value += '"'; break;
                        case '\'': value += '\''; break;
                        default: value += escaped;
                    }
                }
            } else {
                value += advance();
            }
        }
        
        if (!isAtEnd()) {
            advance(); // closing quote
        }
        
        return Token(TokenType::String, value, startLine, startColumn);
    }
    
    Token scanNumber() {
        int startLine = line_;
        int startColumn = column_;
        String value;
        
        while (!isAtEnd() && (std::isdigit(peek()) || peek() == '.')) {
            value += advance();
        }
        
        // 检查是否有单位（如px, em, %等）
        if (!isAtEnd() && std::isalpha(peek())) {
            while (!isAtEnd() && (std::isalnum(peek()) || peek() == '%')) {
                value += advance();
            }
        }
        
        return Token(TokenType::Number, value, startLine, startColumn);
    }
    
    Token scanIdentifier() {
        int startLine = line_;
        int startColumn = column_;
        String value;
        
        while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_' || peek() == '-')) {
            value += advance();
        }
        
        // 检查是否为关键字
        TokenType type = getKeywordType(value);
        return Token(type, value, startLine, startColumn);
    }
    
    Token scanAtIdentifier() {
        int startLine = line_;
        int startColumn = column_;
        String value;
        value += advance(); // @
        
        while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
            value += advance();
        }
        
        TokenType type = getAtKeywordType(value);
        return Token(type, value, startLine, startColumn);
    }
    
    Token scanBracketKeyword() {
        int startLine = line_;
        int startColumn = column_;
        String value;
        value += advance(); // [
        
        while (!isAtEnd() && peek() != ']') {
            value += advance();
        }
        
        if (!isAtEnd()) {
            value += advance(); // ]
        }
        
        TokenType type = getBracketKeywordType(value);
        return Token(type, value, startLine, startColumn);
    }
    
    Token scanOperator() {
        int startLine = line_;
        int startColumn = column_;
        char c = peek();
        
        switch (c) {
            case '{':
                if (peekNext() == '{') {
                    advance();
                    advance();
                    return Token(TokenType::DoubleLeftBrace, "{{", startLine, startColumn);
                }
                advance();
                return Token(TokenType::LeftBrace, "{", startLine, startColumn);
                
            case '}':
                if (peekNext() == '}') {
                    advance();
                    advance();
                    return Token(TokenType::DoubleRightBrace, "}}", startLine, startColumn);
                }
                advance();
                return Token(TokenType::RightBrace, "}", startLine, startColumn);
                
            case '[':
                advance();
                return Token(TokenType::LeftBracket, "[", startLine, startColumn);
                
            case ']':
                advance();
                return Token(TokenType::RightBracket, "]", startLine, startColumn);
                
            case '(':
                advance();
                return Token(TokenType::LeftParen, "(", startLine, startColumn);
                
            case ')':
                advance();
                return Token(TokenType::RightParen, ")", startLine, startColumn);
                
            case ':':
                advance();
                return Token(TokenType::Colon, ":", startLine, startColumn);
                
            case ';':
                advance();
                return Token(TokenType::Semicolon, ";", startLine, startColumn);
                
            case ',':
                advance();
                return Token(TokenType::Comma, ",", startLine, startColumn);
                
            case '.':
                advance();
                return Token(TokenType::Dot, ".", startLine, startColumn);
                
            case '=':
                advance();
                if (peek() == '=') {
                    advance();
                    return Token(TokenType::EqualEqual, "==", startLine, startColumn);
                }
                return Token(TokenType::Equal, "=", startLine, startColumn);
                
            case '+':
                advance();
                return Token(TokenType::Plus, "+", startLine, startColumn);
                
            case '-':
                advance();
                if (peek() == '>') {
                    advance();
                    return Token(TokenType::Arrow, "->", startLine, startColumn);
                }
                return Token(TokenType::Minus, "-", startLine, startColumn);
                
            case '*':
                advance();
                if (peek() == '*') {
                    advance();
                    return Token(TokenType::Power, "**", startLine, startColumn);
                }
                return Token(TokenType::Multiply, "*", startLine, startColumn);
                
            case '/':
                advance();
                return Token(TokenType::Divide, "/", startLine, startColumn);
                
            case '%':
                advance();
                return Token(TokenType::Modulo, "%", startLine, startColumn);
                
            case '<':
                advance();
                if (peek() == '=') {
                    advance();
                    return Token(TokenType::LessEqual, "<=", startLine, startColumn);
                }
                return Token(TokenType::LessThan, "<", startLine, startColumn);
                
            case '>':
                advance();
                if (peek() == '=') {
                    advance();
                    return Token(TokenType::GreaterEqual, ">=", startLine, startColumn);
                }
                return Token(TokenType::GreaterThan, ">", startLine, startColumn);
                
            case '!':
                advance();
                if (peek() == '=') {
                    advance();
                    return Token(TokenType::NotEqual, "!=", startLine, startColumn);
                }
                break;
                
            case '&':
                advance();
                if (peek() == '&') {
                    advance();
                    return Token(TokenType::And, "&&", startLine, startColumn);
                }
                if (peek() == '-' && position_ + 1 < source_.length() && source_[position_ + 1] == '>') {
                    advance(); // -
                    advance(); // >
                    return Token(TokenType::EventBind, "&->", startLine, startColumn);
                }
                return Token(TokenType::Ampersand, "&", startLine, startColumn);
                
            case '|':
                advance();
                if (peek() == '|') {
                    advance();
                    return Token(TokenType::Or, "||", startLine, startColumn);
                }
                break;
                
            case '?':
                advance();
                return Token(TokenType::Question, "?", startLine, startColumn);
                
            case '$':
                advance();
                return Token(TokenType::DollarSign, "$", startLine, startColumn);
                
            default:
                break;
        }
        
        advance();
        return Token(TokenType::Unknown, String(1, c), startLine, startColumn);
    }
    
    TokenType getKeywordType(const String& word) const {
        static const Map<String, TokenType> keywords = {
            {"text", TokenType::Text},
            {"style", TokenType::Style},
            {"script", TokenType::Script},
            {"from", TokenType::From},
            {"as", TokenType::As},
            {"use", TokenType::Use},
            {"inherit", TokenType::Inherit},
            {"delete", TokenType::Delete},
            {"insert", TokenType::Insert},
            {"after", TokenType::After},
            {"before", TokenType::Before},
            {"replace", TokenType::Replace},
            {"except", TokenType::Except},
            {"if", TokenType::If},
            {"else", TokenType::Else}
        };
        
        auto it = keywords.find(word);
        if (it != keywords.end()) {
            return it->second;
        }
        
        return TokenType::Identifier;
    }
    
    TokenType getAtKeywordType(const String& word) const {
        static const Map<String, TokenType> keywords = {
            {"@Style", TokenType::AtStyle},
            {"@Element", TokenType::AtElement},
            {"@Var", TokenType::AtVar},
            {"@Html", TokenType::AtHtml},
            {"@JavaScript", TokenType::AtJavaScript},
            {"@Chtl", TokenType::AtChtl},
            {"@CJmod", TokenType::AtCJmod},
            {"@Config", TokenType::AtConfig}
        };
        
        auto it = keywords.find(word);
        if (it != keywords.end()) {
            return it->second;
        }
        
        return TokenType::Identifier;
    }
    
    TokenType getBracketKeywordType(const String& word) const {
        static const Map<String, TokenType> keywords = {
            {"[Template]", TokenType::Template},
            {"[Custom]", TokenType::Custom},
            {"[Origin]", TokenType::Origin},
            {"[Import]", TokenType::Import},
            {"[Namespace]", TokenType::Namespace},
            {"[Configuration]", TokenType::Configuration},
            {"[Info]", TokenType::Info},
            {"[Export]", TokenType::Export}
        };
        
        auto it = keywords.find(word);
        if (it != keywords.end()) {
            return it->second;
        }
        
        return TokenType::Unknown;
    }
};

} // namespace CHTL
