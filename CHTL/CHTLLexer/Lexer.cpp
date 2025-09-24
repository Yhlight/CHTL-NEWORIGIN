#include "Lexer.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// IdentifierLexerStrategy implementation
bool IdentifierLexerStrategy::canHandle(char current, char next, const std::string& input, size_t position) const {
    return Token::isIdentifierStart(current);
}

Token IdentifierLexerStrategy::process(char& current, size_t& position, const std::string& input) {
    std::string identifier = readIdentifier(input, position);
    TokenType type = determineIdentifierType(identifier);
    
    return TokenFactory::createIdentifier(identifier, 1, position); // Line tracking would be improved
}

std::string IdentifierLexerStrategy::readIdentifier(const std::string& input, size_t& position) {
    std::string identifier;
    identifier += input[position++];
    
    while (position < input.length() && Token::isIdentifierPart(input[position])) {
        identifier += input[position++];
    }
    
    return identifier;
}

TokenType IdentifierLexerStrategy::determineIdentifierType(const std::string& identifier) {
    GlobalMap& globalMap = GlobalMap::getInstance();
    return globalMap.getTokenType(identifier);
}

// StringLiteralLexerStrategy implementation
bool StringLiteralLexerStrategy::canHandle(char current, char next, const std::string& input, size_t position) const {
    return current == '"' || current == '\'' || current == '`';
}

Token StringLiteralLexerStrategy::process(char& current, size_t& position, const std::string& input) {
    char quote = current;
    position++; // Skip opening quote
    
    std::string literal;
    if (quote == '`') {
        literal = readTemplateString(input, position);
    } else {
        literal = readStringLiteral(input, position, quote);
    }
    
    return TokenFactory::createStringLiteral(literal, 1, position);
}

std::string StringLiteralLexerStrategy::readStringLiteral(const std::string& input, size_t& position, char quote) {
    std::string literal;
    bool escaped = false;
    
    while (position < input.length()) {
        char c = input[position++];
        
        if (escaped) {
            switch (c) {
                case 'n': literal += '\n'; break;
                case 't': literal += '\t'; break;
                case 'r': literal += '\r'; break;
                case '\\': literal += '\\'; break;
                case '"': literal += '"'; break;
                case '\'': literal += '\''; break;
                default: literal += c; break;
            }
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
        } else if (c == quote) {
            break;
        } else {
            literal += c;
        }
    }
    
    return literal;
}

std::string StringLiteralLexerStrategy::readTemplateString(const std::string& input, size_t& position) {
    std::string literal;
    bool escaped = false;
    
    while (position < input.length()) {
        char c = input[position++];
        
        if (escaped) {
            literal += c;
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
        } else if (c == '`') {
            break;
        } else {
            literal += c;
        }
    }
    
    return literal;
}

// NumericLiteralLexerStrategy implementation
bool NumericLiteralLexerStrategy::canHandle(char current, char next, const std::string& input, size_t position) const {
    return std::isdigit(current) || 
           (current == '.' && position + 1 < input.length() && std::isdigit(next));
}

Token NumericLiteralLexerStrategy::process(char& current, size_t& position, const std::string& input) {
    std::string literal = readNumericLiteral(input, position);
    return TokenFactory::createNumericLiteral(literal, 1, position);
}

std::string NumericLiteralLexerStrategy::readNumericLiteral(const std::string& input, size_t& position) {
    if (input[position] == '0' && position + 1 < input.length()) {
        char next = input[position + 1];
        if (next == 'x' || next == 'X') {
            return readHexNumber(input, position);
        } else if (next == 'b' || next == 'B') {
            return readBinaryNumber(input, position);
        }
    }
    
    if (input[position] == '.') {
        return readFloat(input, position);
    }
    
    std::string literal = readInteger(input, position);
    
    // Check for decimal part
    if (position < input.length() && input[position] == '.') {
        position++;
        if (position < input.length() && std::isdigit(input[position])) {
            literal += "." + readInteger(input, position);
        } else {
            position--; // Put back the dot
        }
    }
    
    // Check for exponent
    if (position < input.length() && (input[position] == 'e' || input[position] == 'E')) {
        literal += input[position++];
        if (position < input.length() && (input[position] == '+' || input[position] == '-')) {
            literal += input[position++];
        }
        if (position < input.length() && std::isdigit(input[position])) {
            literal += readInteger(input, position);
        }
    }
    
    return literal;
}

