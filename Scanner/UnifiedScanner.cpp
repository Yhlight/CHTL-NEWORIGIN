#include "UnifiedScanner.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_set>

namespace CHTL {
namespace Scanner {

// 静态成员初始化
std::unordered_set<std::string> UnifiedScanner::chtlKeywords_;
std::unordered_set<std::string> UnifiedScanner::chtljsKeywords_;
std::unordered_set<std::string> UnifiedScanner::cssKeywords_;
std::unordered_set<std::string> UnifiedScanner::jsKeywords_;
std::unordered_set<std::string> UnifiedScanner::htmlTags_;
bool UnifiedScanner::keywordsInitialized_ = false;

UnifiedScanner::UnifiedScanner() 
    : enablePlaceholders_(true), enableFragmentSeparation_(true), 
      enableSyntaxBoundaryDetection_(true), position_(0), line_(1), 
      column_(1), placeholderCounter_(0) {
    if (!keywordsInitialized_) {
        initializeKeywords();
        keywordsInitialized_ = true;
    }
}

UnifiedScanner::~UnifiedScanner() = default;

ScanResult UnifiedScanner::scan(const std::string& input) {
    std::cerr << "DEBUG: scan - input length: " << input.length() << std::endl;
    setInput(input);
    ScanResult result;
    
    std::cerr << "DEBUG: scan - calling scanCode" << std::endl;
    scanCode();
    std::cerr << "DEBUG: scan - calling processFragments" << std::endl;
    processFragments();
    
    result.fragments = fragments_;
    result.placeholders = placeholders_;
    result.success = true;
    
    // 生成处理后的代码
    std::ostringstream oss;
    for (const auto& fragment : fragments_) {
        oss << fragment.content;
    }
    result.processedCode = oss.str();
    
    return result;
}

ScanResult UnifiedScanner::scanFile(const std::string& filename) {
    // 这里需要实现文件读取，暂时返回空结果
    ScanResult result;
    result.success = false;
    result.errorMessage = "File reading not implemented yet";
    return result;
}

void UnifiedScanner::setEnablePlaceholders(bool enable) {
    enablePlaceholders_ = enable;
}

void UnifiedScanner::setEnableFragmentSeparation(bool enable) {
    enableFragmentSeparation_ = enable;
}

void UnifiedScanner::setEnableSyntaxBoundaryDetection(bool enable) {
    enableSyntaxBoundaryDetection_ = enable;
}

bool UnifiedScanner::isEnablePlaceholders() const {
    return enablePlaceholders_;
}

bool UnifiedScanner::isEnableFragmentSeparation() const {
    return enableFragmentSeparation_;
}

bool UnifiedScanner::isEnableSyntaxBoundaryDetection() const {
    return enableSyntaxBoundaryDetection_;
}

const std::vector<UnifiedScanner::ScannerError>& UnifiedScanner::getErrors() const {
    return errors_;
}

bool UnifiedScanner::hasErrors() const {
    return !errors_.empty();
}

void UnifiedScanner::clearErrors() {
    errors_.clear();
}

void UnifiedScanner::addPlaceholder(const std::string& placeholder, 
                                   const std::string& content, 
                                   FragmentType type, size_t line, size_t column) {
    placeholders_.emplace_back(placeholder, content, type, line, column);
}

void UnifiedScanner::removePlaceholder(const std::string& placeholder) {
    placeholders_.erase(
        std::remove_if(placeholders_.begin(), placeholders_.end(),
                      [&placeholder](const PlaceholderInfo& info) {
                          return info.placeholder == placeholder;
                      }),
        placeholders_.end()
    );
}

void UnifiedScanner::clearPlaceholders() {
    placeholders_.clear();
}

void UnifiedScanner::addFragment(const CodeFragment& fragment) {
    fragments_.push_back(fragment);
}

void UnifiedScanner::removeFragment(size_t index) {
    if (index < fragments_.size()) {
        fragments_.erase(fragments_.begin() + index);
    }
}

void UnifiedScanner::clearFragments() {
    fragments_.clear();
}

std::vector<CodeFragment> UnifiedScanner::getFragmentsByType(FragmentType type) const {
    std::vector<CodeFragment> result;
    for (const auto& fragment : fragments_) {
        if (fragment.type == type) {
            result.push_back(fragment);
        }
    }
    return result;
}

std::string UnifiedScanner::mergeFragments(const std::vector<FragmentType>& types) const {
    std::ostringstream oss;
    for (const auto& fragment : fragments_) {
        if (std::find(types.begin(), types.end(), fragment.type) != types.end()) {
            oss << fragment.content;
        }
    }
    return oss.str();
}

std::string UnifiedScanner::restorePlaceholders(const std::string& processedCode) const {
    std::string result = processedCode;
    for (const auto& placeholder : placeholders_) {
        // 替换占位符为原始内容
        size_t pos = 0;
        while ((pos = result.find(placeholder.placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.placeholder.length(), placeholder.originalContent);
            pos += placeholder.originalContent.length();
        }
    }
    return result;
}

void UnifiedScanner::scanCode() {
    std::cout << "DEBUG: scanCode - input length: " << input_.length() << std::endl;
    std::cout << "DEBUG: scanCode - enableSyntaxBoundaryDetection_: " << enableSyntaxBoundaryDetection_ << std::endl;
    
    while (position_ < input_.length()) {
        std::cout << "DEBUG: scanCode - before skipWhitespace, position: " << position_ << std::endl;
        
        // 跳过空白字符
        skipWhitespace();
        
        std::cout << "DEBUG: scanCode - after skipWhitespace, position: " << position_ << std::endl;
        
        if (position_ >= input_.length()) {
            std::cout << "DEBUG: scanCode - reached end of input" << std::endl;
            break;
        }
        
        std::cout << "DEBUG: scanCode - position: " << position_ << ", char: '" << currentChar() << "'" << std::endl;
        
        // 检测语法边界
        if (enableSyntaxBoundaryDetection_) {
            if (detectCHTLBoundary()) {
                std::cout << "DEBUG: scanCode - detected CHTL boundary" << std::endl;
                scanCHTLBlock();
            } else if (detectCHTLJSBoundary()) {
                std::cout << "DEBUG: scanCode - detected CHTL JS boundary" << std::endl;
                scanCHTLJSBlock();
            } else if (detectCSSBoundary()) {
                std::cout << "DEBUG: scanCode - detected CSS boundary" << std::endl;
                scanCSSBlock();
            } else if (detectJSBoundary()) {
                std::cout << "DEBUG: scanCode - detected JS boundary" << std::endl;
                scanJSBlock();
            } else if (detectHTMLBoundary()) {
                std::cout << "DEBUG: scanCode - detected HTML boundary" << std::endl;
                scanHTMLBlock();
            } else {
                // 默认作为CHTL处理
                std::cout << "DEBUG: scanCode - default to CHTL" << std::endl;
                scanCHTLBlock();
            }
        } else {
            // 简单扫描，按行处理
            std::cout << "DEBUG: scanCode - simple scan" << std::endl;
            scanCHTLBlock();
        }
    }
}

void UnifiedScanner::scanCHTLBlock() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string content;
    
    while (position_ < input_.length()) {
        char c = currentChar();
        
        // 检查是否为其他语言的边界
        if (enableSyntaxBoundaryDetection_) {
            if (detectCHTLJSBoundary() || detectCSSBoundary() || 
                detectJSBoundary() || detectHTMLBoundary()) {
                break;
            }
        }
        
        content += c;
        advance();
    }
    
    if (!content.empty()) {
        CodeFragment fragment(FragmentType::CHTL, content, startLine, line_, 
                             startColumn, column_);
        fragments_.push_back(fragment);
    }
}

void UnifiedScanner::scanCHTLJSBlock() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string content;
    
