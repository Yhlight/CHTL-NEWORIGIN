#!/usr/bin/env python3
"""
高级样式功能实现
基于CHTL.md规范实现高级样式功能：
- 属性运算 (arithmetic operations)
- 属性条件表达式 (conditional expressions)
- 引用属性 (property references)
- 动态条件渲染 (dynamic conditional rendering)
- 响应式值 (responsive values)
"""

import re
import math
from typing import Dict, List, Tuple, Optional, Union, Any
from dataclasses import dataclass, field
from enum import Enum

@dataclass
class PropertyValue:
    """属性值"""
    value: str
    unit: Optional[str] = None
    is_numeric: bool = False
    numeric_value: Optional[float] = None

@dataclass
class PropertyReference:
    """属性引用"""
    selector: str
    property_name: str
    is_relative: bool = False

@dataclass
class ConditionalExpression:
    """条件表达式"""
    condition: str
    true_value: str
    false_value: Optional[str] = None
    is_chain: bool = False

class StylePropertyProcessor:
    """样式属性处理器"""
    
    def __init__(self):
        self.property_references: Dict[str, Any] = {}
        self.responsive_values: Dict[str, str] = {}
    
    def process_property_value(self, value: str) -> PropertyValue:
        """处理属性值"""
        # 处理响应式值 $...$
        if value.startswith('$') and value.endswith('$'):
            var_name = value[1:-1]
            return PropertyValue(value=var_name, is_numeric=False)
        
        # 处理引用属性 selector.property
        if '.' in value and not value.startswith('.'):
            parts = value.split('.')
            if len(parts) == 2:
                selector, prop_name = parts
                return PropertyValue(
                    value=value,
                    is_numeric=False
                )
        
        # 处理数字值
        numeric_match = re.match(r'^([+-]?\d*\.?\d+)([a-zA-Z%]*)$', value.strip())
        if numeric_match:
            num_value = float(numeric_match.group(1))
            unit = numeric_match.group(2) if numeric_match.group(2) else None
            return PropertyValue(
                value=value,
                unit=unit,
                is_numeric=True,
                numeric_value=num_value
            )
        
        return PropertyValue(value=value, is_numeric=False)
    
    def process_arithmetic_expression(self, expression: str) -> str:
        """处理算术表达式"""
        # 支持的运算符: +, -, *, /, %, **
        operators = ['**', '*', '/', '%', '+', '-']
        
        # 简单的表达式解析和计算
        try:
            # 处理单位
            expression = self._normalize_units(expression)
            
            # 计算表达式
            result = self._evaluate_expression(expression)
            
            return str(result)
        except Exception as e:
            print(f"算术表达式计算错误: {e}")
            return expression
    
    def _normalize_units(self, expression: str) -> str:
        """标准化单位"""
        # 处理百分比转换
        expression = re.sub(r'(\d+)%', r'\1/100', expression)
        
        # 处理单位合并
        # 这里可以实现更复杂的单位处理逻辑
        return expression
    
    def _evaluate_expression(self, expression: str) -> float:
        """计算表达式"""
        # 简单的表达式计算器
        # 注意：这里只处理基本的算术运算，实际实现需要更复杂的解析器
        
        # 处理幂运算
        while '**' in expression:
            pattern = r'(\d+(?:\.\d+)?)\s*\*\*\s*(\d+(?:\.\d+)?)'
            match = re.search(pattern, expression)
            if match:
                base = float(match.group(1))
                exp = float(match.group(2))
                result = base ** exp
                expression = expression.replace(match.group(0), str(result))
            else:
                break
        
        # 处理乘除模运算
        while re.search(r'[\*/%]', expression):
            pattern = r'(\d+(?:\.\d+)?)\s*([\*/%])\s*(\d+(?:\.\d+)?)'
            match = re.search(pattern, expression)
            if match:
                left = float(match.group(1))
                op = match.group(2)
                right = float(match.group(3))
                
                if op == '*':
                    result = left * right
                elif op == '/':
                    result = left / right
                elif op == '%':
                    result = left % right
                
                expression = expression.replace(match.group(0), str(result))
            else:
                break
        
        # 处理加减运算
        while re.search(r'[+-]', expression):
            pattern = r'(\d+(?:\.\d+)?)\s*([+-])\s*(\d+(?:\.\d+)?)'
            match = re.search(pattern, expression)
            if match:
                left = float(match.group(1))
                op = match.group(2)
                right = float(match.group(3))
                
                if op == '+':
                    result = left + right
                elif op == '-':
                    result = left - right
                
                expression = expression.replace(match.group(0), str(result))
            else:
                break
        
        return float(expression)
    
    def process_conditional_expression(self, expression: str) -> ConditionalExpression:
        """处理条件表达式"""
        # 解析条件表达式: condition ? true_value : false_value
        # 支持链式条件: condition1 ? value1, condition2 ? value2, condition3 ? value3 : default_value
        
        # 检查是否是链式条件
        if ',' in expression and '?' in expression:
            return self._parse_chain_conditional(expression)
        else:
            return self._parse_simple_conditional(expression)
    
    def _parse_simple_conditional(self, expression: str) -> ConditionalExpression:
        """解析简单条件表达式"""
        # 格式: condition ? true_value : false_value
        if '?' in expression:
            parts = expression.split('?', 1)
            condition = parts[0].strip()
            
            if ':' in parts[1]:
                true_false = parts[1].split(':', 1)
                true_value = true_false[0].strip()
                false_value = true_false[1].strip()
            else:
                true_value = parts[1].strip()
                false_value = None
            
            return ConditionalExpression(
                condition=condition,
                true_value=true_value,
                false_value=false_value,
                is_chain=False
            )
        else:
            # 不是条件表达式
            return ConditionalExpression(
                condition="",
                true_value=expression,
                false_value=None,
                is_chain=False
            )
    
    def _parse_chain_conditional(self, expression: str) -> ConditionalExpression:
        """解析链式条件表达式"""
        # 格式: condition1 ? value1, condition2 ? value2, condition3 ? value3 : default_value
        conditions = []
        parts = expression.split(',')
        
        for part in parts:
            part = part.strip()
            if '?' in part:
                cond_value = part.split('?', 1)
                condition = cond_value[0].strip()
                value = cond_value[1].strip()
                conditions.append((condition, value))
            else:
                # 默认值
                conditions.append(("", part))
        
        return ConditionalExpression(
            condition=conditions[0][0] if conditions else "",
            true_value=conditions[0][1] if conditions else "",
            false_value=conditions[-1][1] if len(conditions) > 1 else None,
            is_chain=True
        )
    
    def process_property_reference(self, reference: str) -> PropertyReference:
        """处理属性引用"""
        # 格式: selector.property
        if '.' in reference:
            parts = reference.split('.')
            if len(parts) == 2:
                selector = parts[0].strip()
                property_name = parts[1].strip()
                
                return PropertyReference(
                    selector=selector,
                    property_name=property_name,
                    is_relative=selector.startswith('&')
                )
        
        return PropertyReference(
            selector="",
            property_name=reference,
            is_relative=False
        )
    
    def generate_css_with_advanced_features(self, properties: Dict[str, str]) -> str:
        """生成包含高级功能的CSS"""
        css_lines = []
        
        for prop_name, prop_value in properties.items():
            # 处理属性值
            processed_value = self._process_property_value(prop_value)
            
            # 处理算术表达式
            if self._contains_arithmetic(processed_value.value):
                processed_value.value = self.process_arithmetic_expression(processed_value.value)
            
            # 处理条件表达式
            if '?' in processed_value.value:
                conditional = self.process_conditional_expression(processed_value.value)
                css_value = self._generate_conditional_css(conditional)
            else:
                css_value = processed_value.value
            
            css_lines.append(f"{prop_name}: {css_value};")
        
        return '\n'.join(css_lines)
    
    def _process_property_value(self, value: str) -> PropertyValue:
        """处理单个属性值"""
        return self.process_property_value(value)
    
    def _contains_arithmetic(self, value: str) -> bool:
        """检查是否包含算术表达式"""
        arithmetic_ops = ['+', '-', '*', '/', '%', '**']
        return any(op in value for op in arithmetic_ops)
    
    def _generate_conditional_css(self, conditional: ConditionalExpression) -> str:
        """生成条件CSS"""
        if conditional.is_chain:
            # 链式条件需要JavaScript处理
            return f"/* 链式条件: {conditional.condition} ? {conditional.true_value} : {conditional.false_value} */"
        else:
            # 简单条件可以用CSS calc()处理
            if conditional.false_value:
                return f"calc({conditional.condition} ? {conditional.true_value} : {conditional.false_value})"
            else:
                return f"calc({conditional.condition} ? {conditional.true_value} : 0)"
    
    def process_responsive_values(self, content: str) -> str:
        """处理响应式值"""
        # 处理 $...$ 响应式值
        pattern = r'\$([^$]+)\$'
        
        def replace_responsive_value(match):
            var_name = match.group(1)
            if var_name in self.responsive_values:
                return self.responsive_values[var_name]
            else:
                return f"var(--{var_name})"
        
        return re.sub(pattern, replace_responsive_value, content)

