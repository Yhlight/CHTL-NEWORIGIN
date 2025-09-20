#include "StyleEnhancer.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <cmath>

StyleEnhancer::StyleEnhancer() 
    : autoAddClass_(true)
    , autoAddId_(true)
    , debugMode_(false) {
}

bool StyleEnhancer::processStyleBlock(CHTLNode& element, const std::string& styleContent) {
    if (debugMode_) {
        std::cout << "处理局部样式块: " << styleContent << std::endl;
    }
    
    // 解析样式内容
    std::vector<StyleProperty> properties;
    if (!parseStyleContent(styleContent, properties)) {
        return false;
    }
    
    // 自动化类名/ID添加
    if (autoAddClass_ || autoAddId_) {
        if (!addAutoClassId(element, properties)) {
            return false;
        }
    }
    
    // 处理属性运算和条件表达式
    for (auto& prop : properties) {
        if (prop.value.find('+') != std::string::npos || 
            prop.value.find('-') != std::string::npos ||
            prop.value.find('*') != std::string::npos ||
            prop.value.find('/') != std::string::npos) {
            // 属性运算
            auto result = evaluatePropertyOperation(prop.value);
            if (result.success) {
                prop.value = result.result;
            } else {
                reportError("属性运算失败: " + result.error, prop.line, prop.column);
                return false;
            }
        } else if (prop.value.find('?') != std::string::npos) {
            // 条件表达式
            prop.value = processConditionalExpression(prop.value, element);
        }
    }
    
    // 生成全局样式
    generateGlobalStylesFromProperties(properties, element);
    
    return true;
}

bool StyleEnhancer::addAutoClassId(CHTLNode& element, const std::vector<StyleProperty>& properties) {
    std::string autoClass;
    std::string autoId;
    
    // 查找第一个类选择器和ID选择器
    for (const auto& prop : properties) {
        if (prop.type == StylePropertyType::CLASS && autoClass.empty()) {
            autoClass = prop.name;
        } else if (prop.type == StylePropertyType::ID && autoId.empty()) {
            autoId = prop.name;
        }
    }
    
    // 添加类名
    if (!autoClass.empty() && autoAddClass_) {
        std::string existingClass = element.getAttribute("class");
        if (existingClass.empty()) {
            element.setAttribute("class", autoClass);
        } else {
            element.setAttribute("class", existingClass + " " + autoClass);
        }
    }
    
    // 添加ID
    if (!autoId.empty() && autoAddId_) {
        std::string existingId = element.getAttribute("id");
        if (existingId.empty()) {
            element.setAttribute("id", autoId);
        } else {
            reportWarning("元素已有ID，跳过自动添加: " + autoId);
        }
    }
    
    return true;
}

std::string StyleEnhancer::processContextReference(const std::string& content, const CHTLNode& element) {
    std::string result = content;
    
    // 处理 & 引用
    size_t pos = 0;
    while ((pos = result.find("&", pos)) != std::string::npos) {
        // 查找上下文中的类名或ID
        std::string classAttr = element.getAttribute("class");
        std::string idAttr = element.getAttribute("id");
        
        std::string replacement;
        if (!classAttr.empty()) {
            replacement = "." + classAttr;
        } else if (!idAttr.empty()) {
            replacement = "#" + idAttr;
        } else {
            replacement = element.getValue(); // 使用元素名
        }
        
        result.replace(pos, 1, replacement);
        pos += replacement.length();
    }
    
    return result;
}