    while (position_ < input_.length()) {
        char c = currentChar();
        
        // 检查是否为其他语言的边界
        if (enableSyntaxBoundaryDetection_) {
            if (detectCHTLBoundary() || detectCSSBoundary() || 
                detectJSBoundary() || detectHTMLBoundary()) {
                break;
            }
        }
        
        content += c;
        advance();
    }
    
    if (!content.empty()) {
        CodeFragment fragment(FragmentType::CHTL_JS, content, startLine, line_, 
                             startColumn, column_);
        fragments_.push_back(fragment);
    }
}

void UnifiedScanner::scanCSSBlock() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string content;
    
    while (position_ < input_.length()) {
        char c = currentChar();
        
        // 检查是否为其他语言的边界
        if (enableSyntaxBoundaryDetection_) {
            if (detectCHTLBoundary() || detectCHTLJSBoundary() || 
                detectJSBoundary() || detectHTMLBoundary()) {
                break;
            }
        }
        
        content += c;
        advance();
    }
    
    if (!content.empty()) {
        CodeFragment fragment(FragmentType::CSS, content, startLine, line_, 
                             startColumn, column_);
        fragments_.push_back(fragment);
    }
}

void UnifiedScanner::scanJSBlock() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string content;
    
    while (position_ < input_.length()) {
        char c = currentChar();
        
        // 检查是否为其他语言的边界
        if (enableSyntaxBoundaryDetection_) {
            if (detectCHTLBoundary() || detectCHTLJSBoundary() || 
                detectCSSBoundary() || detectHTMLBoundary()) {
                break;
            }
        }
        
        content += c;
        advance();
    }
    
    if (!content.empty()) {
        CodeFragment fragment(FragmentType::JS, content, startLine, line_, 
                             startColumn, column_);
        fragments_.push_back(fragment);
    }
}