std::string NumericLiteralLexerStrategy::readInteger(const std::string& input, size_t& position) {
    std::string literal;
    
    while (position < input.length() && std::isdigit(input[position])) {
        literal += input[position++];
    }
    
    return literal;
}

std::string NumericLiteralLexerStrategy::readFloat(const std::string& input, size_t& position) {
    std::string literal = ".";
    position++;
    
    while (position < input.length() && std::isdigit(input[position])) {
        literal += input[position++];
    }
    
    return literal;
}

std::string NumericLiteralLexerStrategy::readHexNumber(const std::string& input, size_t& position) {
    std::string literal = "0x";
    position += 2;
    
    while (position < input.length() && Token::isHexDigit(input[position])) {
        literal += input[position++];
    }
    
    return literal;
}

std::string NumericLiteralLexerStrategy::readBinaryNumber(const std::string& input, size_t& position) {
    std::string literal = "0b";
    position += 2;
    
    while (position < input.length() && (input[position] == '0' || input[position] == '1')) {
        literal += input[position++];
    }
    
    return literal;
}

// OperatorLexerStrategy implementation
bool OperatorLexerStrategy::canHandle(char current, char next, const std::string& input, size_t position) const {
    return Token::isOperatorChar(current);
}

Token OperatorLexerStrategy::process(char& current, size_t& position, const std::string& input) {
    std::string op = readOperator(input, position);
    TokenType type = getOperatorType(op);
    
    return TokenFactory::createOperator(type, op, 1, position);
}

std::string OperatorLexerStrategy::readOperator(const std::string& input, size_t& position) {
    std::string op;
    op += input[position++];
    
    // Check for multi-character operators
    if (position < input.length()) {
        std::string twoChar = op + input[position];
        if (isMultiCharOperator(input, position - 1, twoChar)) {
            op += input[position++];
            
            // Check for three-character operators
            if (position < input.length()) {
                std::string threeChar = op + input[position];
                if (isMultiCharOperator(input, position - 2, threeChar)) {
                    op += input[position++];
                }
            }
        }
    }
    
    return op;
}

TokenType OperatorLexerStrategy::getOperatorType(const std::string& op) {
    GlobalMap& globalMap = GlobalMap::getInstance();
    return globalMap.getTokenType(op);
}

bool OperatorLexerStrategy::isMultiCharOperator(const std::string& input, size_t position, const std::string& op) {
    // Check for valid multi-character operators
    static const std::vector<std::string> multiCharOps = {
        "==", "!=", "<=", ">=", "&&", "||", "**", "->", "&->"
    };
    
    return std::find(multiCharOps.begin(), multiCharOps.end(), op) != multiCharOps.end();
}

// CommentLexerStrategy implementation
bool CommentLexerStrategy::canHandle(char current, char next, const std::string& input, size_t position) const {
    return (current == '/' && next == '/') ||
           (current == '/' && next == '*') ||
           (current == '#' && next == ' ');
}

Token CommentLexerStrategy::process(char& current, size_t& position, const std::string& input) {
    TokenType type = determineCommentType(input, position);
    std::string comment;
    
    switch (type) {
        case TokenType::LINE_COMMENT:
            comment = readLineComment(input, position);
            break;
        case TokenType::BLOCK_COMMENT:
            comment = readBlockComment(input, position);
            break;
        case TokenType::GENERATOR_COMMENT:
            comment = readGeneratorComment(input, position);
            break;
        default:
            comment = "";
            break;
    }
    
    return TokenFactory::createComment(type, comment, 1, position);
}

std::string CommentLexerStrategy::readLineComment(const std::string& input, size_t& position) {
    std::string comment;
    position += 2; // Skip "//"
    
    while (position < input.length() && input[position] != '\n') {
        comment += input[position++];
    }
    
    return comment;
}

std::string CommentLexerStrategy::readBlockComment(const std::string& input, size_t& position) {
    std::string comment;
    position += 2; // Skip "/*"
    
    while (position + 1 < input.length()) {
        if (input[position] == '*' && input[position + 1] == '/') {
            position += 2;
            break;
        }
        comment += input[position++];
    }
    
    return comment;
}

std::string CommentLexerStrategy::readGeneratorComment(const std::string& input, size_t& position) {
    std::string comment;
    position += 2; // Skip "# "
    
    while (position < input.length() && input[position] != '\n') {
        comment += input[position++];
    }
    
    return comment;
}

