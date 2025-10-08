#include "CJMODApi.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTL {
namespace CJMOD {

// ========================================
// AtomArg 实现
// ========================================

AtomArg::AtomArg(Type type, const String& pattern)
    : type_(type), pattern_(pattern) {}

void AtomArg::bind(ValueGetter getter) {
    getter_ = getter;
}

void AtomArg::fillValue(const String& value) {
    if (getter_) {
        value_ = getter_(value);
    } else {
        value_ = value;
    }
}

bool AtomArg::matches(const String& input) const {
    if (type_ == Type::Literal || type_ == Type::Keyword) {
        return input == pattern_;
    }
    // 占位符匹配任意内容
    return true;
}

// ========================================
// Arg 实现
// ========================================

Arg::Arg(const Vector<AtomArg>& atoms) : atoms_(atoms) {}

void Arg::add(const AtomArg& atom) {
    atoms_.push_back(atom);
}

void Arg::bind(const String& placeholder, AtomArg::ValueGetter getter) {
    for (auto& atom : atoms_) {
        if (atom.getPattern() == placeholder) {
            atom.bind(getter);
        }
    }
}

String Arg::match(const String& placeholder, AtomArg::ValueGetter transformer) {
    // 获取当前占位符的匹配计数
    int& counter = matchCounters_[placeholder];
    
    // 遍历找到第counter个匹配的占位符
    int currentMatch = 0;
    for (const auto& atom : atoms_) {
        if (atom.getPattern() == placeholder) {
            if (currentMatch == counter) {
                counter++;  // 增加计数
                String value = atom.getValue();
                return transformer ? transformer(value) : value;
            }
            currentMatch++;
        }
    }
    
    return "";
}

void Arg::fillValue(const Arg& values) {
    if (values.size() != atoms_.size()) {
        return;
    }
    
    for (size_t i = 0; i < atoms_.size(); i++) {
        atoms_[i].fillValue(values[i].getValue());
    }
}

void Arg::fillValue(const Vector<String>& values) {
    size_t minSize = std::min(values.size(), atoms_.size());
    for (size_t i = 0; i < minSize; i++) {
        atoms_[i].fillValue(values[i]);
    }
}

void Arg::transform(const String& pattern) {
    transformed_ = pattern;
}

AtomArg& Arg::operator[](size_t index) {
    return atoms_[index];
}

const AtomArg& Arg::operator[](size_t index) const {
    return atoms_[index];
}

void Arg::print() const {
    std::cout << "[";
    for (size_t i = 0; i < atoms_.size(); i++) {
        std::cout << "\"" << atoms_[i].getPattern() << "\"";
        if (i < atoms_.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

// ========================================
// Syntax 实现
// ========================================

Arg Syntax::analyze(const String& pattern) {
    Arg result;
    
    auto tokens = tokenize(pattern);
    for (const auto& token : tokens) {
        AtomArg::Type type;
        
        if (Util::StringUtil::startsWith(token, "$")) {
            type = parsePlaceholderType(token);
        } else if (token == "...") {
            type = AtomArg::Type::Variadic;
        } else {
            type = AtomArg::Type::Literal;
        }
        
        result.add(AtomArg(type, token));
    }
    
    return result;
}

bool Syntax::isObject(const String& code) {
    auto trimmed = Util::StringUtil::trim(code);
    return Util::StringUtil::startsWith(trimmed, "{") && Util::StringUtil::endsWith(trimmed, "}");
}

bool Syntax::isFunction(const String& code) {
    auto trimmed = Util::StringUtil::trim(code);
    return Util::StringUtil::startsWith(trimmed, "function") || 
           trimmed.find("=>") != String::npos;
}

bool Syntax::isArray(const String& code) {
    auto trimmed = Util::StringUtil::trim(code);
    return Util::StringUtil::startsWith(trimmed, "[") && Util::StringUtil::endsWith(trimmed, "]");
}

bool Syntax::isCHTLJSFunction(const String& code) {
    // 检查是否包含 { key: value } 格式
    // 简单的启发式检查
    auto trimmed = Util::StringUtil::trim(code);
    
    // 查找函数名后跟 {
    size_t bracePos = trimmed.find('{');
    if (bracePos == String::npos) {
        return false;
    }
    
    // 检查是否有冒号（键值对）
    size_t colonPos = trimmed.find(':', bracePos);
    if (colonPos == String::npos) {
        return false;
    }
    
    // 排除常规JS对象
    if (bracePos == 0) {
        return false;
    }
    
    return true;
}

AtomArg::Type Syntax::parsePlaceholderType(const String& placeholder) {
    if (placeholder == "$") {
        return AtomArg::Type::Placeholder;
    } else if (placeholder == "$?") {
        return AtomArg::Type::Optional;
    } else if (placeholder == "$!") {
        return AtomArg::Type::Required;
    } else if (placeholder == "$_") {
        return AtomArg::Type::Unordered;
    } else if (placeholder.find("$!_") != String::npos || 
               placeholder.find("$_!") != String::npos) {
        return AtomArg::Type::Required;  // 组合类型
    } else if (placeholder.find("$?_") != String::npos || 
               placeholder.find("$_?") != String::npos) {
        return AtomArg::Type::Optional;  // 组合类型
    }
    
    return AtomArg::Type::Placeholder;
}

Vector<String> Syntax::tokenize(const String& pattern) {
    Vector<String> tokens;
    String current;
    bool inPlaceholder = false;
    
    for (size_t i = 0; i < pattern.length(); i++) {
        char c = pattern[i];
        
        if (c == '$') {
            if (!current.empty() && current != " ") {
                tokens.push_back(Util::StringUtil::trim(current));
                current.clear();
            }
            inPlaceholder = true;
            current += c;
        } else if (inPlaceholder) {
            if (c == '!' || c == '?' || c == '_') {
                current += c;
            } else {
                tokens.push_back(current);
                current.clear();
                inPlaceholder = false;
                
                if (c != ' ') {
                    current += c;
                }
            }
        } else if (c == ' ') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else if (i + 2 < pattern.length() && 
                   pattern.substr(i, 3) == "...") {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            tokens.push_back("...");
            i += 2;
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        if (inPlaceholder) {
            tokens.push_back(current);
        } else {
            auto trimmed = Util::StringUtil::trim(current);
            if (!trimmed.empty()) {
                tokens.push_back(trimmed);
            }
        }
    }
    
    return tokens;
}

// ========================================
// CJMODScanner 实现
// ========================================

String CJMODScanner::currentCode_ = "";

void CJMODScanner::setCode(const String& code) {
    currentCode_ = code;
}

Arg CJMODScanner::scan(const Arg& pattern, const String& keyword) {
    return scan(pattern, keyword, currentCode_);
}

Arg CJMODScanner::scan(const Arg& pattern, const String& keyword, const String& code) {
    // 使用双指针扫描算法
    return scanWithTwoPointers(pattern, keyword, code);
}

Arg CJMODScanner::scanWithTwoPointers(const Arg& /* pattern */, const String& keyword, const String& code) {
    Arg result;
    
    // 查找关键字位置
    size_t keywordPos = code.find(keyword);
    if (keywordPos == String::npos) {
        return result;
    }
    
    // 双指针算法
    size_t left = 0;
    size_t right = keywordPos;
    
    // 向前扫描，找到表达式的开始
    while (left < keywordPos) {
        char c = code[left];
        if (c == ';' || c == '\n' || c == '{') {
            left++;
            break;
        }
        left++;
    }
    
    // 跳过空白
    left = skipWhitespace(code, left);
    
    // 向后扫描，找到表达式的结束
    right = keywordPos + keyword.length();
    right = skipWhitespace(code, right);
    
    int depth = 0;
    while (right < code.length()) {
        char c = code[right];
        
        if (c == '{' || c == '(' || c == '[') {
            depth++;
        } else if (c == '}' || c == ')' || c == ']') {
            depth--;
        } else if ((c == ';' || c == '\n') && depth == 0) {
            break;
        }
        
        right++;
    }
    
    // 提取片段并填充结果
    Vector<String> values;
    
    // 提取左侧参数
    if (left < keywordPos) {
        String leftPart = extractFragment(code, left, keywordPos);
        if (!leftPart.empty()) {
            values.push_back(leftPart);
        }
    }
    
    // 添加关键字本身
    values.push_back(keyword);
    
    // 提取右侧参数
    if (keywordPos + keyword.length() < right) {
        String rightPart = extractFragment(code, keywordPos + keyword.length(), right);
        if (!rightPart.empty()) {
            values.push_back(rightPart);
        }
    }
    
    result.fillValue(values);
    return result;
}

Arg CJMODScanner::scanWithDynamicAwareness(const Arg& pattern, const String& keyword, const String& code) {
    // 实现动态感知算法
    // 动态感知算法：根据代码结构动态调整匹配策略
    
    // 1. 先尝试双指针算法
    Arg result = scanWithTwoPointers(pattern, keyword, code);
    
    // 2. 如果双指针失败，尝试递归下降
    if (result.getValues().empty()) {
        // 查找关键字位置
        size_t keywordPos = code.find(keyword);
        if (keywordPos == String::npos) {
            return result;
        }
        
        // 跳过关键字
        size_t pos = keywordPos + keyword.length();
        pos = skipWhitespace(code, pos);
        
        // 根据模式动态提取
        Vector<AtomArg> atoms = pattern.getAtoms();
        for (const auto& atom : atoms) {
            if (atom.getType() == AtomArg::Type::Literal) {
                // 字面量：直接匹配
                String literal = atom.getValue();
                if (code.substr(pos, literal.length()) == literal) {
                    result.add(AtomArg(AtomArg::Type::Literal, literal));
                    pos += literal.length();
                    pos = skipWhitespace(code, pos);
                }
            } else if (atom.getType() == AtomArg::Type::Required ||
                       atom.getType() == AtomArg::Type::Optional) {
                // 占位符：智能提取
                size_t end = pos;
                int braceDepth = 0;
                int parenDepth = 0;
                bool inString = false;
                char stringChar = '\0';
                
                while (end < code.length()) {
                    char ch = code[end];
                    
                    if (!inString) {
                        if (ch == '"' || ch == '\'') {
                            inString = true;
                            stringChar = ch;
                        } else if (ch == '{') {
                            braceDepth++;
                        } else if (ch == '}') {
                            if (braceDepth == 0) break;
                            braceDepth--;
                        } else if (ch == '(') {
                            parenDepth++;
                        } else if (ch == ')') {
                            if (parenDepth == 0) break;
                            parenDepth--;
                        } else if ((ch == ',' || ch == ';') && braceDepth == 0 && parenDepth == 0) {
                            break;
                        }
                    } else {
                        if (ch == stringChar && (end == 0 || code[end-1] != '\\')) {
                            inString = false;
                        }
                    }
                    
                    end++;
                }
                
                String value = extractFragment(code, pos, end);
                result.add(AtomArg(atom.getType(), value));
                pos = end;
                pos = skipWhitespace(code, pos);
            }
        }
    }
    
    return result;
}

String CJMODScanner::extractFragment(const String& code, size_t start, size_t end) {
    if (start >= end || start >= code.length()) {
        return "";
    }
    
    end = std::min(end, code.length());
    String fragment = code.substr(start, end - start);
    return Util::StringUtil::trim(fragment);
}

size_t CJMODScanner::skipWhitespace(const String& code, size_t pos) {
    while (pos < code.length() && std::isspace(code[pos])) {
        pos++;
    }
    return pos;
}

// ========================================
// CJMODGenerator 实现
// ========================================

bool CJMODGenerator::prettyPrint_ = false;
bool CJMODGenerator::wrapIIFE_ = false;

String CJMODGenerator::exportResult(const Arg& arg) {
    String result = arg.getTransformed();
    
    if (wrapIIFE_) {
        result = "(function() {\n" + result + "\n})();";
    }
    
    return result;
}

void CJMODGenerator::setPrettyPrint(bool enable) {
    prettyPrint_ = enable;
}

void CJMODGenerator::setWrapIIFE(bool enable) {
    wrapIIFE_ = enable;
}

// ========================================
// CHTLJSFunction 实现
// ========================================

HashMap<String, Vector<String>> CHTLJSFunction::virtualObjects_;

Arg CHTLJSFunction::createCHTLJSFunction(const String& functionDef) {
    Arg arg = parseFunctionDefinition(functionDef);
    
    // 提取函数名
    size_t spacePos = functionDef.find(' ');
    size_t bracePos = functionDef.find('{');
    if (spacePos != String::npos && bracePos != String::npos && spacePos < bracePos) {
        String funcName = Util::StringUtil::trim(functionDef.substr(0, spacePos));
        bindVirtualObject(funcName);
    }
    
    return arg;
}

void CHTLJSFunction::bindVirtualObject(const String& functionName) {
    if (virtualObjects_.find(functionName) == virtualObjects_.end()) {
        virtualObjects_[functionName] = Vector<String>();
    }
}

bool CHTLJSFunction::hasVirtualObject(const String& functionName) {
    return virtualObjects_.find(functionName) != virtualObjects_.end();
}

Vector<String> CHTLJSFunction::getVirtualObjectKeys(const String& functionName) {
    if (hasVirtualObject(functionName)) {
        return virtualObjects_[functionName];
    }
    return Vector<String>();
}

Arg CHTLJSFunction::parseFunctionDefinition(const String& def) {
    return Syntax::analyze(def);
}

Vector<String> CHTLJSFunction::extractKeys(const String& def) {
    Vector<String> keys;
    
    // 实现键提取逻辑
    // 从 Vir 函数定义中提取虚对象的键
    // 例如: "Listen { click: handler, hover: fn }" -> ["click", "hover"]
    
    // 查找函数体 {...}
    size_t braceStart = def.find('{');
    if (braceStart == String::npos) {
        return keys;
    }
    
    // 查找匹配的右括号
    size_t braceEnd = def.rfind('}');
    if (braceEnd == String::npos || braceEnd <= braceStart) {
        return keys;
    }
    
    // 提取函数体内容
    String body = def.substr(braceStart + 1, braceEnd - braceStart - 1);
    
    // 解析键值对
    size_t pos = 0;
    while (pos < body.length()) {
        // 跳过空白
        while (pos < body.length() && std::isspace(body[pos])) {
            pos++;
        }
        
        if (pos >= body.length()) break;
        
        // 提取键
        size_t keyStart = pos;
        while (pos < body.length() && (std::isalnum(body[pos]) || body[pos] == '_' || body[pos] == '$')) {
            pos++;
        }
        
        if (pos > keyStart) {
            String key = body.substr(keyStart, pos - keyStart);
            
            // 跳过空白
            while (pos < body.length() && std::isspace(body[pos])) {
                pos++;
            }
            
            // 检查是否是键值对（后面有冒号）
            if (pos < body.length() && body[pos] == ':') {
                keys.push_back(key);
                
                // 跳过值（可能很复杂，需要处理嵌套）
                pos++; // 跳过冒号
                int braceDepth = 0;
                int parenDepth = 0;
                bool inString = false;
                char stringChar = '\0';
                
                while (pos < body.length()) {
                    char ch = body[pos];
                    
                    if (!inString) {
                        if (ch == '"' || ch == '\'') {
                            inString = true;
                            stringChar = ch;
                        } else if (ch == '{') {
                            braceDepth++;
                        } else if (ch == '}') {
                            if (braceDepth == 0) break;
                            braceDepth--;
                        } else if (ch == '(') {
                            parenDepth++;
                        } else if (ch == ')') {
                            if (parenDepth == 0) break;
                            parenDepth--;
                        } else if (ch == ',' && braceDepth == 0 && parenDepth == 0) {
                            pos++;
                            break;
                        }
                    } else {
                        if (ch == stringChar && (pos == 0 || body[pos-1] != '\\')) {
                            inString = false;
                        }
                    }
                    
                    pos++;
                }
            } else {
                // 不是键值对，跳过
                pos++;
            }
        } else {
            pos++;
        }
    }
    
    return keys;
}

// ========================================
// CJMODRegistry 实现
// ========================================

HashMap<String, CJMODRegistry::SyntaxInfo> CJMODRegistry::registry_;

void CJMODRegistry::registerSyntax(const String& name, const SyntaxInfo& info) {
    registry_[name] = info;
}

Vector<String> CJMODRegistry::getAllSyntaxNames() {
    Vector<String> names;
    for (const auto& pair : registry_) {
        names.push_back(pair.first);
    }
    return names;
}

Optional<CJMODRegistry::SyntaxInfo> CJMODRegistry::getSyntaxInfo(const String& name) {
    auto it = registry_.find(name);
    if (it != registry_.end()) {
        return it->second;
    }
    return std::nullopt;
}

String CJMODRegistry::generateJSONManifest() {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"syntaxes\": [\n";
    
    size_t count = 0;
    for (const auto& pair : registry_) {
        ss << "    {\n";
        ss << "      \"name\": \"" << pair.first << "\",\n";
        ss << "      \"pattern\": \"" << pair.second.pattern << "\",\n";
        ss << "      \"description\": \"" << pair.second.description << "\",\n";
        ss << "      \"examples\": [\n";
        
        for (size_t i = 0; i < pair.second.examples.size(); i++) {
            ss << "        \"" << pair.second.examples[i] << "\"";
            if (i < pair.second.examples.size() - 1) {
                ss << ",";
            }
            ss << "\n";
        }
        
        ss << "      ]\n";
        ss << "    }";
        
        if (count < registry_.size() - 1) {
            ss << ",";
        }
        ss << "\n";
        count++;
    }
    
    ss << "  ]\n";
    ss << "}";
    
    return ss.str();
}

} // namespace CJMOD
} // namespace CHTL
