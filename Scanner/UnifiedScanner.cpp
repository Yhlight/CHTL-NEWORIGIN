#include "UnifiedScanner.h"
#include <iostream>
#include <algorithm>
#include <regex>

UnifiedScanner::UnifiedScanner() 
    : currentState_(ScanState::NORMAL)
    , currentLine_(1)
    , currentColumn_(1)
    , placeholderCounter_(0) {
}

bool UnifiedScanner::scan(const std::string& content, CodeSegments& segments) {
    std::cout << "统一扫描器开始工作..." << std::endl;
    
    // 重置状态
    currentState_ = ScanState::NORMAL;
    currentLine_ = 1;
    currentColumn_ = 1;
    placeholderMap_.clear();
    placeholderCounter_ = 0;
    
    return scanContent(content, segments);
}

bool UnifiedScanner::scanContent(const std::string& content, CodeSegments& segments) {
    size_t pos = 0;
    size_t contentLength = content.length();
    
    while (pos < contentLength) {
        char currentChar = content[pos];
        
        // 更新行列信息
        if (currentChar == '\n') {
            currentLine_++;
            currentColumn_ = 1;
        } else {
            currentColumn_++;
        }
        
        switch (currentState_) {
            case ScanState::NORMAL:
                // 检查是否进入特殊块
                if (currentChar == '[' && pos + 1 < contentLength) {
                    // 检查CHTL关键字
                    size_t keywordEnd = pos;
                    while (keywordEnd < contentLength && 
                           (std::isalnum(content[keywordEnd]) || content[keywordEnd] == ']')) {
                        keywordEnd++;
                    }
                    
                    std::string potentialKeyword = content.substr(pos, keywordEnd - pos);
                    if (isCHTLKeyword(potentialKeyword)) {
                        if (scanCHTLBlock(content, pos, segments)) {
                            continue;
                        }
                    }
                } else if (currentChar == '<' && pos + 6 < contentLength) {
                    // 检查HTML标签
                    std::string tag = content.substr(pos, 6);
                    if (tag == "<style") {
                        if (scanStyleBlock(content, pos, segments)) {
                            continue;
                        }
                    } else if (tag == "<script") {
                        if (scanScriptBlock(content, pos, segments)) {
                            continue;
                        }
                    }
                } else if (currentChar == '/' && pos + 1 < contentLength) {
                    // 检查注释
                    if (content[pos + 1] == '/' || content[pos + 1] == '*') {
                        if (scanComment(content, pos)) {
                            continue;
                        }
                    }
                } else if (currentChar == '"' || currentChar == '\'') {
                    // 检查字符串
                    if (scanString(content, pos)) {
                        continue;
                    }
                }
                break;
                
            case ScanState::IN_CHTL_BLOCK:
                // CHTL块内的处理
                break;
                
            case ScanState::IN_STYLE_BLOCK:
                // 样式块内的处理
                break;
                
            case ScanState::IN_SCRIPT_BLOCK:
                // 脚本块内的处理
                break;
                
            case ScanState::IN_COMMENT:
                // 注释内的处理
                break;
                
            case ScanState::IN_STRING:
                // 字符串内的处理
                break;
        }
        
        pos++;
    }
    
    return true;
}

bool UnifiedScanner::scanCHTLBlock(const std::string& content, size_t& pos, CodeSegments& segments) {
    size_t startPos = pos;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    // 查找CHTL块的结束位置
    size_t blockEnd = content.find("]", pos);
    if (blockEnd == std::string::npos) {
        return false;
    }
    
    // 查找对应的结束块
    size_t braceStart = content.find("{", blockEnd);
    if (braceStart == std::string::npos) {
        return false;
    }
    
    // 计算大括号的匹配
    int braceCount = 1;
    size_t braceEnd = braceStart + 1;
    
    while (braceEnd < content.length() && braceCount > 0) {
        if (content[braceEnd] == '{') {
            braceCount++;
        } else if (content[braceEnd] == '}') {
            braceCount--;
        }
        braceEnd++;
    }
    
    if (braceCount != 0) {
        return false;
    }
    
    // 提取CHTL代码
    std::string chtlCode = content.substr(startPos, braceEnd - startPos);
    
    // 创建代码片段
    CodeSegment segment;
    segment.content = chtlCode;
    segment.type = "chtl";
    segment.line = startLine;
    segment.column = startColumn;
    segment.length = chtlCode.length();
    
    segments.chtlSegments.push_back(segment);
    
    // 更新位置
    pos = braceEnd;
    currentColumn_ += (braceEnd - startPos);
    
    return true;
}