TokenType CommentLexerStrategy::determineCommentType(const std::string& input, size_t position) {
    if (position + 1 < input.length()) {
        if (input[position] == '/' && input[position + 1] == '/') {
            return TokenType::LINE_COMMENT;
        } else if (input[position] == '/' && input[position + 1] == '*') {
            return TokenType::BLOCK_COMMENT;
        } else if (input[position] == '#' && input[position + 1] == ' ') {
            return TokenType::GENERATOR_COMMENT;
        }
    }
    
    return TokenType::LINE_COMMENT; // Default
}

// WhitespaceLexerStrategy implementation
bool WhitespaceLexerStrategy::canHandle(char current, char next, const std::string& input, size_t position) const {
    return Token::isWhitespace(current) || Token::isNewline(current);
}

Token WhitespaceLexerStrategy::process(char& current, size_t& position, const std::string& input) {
    std::string whitespace = readWhitespace(input, position);
    return TokenFactory::createWhitespace(whitespace, 1, position);
}

std::string WhitespaceLexerStrategy::readWhitespace(const std::string& input, size_t& position) {
    std::string whitespace;
    
    while (position < input.length() && 
           (Token::isWhitespace(input[position]) || Token::isNewline(input[position]))) {
        whitespace += input[position++];
    }
    
    return whitespace;
}

// DelimiterLexerStrategy implementation
bool DelimiterLexerStrategy::canHandle(char current, char next, const std::string& input, size_t position) const {
    return Token::isDelimiterChar(current) && 
           (current == '{' || current == '}' || current == '(' || current == ')' || 
            current == '[' || current == ']' || current == '<' || current == '>');
}

Token DelimiterLexerStrategy::process(char& current, size_t& position, const std::string& input) {
    TokenType type = getDelimiterType(current);
    position++;
    return TokenFactory::createDelimiter(type, 1, position);
}

TokenType DelimiterLexerStrategy::getDelimiterType(char delimiter) {
    switch (delimiter) {
        case '{': return TokenType::BRACE_OPEN;
        case '}': return TokenType::BRACE_CLOSE;
        case '(': return TokenType::PAREN_OPEN;
        case ')': return TokenType::PAREN_CLOSE;
        case '[': return TokenType::BRACKET_OPEN;
        case ']': return TokenType::BRACKET_CLOSE;
        case '<': return TokenType::ANGLE_OPEN;
        case '>': return TokenType::ANGLE_CLOSE;
        default: return TokenType::BRACE_OPEN;
    }
}

// SpecialSymbolLexerStrategy implementation
bool SpecialSymbolLexerStrategy::canHandle(char current, char next, const std::string& input, size_t position) const {
    return current == '@' || current == '#' || current == '$' || current == '&' || current == '%';
}

Token SpecialSymbolLexerStrategy::process(char& current, size_t& position, const std::string& input) {
    std::string symbol = readSpecialSymbol(input, position);
    TokenType type = getSpecialSymbolType(symbol);
    
    return TokenFactory::createOperator(type, symbol, 1, position);
}

std::string SpecialSymbolLexerStrategy::readSpecialSymbol(const std::string& input, size_t& position) {
    std::string symbol;
    symbol += input[position++];
    
    // For @ symbols, read the following identifier
    if (symbol == "@" && position < input.length() && Token::isIdentifierStart(input[position])) {
        while (position < input.length() && Token::isIdentifierPart(input[position])) {
            symbol += input[position++];
        }
    }
    
    return symbol;
}

TokenType SpecialSymbolLexerStrategy::getSpecialSymbolType(const std::string& symbol) {
    GlobalMap& globalMap = GlobalMap::getInstance();
    return globalMap.getTokenType(symbol);
}

// CHTLJSLexerStrategy implementation
bool CHTLJSLexerStrategy::canHandle(char current, char next, const std::string& input, size_t position) const {
    return (current == '{' && next == '{') ||
           (current == '}' && next == '}') ||
           (current == '-' && next == '>') ||
           (current == '&' && next == '-' && position + 2 < input.length() && input[position + 2] == '>');
}

