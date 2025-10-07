/**
 * @file ConditionalCSSGenerator.h
 * @brief 条件渲染CSS生成器
 * 
 * 将CHTL条件表达式转换为CSS @media查询或CSS变量
 */

#ifndef CHTL_CONDITIONAL_CSS_GENERATOR_H
#define CHTL_CONDITIONAL_CSS_GENERATOR_H

#include "../../Common.h"

namespace CHTL {

/**
 * @brief 条件CSS生成器
 */
class ConditionalCSSGenerator {
public:
    /**
     * @brief 将CHTL条件转换为CSS @media查询
     * @param condition CHTL条件表达式（如 "width > 768px"）
     * @param targetSelector 目标选择器
     * @param styles CSS属性列表
     * @return CSS @media查询字符串
     */
    static String generateMediaQuery(const String& condition, 
                                      const String& targetSelector,
                                      const Vector<Pair<String, String>>& styles);
    
    /**
     * @brief 解析条件表达式
     * @param condition 条件字符串
     * @return {property, operator, value}
     */
    static Tuple<String, String, String> parseCondition(const String& condition);
    
    /**
     * @brief 将CHTL条件转换为CSS媒体特性
     * @param property 属性（如 "width"）
     * @param op 运算符（如 ">"）
     * @param value 值（如 "768px"）
     * @return CSS媒体特性（如 "min-width: 768px"）
     */
    static String toMediaFeature(const String& property, const String& op, const String& value);
    
    /**
     * @brief 检查是否可以转换为@media
     * @param condition 条件表达式
     * @return true如果可以转换
     */
    static bool canConvertToMedia(const String& condition);
    
    /**
     * @brief 生成CSS规则
     * @param selector 选择器
     * @param styles 样式列表
     * @return CSS规则字符串
     */
    static String generateCSSRule(const String& selector, const Vector<Pair<String, String>>& styles);

private:
    /**
     * @brief 提取数字和单位
     * @param value 值字符串（如 "768px"）
     * @return {number, unit}
     */
    static Pair<String, String> extractNumberAndUnit(const String& value);
    
    /**
     * @brief 检查是否是支持的媒体属性
     * @param property 属性名
     * @return true如果支持
     */
    static bool isSupportedMediaProperty(const String& property);
};

} // namespace CHTL

#endif // CHTL_CONDITIONAL_CSS_GENERATOR_H
