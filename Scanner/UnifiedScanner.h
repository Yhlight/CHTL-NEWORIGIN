#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {
namespace Scanner {

// 代码片段类型
enum class FragmentType {
    CHTL,           // CHTL代码片段
    CHTL_JS,        // CHTL JS代码片段
    CSS,            // CSS代码片段
    JS,             // JavaScript代码片段
    HTML,           // HTML代码片段
    UNKNOWN         // 未知类型
};

// 代码片段
struct CodeFragment {
    FragmentType type;
    std::string content;
    size_t startLine;
    size_t endLine;
    size_t startColumn;
    size_t endColumn;
    std::string originalContent;  // 原始内容（用于占位符替换）
    
    CodeFragment() : type(FragmentType::UNKNOWN), startLine(0), endLine(0), 
                     startColumn(0), endColumn(0) {}
    
    CodeFragment(FragmentType t, const std::string& c, size_t sl, size_t el, 
                 size_t sc, size_t ec)
        : type(t), content(c), startLine(sl), endLine(el), 
          startColumn(sc), endColumn(ec), originalContent(c) {}
};

// 占位符信息
struct PlaceholderInfo {
    std::string placeholder;
    std::string originalContent;
    FragmentType type;
    size_t line;
    size_t column;
    
    PlaceholderInfo() : type(FragmentType::UNKNOWN), line(0), column(0) {}
    
    PlaceholderInfo(const std::string& ph, const std::string& orig, 
                   FragmentType t, size_t l, size_t c)
        : placeholder(ph), originalContent(orig), type(t), line(l), column(c) {}
};

// 扫描结果
struct ScanResult {
    std::vector<CodeFragment> fragments;
    std::vector<PlaceholderInfo> placeholders;
    std::string processedCode;
    bool success;
    std::string errorMessage;
    
    ScanResult() : success(false) {}
};

// 统一扫描器
class UnifiedScanner {
public:
    UnifiedScanner();
    ~UnifiedScanner();
    
    // 主扫描函数
    ScanResult scan(const std::string& input);
    
    // 扫描CHTL文件
    ScanResult scanFile(const std::string& filename);
    
    // 设置选项
    void setEnablePlaceholders(bool enable);
    void setEnableFragmentSeparation(bool enable);
    void setEnableSyntaxBoundaryDetection(bool enable);
    
    // 获取选项
    bool isEnablePlaceholders() const;
    bool isEnableFragmentSeparation() const;
    bool isEnableSyntaxBoundaryDetection() const;
    
    // 错误处理
    struct ScannerError {
        size_t line;
        size_t column;
        std::string message;
        std::string context;
    };
    
    const std::vector<ScannerError>& getErrors() const;
    bool hasErrors() const;
    void clearErrors();
    
    // 占位符管理
    void addPlaceholder(const std::string& placeholder, const std::string& content, 
                       FragmentType type, size_t line, size_t column);
    void removePlaceholder(const std::string& placeholder);
    void clearPlaceholders();
    
    // 代码片段管理
    void addFragment(const CodeFragment& fragment);
    void removeFragment(size_t index);
    void clearFragments();
    
    // 获取特定类型的代码片段
    std::vector<CodeFragment> getFragmentsByType(FragmentType type) const;
    
    // 合并代码片段
    std::string mergeFragments(const std::vector<FragmentType>& types) const;
    
    // 恢复占位符
    std::string restorePlaceholders(const std::string& processedCode) const;
    
    // 辅助方法
    std::string readIdentifier();
    bool isIdentifierStart(char c);
    bool isIdentifierChar(char c);
    
private:
    // 选项
    bool enablePlaceholders_;
    bool enableFragmentSeparation_;
    bool enableSyntaxBoundaryDetection_;
    
    // 状态
    std::string input_;
    size_t position_;
    size_t line_;
    size_t column_;
    
    // 结果
    std::vector<CodeFragment> fragments_;
    std::vector<PlaceholderInfo> placeholders_;
    std::vector<ScannerError> errors_;
    
    // 占位符计数器
    size_t placeholderCounter_;
    
    // 核心扫描函数
    void scanCode();
    void scanCHTLBlock();
    void scanCHTLJSBlock();
    void scanCSSBlock();
    void scanJSBlock();
    void scanHTMLBlock();
    
    // 语法边界检测
    bool detectCHTLBoundary();
    bool detectCHTLJSBoundary();
    bool detectCSSBoundary();
    bool detectJSBoundary();
    bool detectHTMLBoundary();
    
    // 占位符处理
    std::string createPlaceholder(const std::string& content, FragmentType type);
    std::string replaceWithPlaceholder(const std::string& content, FragmentType type);
    void processPlaceholders();
    
    // 代码片段处理
    void processFragments();
    void optimizeFragments();
    
    // 字符处理
    char currentChar() const;
    char peekChar() const;
    char nextChar();
    void advance();
    void skipWhitespace();
    void skipNewlines();
    
    // 字符串处理
    std::string readString(char delimiter);
    std::string readRawString();
    std::string readTemplateString();
    
    // 注释处理
    std::string readLineComment();
    std::string readBlockComment();
    
    // 括号匹配
    bool isBalanced(const std::string& code);
    bool isBalanced(char open, char close, const std::string& code);
    
    // 语法检测
    bool isCHTLKeyword(const std::string& str);
    bool isCHTLJSKeyword(const std::string& str);
    bool isCSSKeyword(const std::string& str);
    bool isJSKeyword(const std::string& str);
    bool isHTMLTag(const std::string& str);
    
    // 边界检测
    bool isCHTLBlockStart();
    bool isCHTLBlockEnd();
    bool isCHTLJSBlockStart();
    bool isCHTLJSBlockEnd();
    bool isCSSBlockStart();
    bool isCSSBlockEnd();
    bool isJSBlockStart();
    bool isJSBlockEnd();
    bool isHTMLBlockStart();
    bool isHTMLBlockEnd();
    
    // 错误处理
    void reportError(const std::string& message);
    void reportError(const std::string& message, size_t line, size_t column);
    std::string getContext(size_t line, size_t column) const;
    
    // 位置管理
    void updatePosition(char c);
    void resetPosition();
    
    // 状态管理
    void reset();
    void setInput(const std::string& input);
    
    // 工具函数
    std::string trim(const std::string& str);
    bool isEmptyOrWhitespace(const std::string& str);
    std::string generatePlaceholderName();
    
    // 预定义关键字
    static std::unordered_set<std::string> chtlKeywords_;
    static std::unordered_set<std::string> chtljsKeywords_;
    static std::unordered_set<std::string> cssKeywords_;
    static std::unordered_set<std::string> jsKeywords_;
    static std::unordered_set<std::string> htmlTags_;
    
    // 初始化关键字
    static void initializeKeywords();
    static bool keywordsInitialized_;
};

} // namespace Scanner
} // namespace CHTL