#!/usr/bin/env python3
"""
模板特例化实现
基于CHTL.md规范实现模板特例化功能：
- 删除属性 (delete property)
- 删除继承 (delete inheritance)
- 样式组特例化
- 元素特例化
- 索引访问
- 插入元素
- 删除元素
"""

import re
from typing import Dict, List, Tuple, Optional, Union, Any
from dataclasses import dataclass, field
from enum import Enum

@dataclass
class TemplateSpecialization:
    """模板特例化信息"""
    template_name: str
    template_type: str  # 'style', 'element', 'var'
    delete_properties: List[str] = field(default_factory=list)
    delete_inheritance: List[str] = field(default_factory=list)
    add_properties: Dict[str, Any] = field(default_factory=dict)
    index_access: Optional[int] = None
    insert_operations: List[Dict[str, Any]] = field(default_factory=list)
    delete_elements: List[str] = field(default_factory=list)

class TemplateSpecializationProcessor:
    """模板特例化处理器"""
    
    def __init__(self):
        self.specializations: Dict[str, TemplateSpecialization] = {}
    
    def process_style_specialization(self, template_name: str, content: str) -> Dict[str, Any]:
        """处理样式组特例化"""
        specialization = TemplateSpecialization(
            template_name=template_name,
            template_type='style'
        )
        
        # 解析删除属性
        delete_props_pattern = r'delete\s+([^;]+);'
        delete_matches = re.findall(delete_props_pattern, content)
        for match in delete_matches:
            props = [prop.strip() for prop in match.split(',')]
            specialization.delete_properties.extend(props)
        
        # 解析删除继承
        delete_inherit_pattern = r'delete\s+@Style\s+([^;]+);'
        delete_inherit_matches = re.findall(delete_inherit_pattern, content)
        for match in delete_inherit_matches:
            specialization.delete_inheritance.append(match.strip())
        
        # 解析添加属性
        add_props_pattern = r'([a-zA-Z-]+)\s*:\s*([^;]+);'
        add_matches = re.findall(add_props_pattern, content)
        for prop_name, prop_value in add_matches:
            specialization.add_properties[prop_name.strip()] = prop_value.strip()
        
        self.specializations[template_name] = specialization
        return self._apply_style_specialization(specialization)
    
    def process_element_specialization(self, template_name: str, content: str) -> Dict[str, Any]:
        """处理元素特例化"""
        specialization = TemplateSpecialization(
            template_name=template_name,
            template_type='element'
        )
        
        # 解析索引访问
        index_pattern = r'(\w+)\[(\d+)\]'
        index_matches = re.findall(index_pattern, content)
        for element_name, index in index_matches:
            if element_name == template_name:
                specialization.index_access = int(index)
        
        # 解析插入操作
        insert_pattern = r'insert\s+(after|before|replace|at top|at bottom)\s+(\w+)(?:\[(\d+)\])?\s*\{([^}]+)\}'
        insert_matches = re.findall(insert_pattern, content)
        for position, target, index, content in insert_matches:
            specialization.insert_operations.append({
                'position': position,
                'target': target,
                'index': int(index) if index else None,
                'content': content.strip()
            })
        
        # 解析删除元素
        delete_pattern = r'delete\s+(\w+)(?:\[(\d+)\])?;'
        delete_matches = re.findall(delete_pattern, content)
        for element_name, index in delete_matches:
            if index:
                specialization.delete_elements.append(f"{element_name}[{index}]")
            else:
                specialization.delete_elements.append(element_name)
        
        self.specializations[template_name] = specialization
        return self._apply_element_specialization(specialization)
    
    def _apply_style_specialization(self, specialization: TemplateSpecialization) -> Dict[str, Any]:
        """应用样式组特例化"""
        result = {
            'type': 'style_specialization',
            'template_name': specialization.template_name,
            'delete_properties': specialization.delete_properties,
            'delete_inheritance': specialization.delete_inheritance,
            'add_properties': specialization.add_properties
        }
        return result
    
    def _apply_element_specialization(self, specialization: TemplateSpecialization) -> Dict[str, Any]:
        """应用元素特例化"""
        result = {
            'type': 'element_specialization',
            'template_name': specialization.template_name,
            'index_access': specialization.index_access,
            'insert_operations': specialization.insert_operations,
            'delete_elements': specialization.delete_elements
        }
        return result

class TemplateSpecializationParser:
    """模板特例化解析器"""
    
    def __init__(self):
        self.processor = TemplateSpecializationProcessor()
    
    def parse_style_specialization(self, content: str) -> List[Dict[str, Any]]:
        """解析样式组特例化"""
        specializations = []
        
        # 查找样式组特例化模式
        pattern = r'@Style\s+(\w+)\s*\{([^}]+)\}'
        matches = re.finditer(pattern, content)
        
        for match in matches:
            template_name = match.group(1)
            specialization_content = match.group(2)
            
            result = self.processor.process_style_specialization(
                template_name, specialization_content
            )
            specializations.append(result)
        
        return specializations
    
    def parse_element_specialization(self, content: str) -> List[Dict[str, Any]]:
        """解析元素特例化"""
        specializations = []
        
        # 查找元素特例化模式
        pattern = r'@Element\s+(\w+)\s*\{([^}]+)\}'
        matches = re.finditer(pattern, content)
        
        for match in matches:
            template_name = match.group(1)
            specialization_content = match.group(2)
            
            result = self.processor.process_element_specialization(
                template_name, specialization_content
            )
            specializations.append(result)
        
        return specializations
    
    def parse_all_specializations(self, content: str) -> Dict[str, List[Dict[str, Any]]]:
        """解析所有特例化"""
        return {
            'style': self.parse_style_specialization(content),
            'element': self.parse_element_specialization(content)
        }

def main():
    """测试模板特例化"""
    parser = TemplateSpecializationParser()
    
    # 测试样式组特例化
    style_content = """
    [Template] @Style WhiteText
    {
        color: white;
        font-size: 16px;
        line-height: 1.6;
        border: 1px black solid;
    }
    
    [Custom] @Style YellowText
    {
        @Style WhiteText
        {
            delete line-height, border;
        }
        color: yellow;
    }
    
    div
    {
        style
        {
            @Style YellowText
            {
                delete color;
            }
        }
    }
    """
    
    print("🧪 测试样式组特例化")
    print("=" * 50)
    
    style_specializations = parser.parse_style_specialization(style_content)
    for spec in style_specializations:
        print(f"样式组特例化: {spec}")
    
    # 测试元素特例化
    element_content = """
    [Custom] @Element Box
    {
        div
        {
            class: box-item;
        }
        div
        {
            class: box-item;
        }
        span
        {
            class: box-label;
        }
    }
    
    body
    {
        @Element Box
        {
            div[1]  // 索引访问
            {
                style
                {
                    background-color: red;
                }
            }
            
            // 插入元素
            insert after div[0] {
                div
                {
                    class: new-item;
                    text: "New Item";
                }
            }
            
            // 删除元素
            delete span;
        }
    }
    """
    
    print("\n🧪 测试元素特例化")
    print("=" * 50)
    
    element_specializations = parser.parse_element_specialization(element_content)
    for spec in element_specializations:
        print(f"元素特例化: {spec}")
    
    print("\n✅ 模板特例化测试完成")

if __name__ == "__main__":
    main()