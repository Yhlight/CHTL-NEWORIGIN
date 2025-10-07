/**
 * @file ConditionalCSSGenerator.cpp
 * @brief 条件CSS生成器实现
 */

#include "ConditionalCSSGenerator.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <sstream>
#include <regex>

namespace CHTL {

String ConditionalCSSGenerator::generateMediaQuery(const String& condition,
                                                     const String& targetSelector,
                                                     const Vector<Pair<String, String>>& styles) {
    if (!canConvertToMedia(condition)) {
        // 无法转换为@media，返回注释
        std::stringstream ss;
        ss << "/* Conditional: " << condition << " */\n";
        for (const auto& [prop, value] : styles) {
            ss << "  /* " << prop << ": " << value << "; */\n";
        }
        ss << "\n";
        return ss.str();
    }
    
    // 解析条件
    auto [property, op, value] = parseCondition(condition);
    
    // 转换为媒体特性
    String mediaFeature = toMediaFeature(property, op, value);
    
    if (mediaFeature.empty()) {
        // 转换失败，返回注释
        std::stringstream ss;
        ss << "/* Conditional: " << condition << " (cannot convert to @media) */\n";
        return ss.str();
    }
    
    // 生成@media查询
    std::stringstream ss;
    ss << "@media (" << mediaFeature << ") {\n";
    ss << "  " << targetSelector << " {\n";
    
    for (const auto& [prop, value] : styles) {
        ss << "    " << prop << ": " << value << ";\n";
    }
    
    ss << "  }\n";
    ss << "}\n\n";
    
    return ss.str();
}

Tuple<String, String, String> ConditionalCSSGenerator::parseCondition(const String& condition) {
    // 支持的运算符（按长度排序，先匹配长的）
    static const Vector<String> operators = {">=", "<=", "==", "!=", ">", "<"};
    
    for (const auto& op : operators) {
        size_t pos = condition.find(op);
        if (pos != String::npos) {
            String left = Util::StringUtil::trim(condition.substr(0, pos));
            String right = Util::StringUtil::trim(condition.substr(pos + op.length()));
            return std::make_tuple(left, op, right);
        }
    }
    
    return std::make_tuple("", "", "");
}

String ConditionalCSSGenerator::toMediaFeature(const String& property, 
                                                 const String& op, 
                                                 const String& value) {
    // 处理元素属性引用（如 html.width）
    String actualProperty = property;
    if (property.find('.') != String::npos) {
        // 提取属性名（忽略选择器部分）
        size_t dotPos = property.rfind('.');
        if (dotPos != String::npos) {
            actualProperty = property.substr(dotPos + 1);
        }
    }
    
    // 检查是否是支持的媒体属性
    if (!isSupportedMediaProperty(actualProperty)) {
        return "";  // 不支持
    }
    
    // 转换运算符到媒体特性
    if (op == ">") {
        return "min-" + actualProperty + ": " + value;
    } else if (op == ">=") {
        return "min-" + actualProperty + ": " + value;
    } else if (op == "<") {
        return "max-" + actualProperty + ": " + value;
    } else if (op == "<=") {
        return "max-" + actualProperty + ": " + value;
    } else if (op == "==") {
        return actualProperty + ": " + value;
    } else {
        return "";  // 不支持的运算符
    }
}

bool ConditionalCSSGenerator::canConvertToMedia(const String& condition) {
    // 检查是否包含逻辑运算符（&&、||）
    if (condition.find("&&") != String::npos || 
        condition.find("||") != String::npos) {
        return false;  // 暂不支持复杂条件
    }
    
    // 解析条件
    auto [property, op, value] = parseCondition(condition);
    
    if (property.empty() || op.empty() || value.empty()) {
        return false;
    }
    
    // 提取实际属性名
    String actualProperty = property;
    if (property.find('.') != String::npos) {
        size_t dotPos = property.rfind('.');
        if (dotPos != String::npos) {
            actualProperty = property.substr(dotPos + 1);
        }
    }
    
    return isSupportedMediaProperty(actualProperty);
}

String ConditionalCSSGenerator::generateCSSRule(const String& selector, 
                                                  const Vector<Pair<String, String>>& styles) {
    std::stringstream ss;
    ss << selector << " {\n";
    
    for (const auto& [prop, value] : styles) {
        ss << "  " << prop << ": " << value << ";\n";
    }
    
    ss << "}\n";
    
    return ss.str();
}

Pair<String, String> ConditionalCSSGenerator::extractNumberAndUnit(const String& value) {
    std::regex numberRegex("([0-9.]+)(.*)");
    std::smatch match;
    
    if (std::regex_match(value, match, numberRegex)) {
        return {match[1].str(), match[2].str()};
    }
    
    return {"", ""};
}

bool ConditionalCSSGenerator::isSupportedMediaProperty(const String& property) {
    // 支持的CSS媒体特性
    static const Vector<String> supportedProps = {
        "width", "height",
        "min-width", "max-width",
        "min-height", "max-height",
        "aspect-ratio",
        "orientation",
        "resolution"
    };
    
    for (const auto& prop : supportedProps) {
        if (property == prop) {
            return true;
        }
    }
    
    return false;
}

} // namespace CHTL
