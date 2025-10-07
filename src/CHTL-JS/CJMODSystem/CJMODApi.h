#ifndef CHTL_CJMOD_API_H
#define CHTL_CJMOD_API_H

#include "../../Common.h"
#include <functional>
#include <variant>

namespace CHTL {
namespace CJMOD {

// ========================================
// 原子参数 - AtomArg
// ========================================
class AtomArg {
public:
    enum class Type {
        Placeholder,        // $
        Optional,           // $?
        Required,           // $!
        Unordered,          // $_
        Variadic,           // ...
        Literal,            // 字面量
        Keyword             // 关键字
    };
    
    AtomArg(Type type, const String& pattern);
    
    // 绑定值获取函数
    using ValueGetter = std::function<String(const String&)>;
    void bind(ValueGetter getter);
    
    // 填充值
    void fillValue(const String& value);
    String getValue() const { return value_; }
    
    // 获取类型
    Type getType() const { return type_; }
    String getPattern() const { return pattern_; }
    
    // 检查是否匹配
    bool matches(const String& input) const;
    
private:
    Type type_;
    String pattern_;
    String value_;
    ValueGetter getter_;
};

// ========================================
// 参数列表 - Arg
// ========================================
class Arg {
public:
    Arg() = default;
    Arg(const Vector<AtomArg>& atoms);
    
    // 添加原子参数
    void add(const AtomArg& atom);
    
    // 绑定特定占位符的获取函数
    void bind(const String& placeholder, AtomArg::ValueGetter getter);
    
    // 匹配参数并返回值
    String match(const String& placeholder, AtomArg::ValueGetter transformer);
    
    // 填充参数值
    void fillValue(const Arg& values);
    void fillValue(const Vector<String>& values);
    
    // 转换为最终代码
    void transform(const String& pattern);
    String getTransformed() const { return transformed_; }
    
    // 访问原子参数
    AtomArg& operator[](size_t index);
    const AtomArg& operator[](size_t index) const;
    
    // 获取原子参数数量
    size_t size() const { return atoms_.size(); }
    
    // 获取所有原子参数
    Vector<AtomArg> getAtoms() const { return atoms_; }
    
    // 获取所有值
    Vector<String> getValues() const {
        Vector<String> values;
        for (const auto& atom : atoms_) {
            values.push_back(atom.getValue());
        }
        return values;
    }
    
    // 打印（调试用）
    void print() const;
    
private:
    Vector<AtomArg> atoms_;
    String transformed_;
    HashMap<String, int> matchCounters_;  // 记录每个占位符的匹配次数
};

// ========================================
// 语法分析器 - Syntax
// ========================================
class Syntax {
public:
    // 分析语法模式，返回Arg对象
    static Arg analyze(const String& pattern);
    
    // 类型判断
    static bool isObject(const String& code);
    static bool isFunction(const String& code);
    static bool isArray(const String& code);
    static bool isCHTLJSFunction(const String& code);
    
private:
    // 解析占位符类型
    static AtomArg::Type parsePlaceholderType(const String& placeholder);
    
    // 分词
    static Vector<String> tokenize(const String& pattern);
};

// ========================================
// CJMOD 扫描器 - CJMODScanner
// ========================================
class CJMODScanner {
public:
    // 扫描代码片段，提取匹配的参数
    static Arg scan(const Arg& pattern, const String& keyword);
    static Arg scan(const Arg& pattern, const String& keyword, const String& code);
    
    // 设置要扫描的代码
    static void setCode(const String& code);
    
private:
    static String currentCode_;
    
    // 双指针扫描算法
    static Arg scanWithTwoPointers(const Arg& pattern, const String& keyword, const String& code);
    
    // 动态感知扫描算法
    static Arg scanWithDynamicAwareness(const Arg& pattern, const String& keyword, const String& code);
    
    // 提取片段
    static String extractFragment(const String& code, size_t start, size_t end);
    
    // 跳过空白
    static size_t skipWhitespace(const String& code, size_t pos);
};

// ========================================
// CJMOD 生成器 - CJMODGenerator
// ========================================
class CJMODGenerator {
public:
    // 导出最终的JS代码
    static String exportResult(const Arg& arg);
    
    // 设置生成选项
    static void setPrettyPrint(bool enable);
    static void setWrapIIFE(bool enable);
    
private:
    static bool prettyPrint_;
    static bool wrapIIFE_;
};

// ========================================
// CHTL JS 函数 - CHTLJSFunction
// ========================================
class CHTLJSFunction {
public:
    // 快速创建CHTL JS函数
    static Arg createCHTLJSFunction(const String& functionDef);
    
    // 为函数绑定虚对象支持
    static void bindVirtualObject(const String& functionName);
    
    // 检查是否已绑定虚对象
    static bool hasVirtualObject(const String& functionName);
    
    // 获取虚对象的所有键
    static Vector<String> getVirtualObjectKeys(const String& functionName);
    
private:
    static HashMap<String, Vector<String>> virtualObjects_;
    
    // 解析函数定义
    static Arg parseFunctionDefinition(const String& def);
    
    // 提取函数键
    static Vector<String> extractKeys(const String& def);
};

// ========================================
// 模块注册器 - CJMODRegistry
// ========================================
class CJMODRegistry {
public:
    struct SyntaxInfo {
        String pattern;         // 语法模式
        String description;     // 描述
        Vector<String> examples;// 示例
    };
    
    // 注册CJMOD语法
    static void registerSyntax(const String& name, const SyntaxInfo& info);
    
    // 获取所有已注册的语法
    static Vector<String> getAllSyntaxNames();
    
    // 获取语法信息
    static Optional<SyntaxInfo> getSyntaxInfo(const String& name);
    
    // 为VSCode生成JSON清单
    static String generateJSONManifest();
    
private:
    static HashMap<String, SyntaxInfo> registry_;
};

} // namespace CJMOD
} // namespace CHTL

#endif // CHTL_CJMOD_API_H