def main():
    """测试高级样式功能"""
    processor = StylePropertyProcessor()
    
    print("🧪 测试高级样式功能")
    print("=" * 50)
    
    # 测试属性运算
    print("\n📊 测试属性运算:")
    arithmetic_tests = [
        "100px + 20px",
        "50% * 2",
        "200px / 4",
        "10px ** 2",
        "100px + 50%",
        "2 * 3 + 4",
        "10 - 5 * 2"
    ]
    
    for expr in arithmetic_tests:
        result = processor.process_arithmetic_expression(expr)
        print(f"  {expr} = {result}")
    
    # 测试条件表达式
    print("\n🔀 测试条件表达式:")
    conditional_tests = [
        "width > 50px ? red : blue",
        "height < 100px ? green",
        "width > 50px ? red, width > 100px ? green, height < 100px ? purple : pink"
    ]
    
    for expr in conditional_tests:
        conditional = processor.process_conditional_expression(expr)
        print(f"  {expr}")
        print(f"    条件: {conditional.condition}")
        print(f"    真值: {conditional.true_value}")
        print(f"    假值: {conditional.false_value}")
        print(f"    链式: {conditional.is_chain}")
    
    # 测试属性引用
    print("\n🔗 测试属性引用:")
    reference_tests = [
        "box.width",
        ".box.height",
        "#container.padding",
        "&.hover.opacity"
    ]
    
    for ref in reference_tests:
        reference = processor.process_property_reference(ref)
        print(f"  {ref} -> 选择器: {reference.selector}, 属性: {reference.property_name}")
    
    # 测试响应式值
    print("\n📱 测试响应式值:")
    processor.responsive_values = {
        'boxWidth': '200px',
        'boxHeight': '100px',
        'boxColor': 'blue'
    }
    
    responsive_content = "width: $boxWidth$; height: $boxHeight$; color: $boxColor$;"
    processed_content = processor.process_responsive_values(responsive_content)
    print(f"  原始: {responsive_content}")
    print(f"  处理: {processed_content}")
    
    # 测试完整样式生成
    print("\n🎨 测试完整样式生成:")
    advanced_properties = {
        'width': '100px + 20px',
        'height': '50px * 2',
        'background-color': 'width > 100px ? red : blue',
        'padding': '$boxPadding$',
        'margin': '10px + 5px'
    }
    
    css = processor.generate_css_with_advanced_features(advanced_properties)
    print("生成的CSS:")
    print(css)
    
    print("\n✅ 高级样式功能测试完成")

if __name__ == "__main__":
    main()