Token CHTLJSLexerStrategy::process(char& current, size_t& position, const std::string& input) {
    std::string symbol;
    
    if (current == '{' && position + 1 < input.length() && input[position + 1] == '{') {
        symbol = readDoubleBrace(input, position);
        return TokenFactory::createOperator(TokenType::CJMOD_DOUBLE_BRACE_OPEN, symbol, 1, position);
    } else if (current == '}' && position + 1 < input.length() && input[position + 1] == '}') {
        symbol = readDoubleBrace(input, position);
        return TokenFactory::createOperator(TokenType::CJMOD_DOUBLE_BRACE_CLOSE, symbol, 1, position);
    } else if (current == '-' && position + 1 < input.length() && input[position + 1] == '>') {
        symbol = readArrow(input, position);
        return TokenFactory::createOperator(TokenType::OPERATOR_ARROW, symbol, 1, position);
    } else if (current == '&' && position + 2 < input.length() && 
               input[position + 1] == '-' && input[position + 2] == '>') {
        symbol = readEventBinding(input, position);
        return TokenFactory::createOperator(TokenType::CJMOD_DELEGATE, symbol, 1, position);
    }
    
    return TokenFactory::createError("Unknown CHTL JS symbol", 1, position);
}

std::string CHTLJSLexerStrategy::readDoubleBrace(const std::string& input, size_t& position) {
    std::string symbol = "{{";
    position += 2;
    return symbol;
}

std::string CHTLJSLexerStrategy::readArrow(const std::string& input, size_t& position) {
    std::string symbol = "->";
    position += 2;
    return symbol;
}

std::string CHTLJSLexerStrategy::readEventBinding(const std::string& input, size_t& position) {
    std::string symbol = "&->";
    position += 3;
    return symbol;
}

// Main Lexer implementation
Lexer::Lexer() 
    : stateMachine_(LexerState::INIT)
    , globalMap_(GlobalMap::getInstance())
    , symbolResolver_(globalMap_)
    , skipWhitespace_(true)
    , skipComments_(true)
    , enableCHTLJSMode_(false)
    , enableUnquotedLiterals_(true)
    , position_(0)
    , currentLine_(1)
    , currentColumn_(1) {
    
    initializeStrategies();
    initializeStateMachine();
}

void Lexer::initializeStrategies() {
    // Create strategy instances
    identifierStrategy_ = std::make_unique<IdentifierLexerStrategy>();
    stringLiteralStrategy_ = std::make_unique<StringLiteralLexerStrategy>();
    numericLiteralStrategy_ = std::make_unique<NumericLiteralLexerStrategy>();
    operatorStrategy_ = std::make_unique<OperatorLexerStrategy>();
    commentStrategy_ = std::make_unique<CommentLexerStrategy>();
    whitespaceStrategy_ = std::make_unique<WhitespaceLexerStrategy>();
    delimiterStrategy_ = std::make_unique<DelimiterLexerStrategy>();
    specialSymbolStrategy_ = std::make_unique<SpecialSymbolLexerStrategy>();
    chtljsStrategy_ = std::make_unique<CHTLJSLexerStrategy>();
    
    // Register strategies
    registerStrategy(std::make_unique<IdentifierLexerStrategy>());
    registerStrategy(std::make_unique<StringLiteralLexerStrategy>());
    registerStrategy(std::make_unique<NumericLiteralLexerStrategy>());
    registerStrategy(std::make_unique<OperatorLexerStrategy>());
    registerStrategy(std::make_unique<CommentLexerStrategy>());
    registerStrategy(std::make_unique<WhitespaceLexerStrategy>());
    registerStrategy(std::make_unique<DelimiterLexerStrategy>());
    registerStrategy(std::make_unique<SpecialSymbolLexerStrategy>());
    registerStrategy(std::make_unique<CHTLJSLexerStrategy>());
}

void Lexer::initializeStateMachine() {
    // Add state handlers
    stateMachine_.setHandler(LexerState::INIT, std::make_unique<LexerInitHandler>());
    stateMachine_.setHandler(LexerState::SCANNING, std::make_unique<LexerScanningHandler>());
    stateMachine_.setHandler(LexerState::ERROR, std::make_unique<LexerErrorHandler>());
    stateMachine_.setHandler(LexerState::COMPLETE, std::make_unique<LexerCompleteHandler>());
    
    // Add transitions
    stateMachine_.addTransition(LexerState::INIT, LexerState::SCANNING, "input_ready");
    stateMachine_.addTransition(LexerState::SCANNING, LexerState::COMPLETE, "scanning_complete");
    stateMachine_.addTransition(LexerState::SCANNING, LexerState::ERROR, "error");
    stateMachine_.addTransition(LexerState::ERROR, LexerState::INIT, "reset");
}

std::vector<Token> Lexer::tokenize(const std::string& input) {
    input_ = input;
    tokens_.clear();
    resetPosition();
    clearError();
    
    if (!processInput()) {
        return {};
    }
    
    return tokens_;
}