PropertyOperationResult StyleEnhancer::evaluatePropertyOperation(const std::string& expression) {
    // 简化的属性运算实现
    // 这里应该实现完整的算术表达式解析
    
    // 查找运算符
    std::vector<size_t> operatorPositions;
    for (size_t i = 0; i < expression.length(); i++) {
        char c = expression[i];
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%') {
            operatorPositions.push_back(i);
        }
    }
    
    if (operatorPositions.empty()) {
        return PropertyOperationResult(true, expression, "");
    }
    
    // 处理第一个运算符
    size_t opPos = operatorPositions[0];
    char op = expression[opPos];
    
    std::string left = expression.substr(0, opPos);
    std::string right = expression.substr(opPos + 1);
    
    // 去除空格
    left.erase(std::remove(left.begin(), left.end(), ' '), left.end());
    right.erase(std::remove(right.begin(), right.end(), ' '), right.end());
    
    // 提取数字和单位
    std::string leftUnit = extractUnit(left);
    std::string rightUnit = extractUnit(right);
    std::string leftNum = extractNumber(left);
    std::string rightNum = extractNumber(right);
    
    if (leftNum.empty() || rightNum.empty()) {
        return PropertyOperationResult(false, "", "无法解析数字");
    }
    
    // 单位检查
    if (!leftUnit.empty() && !rightUnit.empty() && !isSameUnit(leftUnit, rightUnit)) {
        return PropertyOperationResult(false, "", "单位不匹配: " + leftUnit + " vs " + rightUnit);
    }
    
    // 执行运算
    double leftVal = std::stod(leftNum);
    double rightVal = std::stod(rightNum);
    double result = 0;
    
    switch (op) {
        case '+': result = leftVal + rightVal; break;
        case '-': result = leftVal - rightVal; break;
        case '*': result = leftVal * rightVal; break;
        case '/': result = rightVal != 0 ? leftVal / rightVal : 0; break;
        case '%': result = rightVal != 0 ? std::fmod(leftVal, rightVal) : 0; break;
        default: return PropertyOperationResult(false, "", "未知运算符: " + std::string(1, op));
    }
    
    // 生成结果
    std::string resultUnit = leftUnit.empty() ? rightUnit : leftUnit;
    std::string resultStr = std::to_string(result);
    
    // 去除尾随零
    if (resultStr.find('.') != std::string::npos) {
        resultStr.erase(resultStr.find_last_not_of('0') + 1, std::string::npos);
        resultStr.erase(resultStr.find_last_not_of('.') + 1, std::string::npos);
    }
    
    return PropertyOperationResult(true, resultStr + resultUnit, "");
}

std::string StyleEnhancer::processPropertyReference(const std::string& reference, const CHTLNode& element) {
    // 处理引用属性，如 box.width
    // 这里需要实现选择器解析和属性引用
    
    if (reference.find('.') == std::string::npos) {
        return reference;
    }
    
    // 简化实现：直接返回引用
    return reference;
}

std::string StyleEnhancer::processConditionalExpression(const std::string& expression, const CHTLNode& element) {
    // 处理条件表达式，如 width > 50px ? "red" : "blue"
    // 这里需要实现完整的条件表达式解析
    
    size_t questionPos = expression.find('?');
    if (questionPos == std::string::npos) {
        return expression;
    }
    
    std::string condition = expression.substr(0, questionPos);
    std::string remaining = expression.substr(questionPos + 1);
    
    // 查找冒号
    size_t colonPos = remaining.find(':');
    if (colonPos == std::string::npos) {
        return expression;
    }
    
    std::string trueValue = remaining.substr(0, colonPos);
    std::string falseValue = remaining.substr(colonPos + 1);
    
    // 去除空格
    condition.erase(std::remove(condition.begin(), condition.end(), ' '), condition.end());
    trueValue.erase(std::remove(trueValue.begin(), trueValue.end(), ' '), trueValue.end());
    falseValue.erase(std::remove(falseValue.begin(), falseValue.end(), ' '), falseValue.end());
    
    // 简化实现：总是返回真值
    return trueValue;
}

std::string StyleEnhancer::generateGlobalStyles() const {
    std::string result;
    for (const auto& style : globalStyles_) {
        result += style + "\n";
    }
    return result;
}

PropertyOperationResult StyleEnhancer::evaluateArithmeticExpression(const std::string& expression) {
    return evaluatePropertyOperation(expression);
}

PropertyOperationResult StyleEnhancer::evaluateComparisonExpression(const std::string& expression) {
    // 实现比较表达式
    return PropertyOperationResult(true, expression, "");
}

PropertyOperationResult StyleEnhancer::evaluateLogicalExpression(const std::string& expression) {
    // 实现逻辑表达式
    return PropertyOperationResult(true, expression, "");
}