void UnifiedScanner::scanHTMLBlock() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string content;
    
    while (position_ < input_.length()) {
        char c = currentChar();
        
        // 检查是否为其他语言的边界
        if (enableSyntaxBoundaryDetection_) {
            if (detectCHTLBoundary() || detectCHTLJSBoundary() || 
                detectCSSBoundary() || detectJSBoundary()) {
                break;
            }
        }
        
        content += c;
        advance();
    }
    
    if (!content.empty()) {
        CodeFragment fragment(FragmentType::HTML, content, startLine, line_, 
                             startColumn, column_);
        fragments_.push_back(fragment);
    }
}

bool UnifiedScanner::detectCHTLBoundary() {
    // 检测CHTL语法边界
    if (position_ >= input_.length()) return false;
    
    char c = currentChar();
    
    // 检测HTML标签
    if (c == '<' && peekChar() != '/') {
        return true;
    }
    
    // 检测CHTL关键字
    std::string keyword = readIdentifier();
    std::cout << "DEBUG: detectCHTLBoundary - keyword: '" << keyword << "', isCHTLKeyword: " << isCHTLKeyword(keyword) << std::endl;
    if (!keyword.empty() && isCHTLKeyword(keyword)) {
        return true;
    }
    
    // 回退位置
    position_ -= keyword.length();
    return false;
}

bool UnifiedScanner::detectCHTLJSBoundary() {
    // 检测CHTL JS语法边界
    if (position_ >= input_.length()) return false;
    
    char c = currentChar();
    
    // 检测CHTL JS关键字
    if (c == '{' && peekChar() == '{') {
        return true;
    }
    
    // 检测CHTL JS函数
    std::string keyword = readIdentifier();
    if (!keyword.empty() && isCHTLJSKeyword(keyword)) {
        return true;
    }
    
    // 回退位置
    position_ -= keyword.length();
    return false;
}

bool UnifiedScanner::detectCSSBoundary() {
    // 检测CSS语法边界
    if (position_ >= input_.length()) return false;
    
    char c = currentChar();
    
    // 检测CSS选择器
    if (c == '.' || c == '#' || c == '@') {
        return true;
    }
    
    // 检测CSS关键字
    std::string keyword = readIdentifier();
    if (!keyword.empty() && isCSSKeyword(keyword)) {
        return true;
    }
    
    // 回退位置
    position_ -= keyword.length();
    return false;
}

bool UnifiedScanner::detectJSBoundary() {
    // 检测JavaScript语法边界
    if (position_ >= input_.length()) return false;
    
    char c = currentChar();
    
    // 检测JavaScript关键字
    if (c == 'f' && peekChar() == 'u' && 
        position_ + 2 < input_.length() && 
        input_[position_ + 2] == 'n' && 
        input_[position_ + 3] == 'c' && 
        input_[position_ + 4] == 't' && 
        input_[position_ + 5] == 'i' && 
        input_[position_ + 6] == 'o' && 
        input_[position_ + 7] == 'n') {
        return true;
    }
    
    // 检测JavaScript关键字
    std::string keyword = readIdentifier();
    if (!keyword.empty() && isJSKeyword(keyword)) {
        return true;
    }
    
    // 回退位置
    position_ -= keyword.length();
    return false;
}

bool UnifiedScanner::detectHTMLBoundary() {
    // 检测HTML语法边界
    if (position_ >= input_.length()) return false;
    
    char c = currentChar();
    
    // 检测HTML标签
    if (c == '<') {
        return true;
    }
    
    return false;
}