bool UnifiedScanner::scanStyleBlock(const std::string& content, size_t& pos, CodeSegments& segments) {
    size_t startPos = pos;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    // 查找style标签的结束
    size_t tagEnd = content.find(">", pos);
    if (tagEnd == std::string::npos) {
        return false;
    }
    
    // 查找</style>标签
    size_t closeTag = content.find("</style>", tagEnd);
    if (closeTag == std::string::npos) {
        return false;
    }
    
    // 提取CSS代码
    std::string cssCode = content.substr(tagEnd + 1, closeTag - tagEnd - 1);
    
    // 创建代码片段
    CodeSegment segment;
    segment.content = cssCode;
    segment.type = "css";
    segment.line = startLine;
    segment.column = startColumn;
    segment.length = cssCode.length();
    
    segments.cssSegments.push_back(segment);
    
    // 更新位置
    pos = closeTag + 8; // "</style>"的长度
    currentColumn_ += (pos - startPos);
    
    return true;
}

bool UnifiedScanner::scanScriptBlock(const std::string& content, size_t& pos, CodeSegments& segments) {
    size_t startPos = pos;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    // 查找script标签的结束
    size_t tagEnd = content.find(">", pos);
    if (tagEnd == std::string::npos) {
        return false;
    }
    
    // 查找</script>标签
    size_t closeTag = content.find("</script>", tagEnd);
    if (closeTag == std::string::npos) {
        return false;
    }
    
    // 提取JS代码
    std::string jsCode = content.substr(tagEnd + 1, closeTag - tagEnd - 1);
    
    // 检查是否包含CHTL JS语法
    if (jsCode.find("{{") != std::string::npos || 
        jsCode.find("Listen") != std::string::npos ||
        jsCode.find("Animate") != std::string::npos) {
        
        // 分离CHTL JS和普通JS
        std::string chtlJSCode;
        std::string pureJSCode;
        
        // 这里需要更复杂的逻辑来分离CHTL JS和JS
        // 暂时简单处理
        if (jsCode.find("{{") != std::string::npos) {
            chtlJSCode = jsCode;
        } else {
            pureJSCode = jsCode;
        }
        
        if (!chtlJSCode.empty()) {
            CodeSegment chtlJSSegment;
            chtlJSSegment.content = chtlJSCode;
            chtlJSSegment.type = "chtljs";
            chtlJSSegment.line = startLine;
            chtlJSSegment.column = startColumn;
            chtlJSSegment.length = chtlJSCode.length();
            segments.chtlJSSegments.push_back(chtlJSSegment);
        }
        
        if (!pureJSCode.empty()) {
            CodeSegment jsSegment;
            jsSegment.content = pureJSCode;
            jsSegment.type = "js";
            jsSegment.line = startLine;
            jsSegment.column = startColumn;
            jsSegment.length = pureJSCode.length();
            segments.jsSegments.push_back(jsSegment);
        }
    } else {
        // 纯JS代码
        CodeSegment segment;
        segment.content = jsCode;
        segment.type = "js";
        segment.line = startLine;
        segment.column = startColumn;
        segment.length = jsCode.length();
        segments.jsSegments.push_back(segment);
    }
    
    // 更新位置
    pos = closeTag + 9; // "</script>"的长度
    currentColumn_ += (pos - startPos);
    
    return true;
}

bool UnifiedScanner::scanComment(const std::string& content, size_t& pos) {
    if (content[pos + 1] == '/') {
        // 单行注释
        size_t endPos = content.find('\n', pos);
        if (endPos == std::string::npos) {
            endPos = content.length();
        }
        pos = endPos;
        return true;
    } else if (content[pos + 1] == '*') {
        // 多行注释
        size_t endPos = content.find("*/", pos + 2);
        if (endPos == std::string::npos) {
            return false;
        }
        pos = endPos + 2;
        return true;
    }
    return false;
}

bool UnifiedScanner::scanString(const std::string& content, size_t& pos) {
    char quote = content[pos];
    size_t startPos = pos;
    pos++;
    
    while (pos < content.length()) {
        if (content[pos] == quote && content[pos - 1] != '\\') {
            pos++;
            return true;
        }
        pos++;
    }
    
    return false;
}

bool UnifiedScanner::isCHTLKeyword(const std::string& word) {
    static const std::vector<std::string> keywords = {
        "[Template]", "[Custom]", "[Origin]", "[Import]", 
        "[Configuration]", "[Namespace]", "[Info]", "[Export]"
    };
    
    return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
}

bool UnifiedScanner::isCHTLJSKeyword(const std::string& word) {
    static const std::vector<std::string> keywords = {
        "Listen", "Animate", "Router", "Vir", "iNeverAway", 
        "printMylove", "ScriptLoader"
    };
    
    return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
}

bool UnifiedScanner::isCSSKeyword(const std::string& word) {
    // CSS关键字检测
    return false;
}

bool UnifiedScanner::isJSKeyword(const std::string& word) {
    static const std::vector<std::string> keywords = {
        "function", "var", "let", "const", "if", "else", "for", "while",
        "return", "class", "import", "export", "async", "await"
    };
    
    return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
}

std::string UnifiedScanner::createPlaceholder(const std::string& type, size_t id) {
    return "_" + type + "_PLACEHOLDER_" + std::to_string(id) + "_";
}

std::string UnifiedScanner::replaceWithPlaceholder(const std::string& content, const std::string& placeholder) {
    placeholderMap_.push_back({placeholder, content});
    return placeholder;
}