TokenStream Lexer::tokenizeToStream(const std::string& input) {
    auto tokens = tokenize(input);
    return TokenStream(std::move(tokens));
}

void Lexer::registerStrategy(std::unique_ptr<LexerStrategy> strategy) {
    if (strategy) {
        strategies_[strategy->getName()] = std::move(strategy);
    }
}

void Lexer::setDefaultStrategy(const std::string& name) {
    // Implementation for setting default strategy
}

void Lexer::printTokens(const std::vector<Token>& tokens) const {
    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }
}

std::string Lexer::getDebugInfo() const {
    std::ostringstream oss;
    oss << "Lexer Debug Info:\n";
    oss << "Strategies: " << strategies_.size() << "\n";
    oss << "Current Position: " << position_ << "\n";
    oss << "Current Line: " << currentLine_ << "\n";
    oss << "Current Column: " << currentColumn_ << "\n";
    oss << "Tokens Generated: " << tokens_.size() << "\n";
    return oss.str();
}

bool Lexer::processInput() {
    try {
        stateMachine_.trigger("input_ready");
        
        while (position_ < input_.length()) {
            char current = input_[position_];
            char next = (position_ + 1 < input_.length()) ? input_[position_ + 1] : '\0';
            
            LexerStrategy* strategy = selectStrategy(current, next);
            if (strategy) {
                Token token = strategy->process(current, position_);
                
                // Filter tokens based on configuration
                if (skipWhitespace_ && token.isWhitespace()) {
                    continue;
                }
                if (skipComments_ && token.isComment()) {
                    continue;
                }
                
                addToken(token);
            } else {
                handleError("No strategy found for character: " + std::string(1, current));
                return false;
            }
        }
        
        // Add EOF token
        addToken(TokenFactory::createEOF(currentLine_, currentColumn_));
        
        stateMachine_.trigger("scanning_complete");
        return true;
    } catch (const std::exception& e) {
        handleError(e.what());
        return false;
    }
}

void Lexer::advancePosition() {
    if (position_ < input_.length()) {
        if (input_[position_] == '\n') {
            currentLine_++;
            currentColumn_ = 1;
        } else {
            currentColumn_++;
        }
        position_++;
    }
}

void Lexer::resetPosition() {
    position_ = 0;
    currentLine_ = 1;
    currentColumn_ = 1;
}

LexerStrategy* Lexer::selectStrategy(char current, char next) {
    // Priority order for strategy selection
    std::vector<std::string> strategyOrder = {
        "CHTLJSLexerStrategy",
        "CommentLexerStrategy",
        "StringLiteralLexerStrategy",
        "NumericLiteralLexerStrategy",
        "IdentifierLexerStrategy",
        "OperatorLexerStrategy",
        "SpecialSymbolLexerStrategy",
        "DelimiterLexerStrategy",
        "WhitespaceLexerStrategy"
    };
    
    if (!enableCHTLJSMode_) {
        // Remove CHTL JS strategy if not enabled
        strategyOrder.erase(std::remove(strategyOrder.begin(), strategyOrder.end(), "CHTLJSLexerStrategy"), 
                          strategyOrder.end());
    }
    
    for (const auto& strategyName : strategyOrder) {
        auto it = strategies_.find(strategyName);
        if (it != strategies_.end() && it->second->canHandle(current, next, input_, position_)) {
            return it->second.get();
        }
    }
    
    return nullptr;
}

void Lexer::addToken(const Token& token) {
    tokens_.push_back(token);
}

void Lexer::handleError(const std::string& message) {
    error_ = message;
    stateMachine_.trigger("error");
}

// State Handler implementations
void LexerInitHandler::onEnter(StateContext<LexerState>& context) {
    context.setData("status", "initialized");
}

void LexerInitHandler::onUpdate(StateContext<LexerState>& context) {
    // Initialize lexer state
}

void LexerScanningHandler::onEnter(StateContext<LexerState>& context) {
    context.setData("status", "scanning");
}

void LexerScanningHandler::onUpdate(StateContext<LexerState>& context) {
    // Perform scanning
}

void LexerErrorHandler::onEnter(StateContext<LexerState>& context) {
    context.setData("status", "error");
}

void LexerErrorHandler::onUpdate(StateContext<LexerState>& context) {
    // Handle error state
}

void LexerCompleteHandler::onEnter(StateContext<LexerState>& context) {
    context.setData("status", "complete");
}

void LexerCompleteHandler::onUpdate(StateContext<LexerState>& context) {
    // Handle completion
}

} // namespace CHTL