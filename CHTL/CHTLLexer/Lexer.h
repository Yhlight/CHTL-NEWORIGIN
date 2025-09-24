#pragma once

#include "Token.h"
#include "GlobalMap.h"
#include "../../Scanner/StateMachine.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Lexer states
enum class LexerState {
    INIT,
    SCANNING,
    IDENTIFIER,
    STRING_LITERAL,
    NUMERIC_LITERAL,
    OPERATOR,
    COMMENT,
    WHITESPACE,
    DELIMITER,
    ERROR,
    COMPLETE
};

// Lexer strategy interface
class LexerStrategy {
public:
    virtual ~LexerStrategy() = default;
    virtual bool canHandle(char current, char next, const std::string& input, size_t position) const = 0;
    virtual Token process(char& current, size_t& position, const std::string& input) = 0;
    virtual std::string getName() const = 0;
};

// Identifier lexer strategy
class IdentifierLexerStrategy : public LexerStrategy {
public:
    bool canHandle(char current, char next, const std::string& input, size_t position) const override;
    Token process(char& current, size_t& position, const std::string& input) override;
    std::string getName() const override { return "IdentifierLexerStrategy"; }
    
private:
    std::string readIdentifier(const std::string& input, size_t& position);
    TokenType determineIdentifierType(const std::string& identifier);
};

// String literal lexer strategy
class StringLiteralLexerStrategy : public LexerStrategy {
public:
    bool canHandle(char current, char next, const std::string& input, size_t position) const override;
    Token process(char& current, size_t& position, const std::string& input) override;
    std::string getName() const override { return "StringLiteralLexerStrategy"; }
    
private:
    std::string readStringLiteral(const std::string& input, size_t& position, char quote);
    std::string readRawString(const std::string& input, size_t& position);
    std::string readTemplateString(const std::string& input, size_t& position);
};

// Numeric literal lexer strategy
class NumericLiteralLexerStrategy : public LexerStrategy {
public:
    bool canHandle(char current, char next, const std::string& input, size_t position) const override;
    Token process(char& current, size_t& position, const std::string& input) override;
    std::string getName() const override { return "NumericLiteralLexerStrategy"; }
    
private:
    std::string readNumericLiteral(const std::string& input, size_t& position);
    std::string readInteger(const std::string& input, size_t& position);
    std::string readFloat(const std::string& input, size_t& position);
    std::string readHexNumber(const std::string& input, size_t& position);
    std::string readBinaryNumber(const std::string& input, size_t& position);
};

// Operator lexer strategy
class OperatorLexerStrategy : public LexerStrategy {
public:
    bool canHandle(char current, char next, const std::string& input, size_t position) const override;
    Token process(char& current, size_t& position, const std::string& input) override;
    std::string getName() const override { return "OperatorLexerStrategy"; }
    
private:
    std::string readOperator(const std::string& input, size_t& position);
    TokenType getOperatorType(const std::string& op);
    bool isMultiCharOperator(const std::string& input, size_t position, const std::string& op);
};

// Comment lexer strategy
class CommentLexerStrategy : public LexerStrategy {
public:
    bool canHandle(char current, char next, const std::string& input, size_t position) const override;
    Token process(char& current, size_t& position, const std::string& input) override;
    std::string getName() const override { return "CommentLexerStrategy"; }
    
private:
    std::string readLineComment(const std::string& input, size_t& position);
    std::string readBlockComment(const std::string& input, size_t& position);
    std::string readGeneratorComment(const std::string& input, size_t& position);
    TokenType determineCommentType(const std::string& input, size_t position);
};

// Whitespace lexer strategy
class WhitespaceLexerStrategy : public LexerStrategy {
public:
    bool canHandle(char current, char next, const std::string& input, size_t position) const override;
    Token process(char& current, size_t& position, const std::string& input) override;
    std::string getName() const override { return "WhitespaceLexerStrategy"; }
    
private:
    std::string readWhitespace(const std::string& input, size_t& position);
};

// Delimiter lexer strategy
class DelimiterLexerStrategy : public LexerStrategy {
public:
    bool canHandle(char current, char next, const std::string& input, size_t position) const override;
    Token process(char& current, size_t& position, const std::string& input) override;
    std::string getName() const override { return "DelimiterLexerStrategy"; }
    
private:
    TokenType getDelimiterType(char delimiter);
};

