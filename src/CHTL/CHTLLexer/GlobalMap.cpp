#include "GlobalMap.h"
#include "../../Util/StringUtil/StringUtil.h"

namespace CHTL {

GlobalMap::GlobalMap() {
    initializeKeywords();
    initializeHtmlTags();
}

void GlobalMap::initializeKeywords() {
    // CHTL关键字
    keywordMap["text"] = TokenType::TextKeyword;
    keywordMap["style"] = TokenType::StyleKeyword;
    keywordMap["script"] = TokenType::ScriptKeyword;
    
    // 模板和自定义关键字
    keywordMap["[Template]"] = TokenType::Template;
    keywordMap["[Custom]"] = TokenType::Custom;
    keywordMap["[Origin]"] = TokenType::Origin;
    keywordMap["[Import]"] = TokenType::Import;
    keywordMap["[Namespace]"] = TokenType::Namespace;
    keywordMap["[Configuration]"] = TokenType::Configuration;
    
    // @类型关键字
    keywordMap["@Style"] = TokenType::AtStyle;
    keywordMap["@Element"] = TokenType::AtElement;
    keywordMap["@Var"] = TokenType::AtVar;
    keywordMap["@Html"] = TokenType::AtHtml;
    keywordMap["@JavaScript"] = TokenType::AtJavaScript;
    keywordMap["@Chtl"] = TokenType::AtChtl;
    keywordMap["@CJmod"] = TokenType::AtCJmod;
    keywordMap["@Config"] = TokenType::AtConfig;
    
    // 操作符关键字
    keywordMap["inherit"] = TokenType::InheritKeyword;
    keywordMap["delete"] = TokenType::DeleteKeyword;
    keywordMap["insert"] = TokenType::InsertKeyword;
    keywordMap["after"] = TokenType::AfterKeyword;
    keywordMap["before"] = TokenType::BeforeKeyword;
    keywordMap["replace"] = TokenType::ReplaceKeyword;
    keywordMap["from"] = TokenType::FromKeyword;
    keywordMap["as"] = TokenType::AsKeyword;
    keywordMap["except"] = TokenType::ExceptKeyword;
    keywordMap["use"] = TokenType::UseKeyword;
    
    // 特殊关键字（需要两个单词）
    // 这些会在词法分析时特殊处理
    
    // CHTL JS关键字
    keywordMap["Vir"] = TokenType::VirKeyword;
    keywordMap["Listen"] = TokenType::ListenKeyword;
    keywordMap["Delegate"] = TokenType::DelegateKeyword;
    keywordMap["Animate"] = TokenType::AnimateKeyword;
    keywordMap["Router"] = TokenType::RouterKeyword;
    keywordMap["ScriptLoader"] = TokenType::ScriptLoaderKeyword;
}

void GlobalMap::initializeHtmlTags() {
    // HTML5标签
    const char* tags[] = {
        // 文档结构
        "html", "head", "body", "title", "meta", "link", "style", "script",
        
        // 内容分区
        "header", "footer", "nav", "main", "section", "article", "aside",
        "h1", "h2", "h3", "h4", "h5", "h6",
        
        // 文本内容
        "div", "p", "span", "a", "strong", "em", "b", "i", "u", "s",
        "code", "pre", "blockquote", "cite", "q", "abbr", "time",
        "small", "mark", "del", "ins", "sub", "sup",
        
        // 列表
        "ul", "ol", "li", "dl", "dt", "dd",
        
        // 表格
        "table", "thead", "tbody", "tfoot", "tr", "th", "td",
        "caption", "colgroup", "col",
        
        // 表单
        "form", "input", "textarea", "button", "select", "option",
        "optgroup", "label", "fieldset", "legend", "datalist", "output",
        
        // 多媒体
        "img", "audio", "video", "source", "track", "canvas", "svg",
        "picture", "figure", "figcaption",
        
        // 嵌入内容
        "iframe", "embed", "object", "param",
        
        // 其他
        "br", "hr", "wbr", "area", "map", "base", "noscript",
        "template", "slot", "details", "summary", "dialog", "menu"
    };
    
    for (const char* tag : tags) {
        htmlTags.insert(tag);
    }
}

bool GlobalMap::isKeyword(const String& str) const {
    return keywordMap.find(str) != keywordMap.end();
}

bool GlobalMap::isHtmlTag(const String& str) const {
    return htmlTags.find(Util::StringUtil::toLower(str)) != htmlTags.end();
}

bool GlobalMap::isCHTLKeyword(const String& str) const {
    auto it = keywordMap.find(str);
    if (it == keywordMap.end()) {
        return false;
    }
    
    TokenType type = it->second;
    return type >= TokenType::TextKeyword && type <= TokenType::UseKeyword;
}

bool GlobalMap::isCHTLJSKeyword(const String& str) const {
    auto it = keywordMap.find(str);
    if (it == keywordMap.end()) {
        return false;
    }
    
    TokenType type = it->second;
    return type >= TokenType::VirKeyword && type <= TokenType::ScriptLoaderKeyword;
}

Optional<TokenType> GlobalMap::getKeywordType(const String& str) const {
    auto it = keywordMap.find(str);
    if (it != keywordMap.end()) {
        return it->second;
    }
    
    // 检查是否是HTML标签
    if (isHtmlTag(str)) {
        return TokenType::HtmlKeyword;
    }
    
    return std::nullopt;
}

void GlobalMap::addCustomKeyword(const String& keyword, TokenType type) {
    keywordMap[keyword] = type;
}

bool GlobalMap::isValidIdentifier(const String& str) const {
    if (str.empty()) {
        return false;
    }
    
    if (!Util::StringUtil::isIdentifierStart(str[0])) {
        return false;
    }
    
    for (size_t i = 1; i < str.length(); ++i) {
        if (!Util::StringUtil::isIdentifierChar(str[i])) {
            return false;
        }
    }
    
    return !isKeyword(str);
}

} // namespace CHTL