std::string UnifiedScanner::createPlaceholder(const std::string& content, FragmentType type) {
    std::string placeholder = generatePlaceholderName();
    addPlaceholder(placeholder, content, type, line_, column_);
    return placeholder;
}

std::string UnifiedScanner::replaceWithPlaceholder(const std::string& content, FragmentType type) {
    if (enablePlaceholders_) {
        return createPlaceholder(content, type);
    }
    return content;
}

void UnifiedScanner::processPlaceholders() {
    // 处理占位符替换
    for (auto& fragment : fragments_) {
        // 这里可以实现占位符替换逻辑
    }
}

void UnifiedScanner::processFragments() {
    // 处理代码片段
    optimizeFragments();
}

void UnifiedScanner::optimizeFragments() {
    // 优化代码片段
    // 移除空的片段
    fragments_.erase(
        std::remove_if(fragments_.begin(), fragments_.end(),
                      [](const CodeFragment& fragment) {
                          return fragment.content.empty() || 
                                 (fragment.content.length() == 1 && 
                                  std::isspace(fragment.content[0]));
                      }),
        fragments_.end()
    );
}

char UnifiedScanner::currentChar() const {
    return (position_ < input_.length()) ? input_[position_] : '\0';
}

char UnifiedScanner::peekChar() const {
    return (position_ + 1 < input_.length()) ? input_[position_ + 1] : '\0';
}

char UnifiedScanner::nextChar() {
    if (position_ < input_.length()) {
        char c = input_[position_];
        advance();
        return c;
    }
    return '\0';
}

void UnifiedScanner::advance() {
    if (position_ < input_.length()) {
        updatePosition(input_[position_]);
        position_++;
    }
}

void UnifiedScanner::skipWhitespace() {
    while (position_ < input_.length() && std::isspace(currentChar())) {
        advance();
    }
}

void UnifiedScanner::skipNewlines() {
    while (position_ < input_.length() && currentChar() == '\n') {
        advance();
    }
}

std::string UnifiedScanner::readString(char delimiter) {
    std::string result;
    advance(); // 跳过开始引号
    
    while (position_ < input_.length() && currentChar() != delimiter) {
        char c = currentChar();
        if (c == '\\') {
            advance();
            char next = currentChar();
            switch (next) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '\\': result += '\\'; break;
                case '"': result += '"'; break;
                case '\'': result += '\''; break;
                default: result += next; break;
            }
            advance();
        } else {
            result += c;
            advance();
        }
    }
    
    if (currentChar() == delimiter) {
        advance(); // 跳过结束引号
    }
    
    return result;
}

std::string UnifiedScanner::readRawString() {
    // 实现原始字符串读取
    return "";
}

std::string UnifiedScanner::readTemplateString() {
    // 实现模板字符串读取
    return "";
}

std::string UnifiedScanner::readIdentifier() {
    std::string result;
    
    // 读取第一个字符
    if (isIdentifierStart(currentChar())) {
        result += currentChar();
        advance();
    }
    
    // 读取后续字符
    while (position_ < input_.length() && isIdentifierChar(currentChar())) {
        result += currentChar();
        advance();
    }
    
    return result;
}