// Special symbol lexer strategy (for CHTL-specific symbols)
class SpecialSymbolLexerStrategy : public LexerStrategy {
public:
    bool canHandle(char current, char next, const std::string& input, size_t position) const override;
    Token process(char& current, size_t& position, const std::string& input) override;
    std::string getName() const override { return "SpecialSymbolLexerStrategy"; }
    
private:
    std::string readSpecialSymbol(const std::string& input, size_t& position);
    TokenType getSpecialSymbolType(const std::string& symbol);
};

// CHTL JS specific lexer strategy
class CHTLJSLexerStrategy : public LexerStrategy {
public:
    bool canHandle(char current, char next, const std::string& input, size_t position) const override;
    Token process(char& current, size_t& position, const std::string& input) override;
    std::string getName() const override { return "CHTLJSLexerStrategy"; }
    
private:
    std::string readDoubleBrace(const std::string& input, size_t& position);
    std::string readArrow(const std::string& input, size_t& position);
    std::string readEventBinding(const std::string& input, size_t& position);
    std::string readVirtualObject(const std::string& input, size_t& position);
};

// Lexer state handlers
class LexerInitHandler : public StateHandler<LexerState> {
public:
    void onEnter(StateContext<LexerState>& context) override;
    void onUpdate(StateContext<LexerState>& context) override;
};

class LexerScanningHandler : public StateHandler<LexerState> {
public:
    void onEnter(StateContext<LexerState>& context) override;
    void onUpdate(StateContext<LexerState>& context) override;
};

class LexerErrorHandler : public StateHandler<LexerState> {
public:
    void onEnter(StateContext<LexerState>& context) override;
    void onUpdate(StateContext<LexerState>& context) override;
};

class LexerCompleteHandler : public StateHandler<LexerState> {
public:
    void onEnter(StateContext<LexerState>& context) override;
    void onUpdate(StateContext<LexerState>& context) override;
};

// Main Lexer class
class Lexer {
public:
    Lexer();
    ~Lexer() = default;
    
    // Main tokenization method
    std::vector<Token> tokenize(const std::string& input);
    TokenStream tokenizeToStream(const std::string& input);
    
    // Configuration
    void setSkipWhitespace(bool skip) { skipWhitespace_ = skip; }
    void setSkipComments(bool skip) { skipComments_ = skip; }
    void setEnableCHTLJSMode(bool enable) { enableCHTLJSMode_ = enable; }
    void setEnableUnquotedLiterals(bool enable) { enableUnquotedLiterals_ = enable; }
    
    // Strategy management
    void registerStrategy(std::unique_ptr<LexerStrategy> strategy);
    void setDefaultStrategy(const std::string& name);
    
    // Error handling
    bool hasError() const { return !error_.empty(); }
    std::string getError() const { return error_; }
    void clearError() { error_.clear(); }
    
    // Position tracking
    size_t getCurrentLine() const { return currentLine_; }
    size_t getCurrentColumn() const { return currentColumn_; }
    
    // Debug and introspection
    void printTokens(const std::vector<Token>& tokens) const;
    std::string getDebugInfo() const;
    
private:
    StateMachine<LexerState> stateMachine_;
    std::map<std::string, std::unique_ptr<LexerStrategy>> strategies_;
    GlobalMap& globalMap_;
    SymbolResolver symbolResolver_;
    
    // Configuration
    bool skipWhitespace_;
    bool skipComments_;
    bool enableCHTLJSMode_;
    bool enableUnquotedLiterals_;
    
    // Processing state
    std::string input_;
    std::vector<Token> tokens_;
    size_t position_;
    size_t currentLine_;
    size_t currentColumn_;
    std::string error_;
    
    // Strategy instances
    std::unique_ptr<IdentifierLexerStrategy> identifierStrategy_;
    std::unique_ptr<StringLiteralLexerStrategy> stringLiteralStrategy_;
    std::unique_ptr<NumericLiteralLexerStrategy> numericLiteralStrategy_;
    std::unique_ptr<OperatorLexerStrategy> operatorStrategy_;
    std::unique_ptr<CommentLexerStrategy> commentStrategy_;
    std::unique_ptr<WhitespaceLexerStrategy> whitespaceStrategy_;
    std::unique_ptr<DelimiterLexerStrategy> delimiterStrategy_;
    std::unique_ptr<SpecialSymbolLexerStrategy> specialSymbolStrategy_;
    std::unique_ptr<CHTLJSLexerStrategy> chtljsStrategy_;
    
    void initializeStrategies();
    void initializeStateMachine();
    bool processInput();
    void advancePosition();
    void resetPosition();
    
    LexerStrategy* selectStrategy(char current, char next);
    void addToken(const Token& token);
    void handleError(const std::string& message);
};

} // namespace CHTL