std::string StyleEnhancer::extractUnit(const std::string& value) {
    std::regex unitRegex(R"((\d+(?:\.\d+)?)([a-zA-Z%]+)?)");
    std::smatch match;
    
    if (std::regex_match(value, match, unitRegex)) {
        return match[2].str();
    }
    
    return "";
}

std::string StyleEnhancer::extractNumber(const std::string& value) {
    std::regex numberRegex(R"((\d+(?:\.\d+)?))");
    std::smatch match;
    
    if (std::regex_search(value, match, numberRegex)) {
        return match[1].str();
    }
    
    return "";
}

bool StyleEnhancer::isSameUnit(const std::string& unit1, const std::string& unit2) {
    if (unit1.empty() && unit2.empty()) return true;
    if (unit1.empty() || unit2.empty()) return false;
    return unit1 == unit2;
}

std::string StyleEnhancer::convertPercentage(const std::string& value) {
    // 百分比转换
    return value;
}

std::vector<std::string> StyleEnhancer::extractNumbers(const std::string& value) {
    std::vector<std::string> numbers;
    std::regex numberRegex(R"(\d+(?:\.\d+)?)");
    std::sregex_iterator begin(value.begin(), value.end(), numberRegex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        numbers.push_back(it->str());
    }
    
    return numbers;
}

std::string StyleEnhancer::generateSelector(const std::string& selector, const CHTLNode& element) {
    return processContextReference(selector, element);
}

std::string StyleEnhancer::resolveContextReference(const std::string& reference, const CHTLNode& element) {
    return processContextReference(reference, element);
}

void StyleEnhancer::reportError(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "样式增强器错误";
    if (line > 0) ss << " [" << line << ":" << column << "]";
    ss << ": " << message;
    
    std::cerr << ss.str() << std::endl;
    errors_.push_back(ss.str());
}

void StyleEnhancer::reportWarning(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "样式增强器警告";
    if (line > 0) ss << " [" << line << ":" << column << "]";
    ss << ": " << message;
    
    std::cout << ss.str() << std::endl;
    warnings_.push_back(ss.str());
}

// 辅助方法：解析样式内容
bool StyleEnhancer::parseStyleContent(const std::string& content, std::vector<StyleProperty>& properties) {
    // 简化的样式内容解析
    // 这里应该实现完整的CSS解析
    
    std::istringstream stream(content);
    std::string line;
    size_t lineNum = 1;
    
    while (std::getline(stream, line)) {
        // 去除前后空格
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty() || line[0] == '/' || line[0] == '*') {
            lineNum++;
            continue;
        }
        
        // 查找冒号
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            lineNum++;
            continue;
        }
        
        std::string name = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        
        // 去除空格
        name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
        value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
        
        // 去除分号
        if (!value.empty() && value.back() == ';') {
            value.pop_back();
        }
        
        // 确定属性类型
        StylePropertyType type = StylePropertyType::INLINE;
        if (name[0] == '.') {
            type = StylePropertyType::CLASS;
            name = name.substr(1);
        } else if (name[0] == '#') {
            type = StylePropertyType::ID;
            name = name.substr(1);
        } else if (name.find(':') != std::string::npos) {
            type = StylePropertyType::PSEUDO_CLASS;
        } else if (name.find("::") != std::string::npos) {
            type = StylePropertyType::PSEUDO_ELEMENT;
        }
        
        properties.emplace_back(name, value, type, lineNum, 0);
        lineNum++;
    }
    
    return true;
}

// 辅助方法：从属性生成全局样式
void StyleEnhancer::generateGlobalStylesFromProperties(const std::vector<StyleProperty>& properties, const CHTLNode& element) {
    std::string elementClass = element.getAttribute("class");
    std::string elementId = element.getAttribute("id");
    
    for (const auto& prop : properties) {
        if (prop.type == StylePropertyType::CLASS || prop.type == StylePropertyType::ID) {
            std::string selector;
            if (prop.type == StylePropertyType::CLASS) {
                selector = "." + prop.name;
            } else {
                selector = "#" + prop.name;
            }
            
            std::string style = selector + " {\n";
            style += "    " + prop.name + ": " + prop.value + ";\n";
            style += "}\n";
            
            globalStyles_.push_back(style);
        }
    }
}