bool UnifiedScanner::isIdentifierStart(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool UnifiedScanner::isIdentifierChar(char c) {
    return isIdentifierStart(c) || (c >= '0' && c <= '9');
}

std::string UnifiedScanner::readLineComment() {
    std::string result;
    advance(); advance(); // 跳过 //
    
    while (position_ < input_.length() && currentChar() != '\n') {
        result += currentChar();
        advance();
    }
    
    return result;
}

std::string UnifiedScanner::readBlockComment() {
    std::string result;
    advance(); advance(); // 跳过 /*
    
    while (position_ < input_.length()) {
        if (currentChar() == '*' && peekChar() == '/') {
            advance(); advance();
            break;
        }
        result += currentChar();
        advance();
    }
    
    return result;
}

bool UnifiedScanner::isBalanced(const std::string& code) {
    return isBalanced('{', '}', code) && 
           isBalanced('(', ')', code) && 
           isBalanced('[', ']', code);
}

bool UnifiedScanner::isBalanced(char open, char close, const std::string& code) {
    int count = 0;
    for (char c : code) {
        if (c == open) count++;
        else if (c == close) count--;
        if (count < 0) return false;
    }
    return count == 0;
}

bool UnifiedScanner::isCHTLKeyword(const std::string& str) {
    return chtlKeywords_.find(str) != chtlKeywords_.end();
}

bool UnifiedScanner::isCHTLJSKeyword(const std::string& str) {
    return chtljsKeywords_.find(str) != chtljsKeywords_.end();
}

bool UnifiedScanner::isCSSKeyword(const std::string& str) {
    return cssKeywords_.find(str) != cssKeywords_.end();
}

bool UnifiedScanner::isJSKeyword(const std::string& str) {
    return jsKeywords_.find(str) != jsKeywords_.end();
}

bool UnifiedScanner::isHTMLTag(const std::string& str) {
    return htmlTags_.find(str) != htmlTags_.end();
}

bool UnifiedScanner::isCHTLBlockStart() {
    return detectCHTLBoundary();
}

bool UnifiedScanner::isCHTLBlockEnd() {
    // 简化实现
    return false;
}

bool UnifiedScanner::isCHTLJSBlockStart() {
    return detectCHTLJSBoundary();
}

bool UnifiedScanner::isCHTLJSBlockEnd() {
    // 简化实现
    return false;
}

bool UnifiedScanner::isCSSBlockStart() {
    return detectCSSBoundary();
}

bool UnifiedScanner::isCSSBlockEnd() {
    // 简化实现
    return false;
}

bool UnifiedScanner::isJSBlockStart() {
    return detectJSBoundary();
}

bool UnifiedScanner::isJSBlockEnd() {
    // 简化实现
    return false;
}

bool UnifiedScanner::isHTMLBlockStart() {
    return detectHTMLBoundary();
}

bool UnifiedScanner::isHTMLBlockEnd() {
    // 简化实现
    return false;
}

void UnifiedScanner::reportError(const std::string& message) {
    reportError(message, line_, column_);
}

void UnifiedScanner::reportError(const std::string& message, size_t line, size_t column) {
    ScannerError error;
    error.line = line;
    error.column = column;
    error.message = message;
    error.context = getContext(line, column);
    errors_.push_back(error);
}

std::string UnifiedScanner::getContext(size_t line, size_t column) const {
    // 简化实现，返回当前行内容
    return "";
}

void UnifiedScanner::updatePosition(char c) {
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
}

void UnifiedScanner::resetPosition() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
}

void UnifiedScanner::reset() {
    resetPosition();
    fragments_.clear();
    placeholders_.clear();
    errors_.clear();
    placeholderCounter_ = 0;
}

void UnifiedScanner::setInput(const std::string& input) {
    input_ = input;
    reset();
}

std::string UnifiedScanner::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

bool UnifiedScanner::isEmptyOrWhitespace(const std::string& str) {
    return str.empty() || std::all_of(str.begin(), str.end(), ::isspace);
}

std::string UnifiedScanner::generatePlaceholderName() {
    return "_PLACEHOLDER_" + std::to_string(placeholderCounter_++) + "_";
}

void UnifiedScanner::initializeKeywords() {
    // 初始化CHTL关键字
    chtlKeywords_ = {
        "html", "head", "body", "div", "span", "text", "style", "script",
        "[Template]", "[Custom]", "[Origin]", "[Import]", "[Namespace]",
        "[Configuration]", "[Info]", "[Export]"
    };
    
    // 初始化CHTL JS关键字
    chtljsKeywords_ = {
        "Listen", "Animate", "Router", "ScriptLoader", "printMylove",
        "iNeverAway", "util", "Vir"
    };
    
    // 初始化CSS关键字
    cssKeywords_ = {
        "color", "background", "width", "height", "margin", "padding",
        "border", "font", "display", "position", "float", "clear"
    };
    
    // 初始化JavaScript关键字
    jsKeywords_ = {
        "function", "var", "let", "const", "if", "else", "for", "while",
        "do", "switch", "case", "default", "break", "continue", "return",
        "try", "catch", "finally", "throw", "new", "this", "typeof",
        "instanceof", "in", "of", "class", "extends", "super", "static"
    };
    
    // 初始化HTML标签
    htmlTags_ = {
        "html", "head", "body", "div", "span", "p", "h1", "h2", "h3",
        "h4", "h5", "h6", "a", "img", "ul", "ol", "li", "table", "tr",
        "td", "th", "form", "input", "button", "select", "option", "textarea"
    };
}

} // namespace Scanner
} // namespace CHTL