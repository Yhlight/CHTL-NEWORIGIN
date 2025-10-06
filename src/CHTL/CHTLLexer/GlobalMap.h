#ifndef CHTL_GLOBAL_MAP_H
#define CHTL_GLOBAL_MAP_H

#include "../../Common.h"
#include "Token.h"
#include <unordered_set>

namespace CHTL {

// 全局映射表 - 用于管理关键字、HTML标签等
class GlobalMap {
public:
    GlobalMap();
    
    // 检查是否是关键字
    bool isKeyword(const String& str) const;
    bool isHtmlTag(const String& str) const;
    bool isCHTLKeyword(const String& str) const;
    bool isCHTLJSKeyword(const String& str) const;
    
    // 获取关键字对应的Token类型
    Optional<TokenType> getKeywordType(const String& str) const;
    
    // 添加自定义关键字
    void addCustomKeyword(const String& keyword, TokenType type);
    
    // 检查是否是合法的标识符
    bool isValidIdentifier(const String& str) const;
    
private:
    void initializeKeywords();
    void initializeHtmlTags();
    
    HashMap<String, TokenType> keywordMap;
    std::unordered_set<String> htmlTags;
};

} // namespace CHTL

#endif // CHTL_GLOBAL_MAP_H
