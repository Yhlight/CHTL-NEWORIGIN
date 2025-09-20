#ifndef UNIFIED_SCANNER_H
#define UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <memory>

// 代码片段结构
struct CodeSegment {
    std::string content;
    std::string type;  // "chtl", "chtljs", "css", "js"
    size_t line;
    size_t column;
    size_t length;
};

// 代码片段集合
struct CodeSegments {
    std::vector<CodeSegment> chtlSegments;
    std::vector<CodeSegment> chtlJSSegments;
    std::vector<CodeSegment> cssSegments;
    std::vector<CodeSegment> jsSegments;
};

// 编译后的代码
struct CompiledCode {
    std::string chtlCode;
    std::string chtlJSCode;
    std::string cssCode;
    std::string jsCode;
};

class UnifiedScanner {
public:
    UnifiedScanner();
    ~UnifiedScanner() = default;
    
    // 主要扫描接口
    bool scan(const std::string& content, CodeSegments& segments);
    
private:
    // 扫描状态
    enum class ScanState {
        NORMAL,
        IN_CHTL_BLOCK,
        IN_STYLE_BLOCK,
        IN_SCRIPT_BLOCK,
        IN_COMMENT,
        IN_STRING
    };
    
    ScanState currentState_;
    size_t currentLine_;
    size_t currentColumn_;
    
    // 扫描方法
    bool scanContent(const std::string& content, CodeSegments& segments);
    bool scanCHTLBlock(const std::string& content, size_t& pos, CodeSegments& segments);
    bool scanStyleBlock(const std::string& content, size_t& pos, CodeSegments& segments);
    bool scanScriptBlock(const std::string& content, size_t& pos, CodeSegments& segments);
    bool scanComment(const std::string& content, size_t& pos);
    bool scanString(const std::string& content, size_t& pos);
    
    // 辅助方法
    bool isCHTLKeyword(const std::string& word);
    bool isCHTLJSKeyword(const std::string& word);
    bool isCSSKeyword(const std::string& word);
    bool isJSKeyword(const std::string& word);
    
    // 占位符机制
    std::string createPlaceholder(const std::string& type, size_t id);
    std::string replaceWithPlaceholder(const std::string& content, const std::string& placeholder);
    
    // 占位符映射
    std::vector<std::pair<std::string, std::string>> placeholderMap_;
    size_t placeholderCounter_;
};

#endif // UNIFIED_SCANNER_H