#!/usr/bin/env python3
"""
高级CHTL编译器实现
基于CHTL.md规范实现高级功能：
- 模板特例化（删除属性、删除继承）
- 自定义元素特例化（索引访问、插入元素、删除元素）
- 命名空间系统
- 约束系统
- 配置系统
- 导入系统
- 原始嵌入系统
"""

import re
import os
from typing import Dict, List, Tuple, Optional, Union, Any
from dataclasses import dataclass, field
from enum import Enum
from abc import ABC, abstractmethod

# 导入基础编译器组件
from chtl_compiler import (
    TokenType, Token, CHTLContext, CHTLNode, ElementNode, TextNode, StyleNode,
    TemplateNode, StyleTemplateNode, ElementTemplateNode, VarTemplateNode,
    CustomNode, CustomStyleNode, CustomElementNode, CustomVarNode,
    ImportNode, ConfigNode, UseNode, CHTLVisitor, CHTLLexer, CHTLParser, CHTLGenerator
)

class AdvancedTokenType(Enum):
    """高级CHTL标记类型"""
    # 特例化关键字
    DELETE = "DELETE"
    INHERIT = "INHERIT"
    INSERT = "INSERT"
    AFTER = "AFTER"
    BEFORE = "BEFORE"
    REPLACE = "REPLACE"
    AT_TOP = "AT_TOP"
    AT_BOTTOM = "AT_BOTTOM"
    
    # 约束关键字
    EXCEPT = "EXCEPT"
    
    # 命名空间
    NAMESPACE = "NAMESPACE"
    
    # 配置
    CONFIGURATION = "CONFIGURATION"
    NAME = "NAME"
    
    # 原始嵌入
    ORIGIN = "ORIGIN"
    ORIGIN_HTML = "ORIGIN_HTML"
    ORIGIN_STYLE = "ORIGIN_STYLE"
    ORIGIN_JAVASCRIPT = "ORIGIN_JAVASCRIPT"
    
    # 索引访问
    LBRACKET_INDEX = "LBRACKET_INDEX"  # [数字]
    RBRACKET_INDEX = "RBRACKET_INDEX"
    
    # 位置关键字
    FROM = "FROM"
    AS = "AS"

@dataclass
class TemplateSpecialization:
    """模板特例化信息"""
    template_name: str
    delete_properties: List[str] = field(default_factory=list)
    delete_inheritance: List[str] = field(default_factory=list)
    add_properties: Dict[str, Any] = field(default_factory=dict)

@dataclass
class ElementSpecialization:
    """元素特例化信息"""
    element_name: str
    index: Optional[int] = None
    insert_operations: List[Dict[str, Any]] = field(default_factory=list)
    delete_elements: List[str] = field(default_factory=list)
    add_styles: Dict[str, Any] = field(default_factory=dict)

@dataclass
class NamespaceInfo:
    """命名空间信息"""
    name: str
    parent: Optional['NamespaceInfo'] = None
    templates: Dict[str, Any] = field(default_factory=dict)
    custom_elements: Dict[str, Any] = field(default_factory=dict)
    custom_styles: Dict[str, Any] = field(default_factory=dict)
    custom_vars: Dict[str, Any] = field(default_factory=dict)
    constraints: List[str] = field(default_factory=list)

@dataclass
class ConstraintInfo:
    """约束信息"""
    type: str  # 'precise', 'type', 'global'
    targets: List[str]
    scope: Optional[str] = None

@dataclass
class ConfigurationInfo:
    """配置信息"""
    name: Optional[str] = None
    settings: Dict[str, Any] = field(default_factory=dict)
    name_mappings: Dict[str, List[str]] = field(default_factory=dict)

class AdvancedCHTLContext(CHTLContext):
    """高级CHTL编译上下文"""
    
    def __init__(self):
        super().__init__()
        # 命名空间管理
        self.namespaces: Dict[str, NamespaceInfo] = {}
        self.current_namespace: Optional[str] = None
        
        # 约束管理
        self.constraints: List[ConstraintInfo] = []
        
        # 配置管理
        self.configurations: Dict[str, ConfigurationInfo] = {}
        self.active_configuration: Optional[str] = None
        
        # 导入管理
        self.imports: Dict[str, Any] = {}
        self.imported_namespaces: Set[str] = set()
        
        # 原始嵌入管理
        self.origin_embeds: Dict[str, str] = {}
        
        # 特例化管理
        self.template_specializations: Dict[str, TemplateSpecialization] = {}
        self.element_specializations: Dict[str, ElementSpecialization] = {}

class AdvancedCHTLLexer(CHTLLexer):
    """高级CHTL词法分析器"""
    
    def __init__(self):
        super().__init__()
        
        # 添加高级关键字
        self.keywords.update({
            'delete': AdvancedTokenType.DELETE,
            'inherit': AdvancedTokenType.INHERIT,
            'insert': AdvancedTokenType.INSERT,
            'after': AdvancedTokenType.AFTER,
            'before': AdvancedTokenType.BEFORE,
            'replace': AdvancedTokenType.REPLACE,
            'at': AdvancedTokenType.AT_TOP,  # 需要特殊处理 "at top", "at bottom"
            'top': AdvancedTokenType.AT_TOP,
            'bottom': AdvancedTokenType.AT_BOTTOM,
            'except': AdvancedTokenType.EXCEPT,
            'namespace': AdvancedTokenType.NAMESPACE,
            'configuration': AdvancedTokenType.CONFIGURATION,
            'name': AdvancedTokenType.NAME,
            'origin': AdvancedTokenType.ORIGIN,
            'from': AdvancedTokenType.FROM,
            'as': AdvancedTokenType.AS,
        })
        
        # 添加高级符号
        # 注意：不要覆盖基础的LBRACKET和RBRACKET
        # self.symbols.update({
        #     '[': AdvancedTokenType.LBRACKET_INDEX,
        #     ']': AdvancedTokenType.RBRACKET_INDEX,
        # })
    
    def tokenize(self, source: str) -> List[Token]:
        """高级词法分析"""
        tokens = super().tokenize(source)
        
        # 后处理：处理复合关键字
        processed_tokens = []
        i = 0
        while i < len(tokens):
            token = tokens[i]
            
            # 处理 "at top" 和 "at bottom"
            if (token.type == AdvancedTokenType.AT_TOP and 
                i + 1 < len(tokens) and 
                tokens[i + 1].value.lower() == 'top'):
                processed_tokens.append(Token(AdvancedTokenType.AT_TOP, "at top", 
                                            token.line, token.column, token.position))
                i += 2
            elif (token.type == AdvancedTokenType.AT_TOP and 
                  i + 1 < len(tokens) and 
                  tokens[i + 1].value.lower() == 'bottom'):
                processed_tokens.append(Token(AdvancedTokenType.AT_BOTTOM, "at bottom", 
                                            token.line, token.column, token.position))
                i += 2
            else:
                processed_tokens.append(token)
                i += 1
        
        return processed_tokens

class AdvancedCHTLParser(CHTLParser):
    """高级CHTL语法分析器"""
    
    def __init__(self, tokens: List[Token]):
        super().__init__(tokens)
        self.context = AdvancedCHTLContext()
    
    def parse(self) -> List[CHTLNode]:
        """高级解析主方法"""
        nodes = []
        
        while not self.match(TokenType.EOF):
            node = self.parse_advanced_statement()
            if node:
                nodes.append(node)
        
        return nodes
    
    def parse_advanced_statement(self) -> Optional[CHTLNode]:
        """解析高级语句"""
        token = self.current_token()
        
        if token.type == TokenType.LBRACKET:
            # 检查是否是高级语句
            if self.peek_token().type == AdvancedTokenType.CONFIGURATION:
                return self.parse_configuration_statement()
            elif self.peek_token().type == AdvancedTokenType.NAMESPACE:
                return self.parse_namespace_statement()
            elif self.peek_token().type == AdvancedTokenType.ORIGIN:
                return self.parse_origin_statement()
            else:
                return self.parse_bracket_statement()
        else:
            # 回退到基础解析
            return self.parse_statement()
    
    def parse_namespace_statement(self) -> 'NamespaceNode':
        """解析命名空间语句"""
        self.expect(TokenType.LBRACKET)
        self.expect(AdvancedTokenType.NAMESPACE)
        self.expect(TokenType.RBRACKET)
        namespace_name = self.expect(TokenType.IDENTIFIER).value
        self.expect(TokenType.LBRACE)
        
        # 创建命名空间
        namespace_info = NamespaceInfo(name=namespace_name)
        self.context.namespaces[namespace_name] = namespace_info
        self.context.current_namespace = namespace_name
        
        # 解析命名空间内容
        elements = []
        while not self.match(TokenType.RBRACE, TokenType.EOF):
            if self.match(TokenType.LBRACKET):
                # 检查是否是嵌套命名空间
                if self.peek_token().type == AdvancedTokenType.NAMESPACE:
                    element = self.parse_namespace_statement()
                    if element:
                        elements.append(element)
                else:
                    element = self.parse_bracket_statement()
                    if element:
                        elements.append(element)
            elif self.match(TokenType.IDENTIFIER):
                element = self.parse_element_statement()
                elements.append(element)
            else:
                self.advance()
        
        self.expect(TokenType.RBRACE)
        self.context.current_namespace = None
        
        return NamespaceNode(namespace_name, elements)
    
    def parse_import_statement(self) -> 'ImportNode':
        """解析导入语句"""
        self.expect(TokenType.IMPORT)
        self.expect(TokenType.RBRACKET)
        
        # 解析导入的命名空间名称
        namespace_name = self.expect(TokenType.IDENTIFIER).value
        self.expect(TokenType.SEMICOLON)
        
        # 创建导入节点
        import_node = ImportNode(namespace_name, 0, 0)
        
        # 将导入的命名空间添加到当前上下文中
        if namespace_name in self.context.namespaces:
            self.context.imported_namespaces.add(namespace_name)
        
        return import_node
    
    def parse_configuration_statement(self) -> 'ConfigurationNode':
        """解析配置语句"""
        self.expect(TokenType.LBRACKET)
        self.expect(AdvancedTokenType.CONFIGURATION)
        self.expect(TokenType.RBRACKET)
        
        # 检查是否有配置名称
        config_name = None
        if self.match(TokenType.AT_SYMBOL):
            self.advance()  # 跳过@
            config_name = self.expect(TokenType.IDENTIFIER).value
        
        # 配置语句必须有大括号
        self.expect(TokenType.LBRACE)
        
        settings = {}
        name_mappings = {}
        
        # 解析配置项
        while not self.match(TokenType.RBRACE, TokenType.EOF):
            if self.match(TokenType.IDENTIFIER):
                key = self.advance().value
                self.expect(TokenType.EQUALS)
                value = self.parse_attribute_value()
                settings[key] = value
                self.expect(TokenType.SEMICOLON)
            elif self.match(TokenType.LBRACKET) and self.peek_token().type == AdvancedTokenType.NAME:
                # 解析Name配置块
                self.advance()  # 跳过[
                self.expect(AdvancedTokenType.NAME)
                self.expect(TokenType.RBRACKET)
                self.expect(TokenType.LBRACE)
                
                # 解析名称映射
                while not self.match(TokenType.RBRACE, TokenType.EOF):
                    if self.match(TokenType.IDENTIFIER) or self.match(TokenType.AT_SYMBOL):
                        if self.match(TokenType.AT_SYMBOL):
                            self.advance()  # 跳过@
                        name_key = self.advance().value
                        self.expect(TokenType.EQUALS)
                        name_value = self.parse_name_mapping()
                        name_mappings[name_key] = name_value
                        self.expect(TokenType.SEMICOLON)
                    else:
                        self.advance()
                
                self.expect(TokenType.RBRACE)
            else:
                # 跳过未知标记
                self.advance()
        
        # 关闭大括号
        self.expect(TokenType.RBRACE)
        
        config_info = ConfigurationInfo(name=config_name, settings=settings, name_mappings=name_mappings)
        if config_name:
            self.context.configurations[config_name] = config_info
        else:
            self.context.active_configuration = "default"
            self.context.configurations["default"] = config_info
        
        return ConfigurationNode(config_name, settings, name_mappings)
    
    def parse_name_mapping(self) -> Union[str, List[str]]:
        """解析名称映射"""
        if self.match(TokenType.LBRACKET):
            # 组选项
            self.advance()  # 跳过[
            values = []
            while not self.match(TokenType.RBRACKET, TokenType.EOF):
                if self.match(TokenType.STRING):
                    values.append(self.advance().value.strip('"\''))
                elif self.match(TokenType.AT_SYMBOL):
                    # 处理@符号
                    at_token = self.advance()
                    if (self.match(TokenType.IDENTIFIER) or 
                        self.match(TokenType.STYLE) or 
                        self.match(TokenType.ELEMENT)):
                        identifier_token = self.advance()
                        values.append(f"@{identifier_token.value}")
                    else:
                        raise SyntaxError(f"Expected identifier after @ at line {at_token.line}, column {at_token.column}")
                elif self.match(TokenType.IDENTIFIER) or self.match(TokenType.STYLE) or self.match(TokenType.ELEMENT):
                    values.append(self.advance().value)
                elif self.match(TokenType.COMMA):
                    self.advance()
                else:
                    self.advance()
            self.expect(TokenType.RBRACKET)
            return values
        else:
            # 单个值
            if self.match(TokenType.AT_SYMBOL):
                # 处理@符号
                at_token = self.advance()
                if (self.match(TokenType.IDENTIFIER) or 
                    self.match(TokenType.STYLE) or 
                    self.match(TokenType.ELEMENT)):
                    identifier_token = self.advance()
                    return f"@{identifier_token.value}"
                else:
                    raise SyntaxError(f"Expected identifier after @ at line {at_token.line}, column {at_token.column}")
            else:
                return self.parse_attribute_value()
    
    def parse_origin_statement(self) -> 'OriginNode':
        """解析原始嵌入语句"""
        self.expect(TokenType.LBRACKET)
        self.expect(AdvancedTokenType.ORIGIN)
        self.expect(TokenType.RBRACKET)
        
        origin_type = self.expect(TokenType.IDENTIFIER).value
        self.expect(TokenType.RBRACKET)
        
        if origin_type.lower() == 'html':
            return self.parse_origin_html()
        elif origin_type.lower() == 'style':
            return self.parse_origin_style()
        elif origin_type.lower() == 'javascript':
            return self.parse_origin_javascript()
        else:
            # 跳过未知类型
            while not self.match(TokenType.LBRACE, TokenType.EOF):
                self.advance()
            if self.match(TokenType.LBRACE):
                self.parse_brace_block()
            return None
    
    def parse_origin_html(self) -> 'OriginHTMLNode':
        """解析原始HTML嵌入"""
        self.expect(TokenType.LBRACE)
        content = self.parse_origin_content()
        self.expect(TokenType.RBRACE)
        return OriginHTMLNode(content)
    
    def parse_origin_style(self) -> 'OriginStyleNode':
        """解析原始样式嵌入"""
        self.expect(TokenType.LBRACE)
        content = self.parse_origin_content()
        self.expect(TokenType.RBRACE)
        return OriginStyleNode(content)
    
    def parse_origin_javascript(self) -> 'OriginJavaScriptNode':
        """解析原始JavaScript嵌入"""
        self.expect(TokenType.LBRACE)
        content = self.parse_origin_content()
        self.expect(TokenType.RBRACE)
        return OriginJavaScriptNode(content)
    
    def parse_origin_content(self) -> str:
        """解析原始内容"""
        content = ""
        brace_count = 1
        
        while brace_count > 0 and not self.match(TokenType.EOF):
            token = self.advance()
            if token.type == TokenType.LBRACE:
                brace_count += 1
            elif token.type == TokenType.RBRACE:
                brace_count -= 1
            
            if brace_count > 0:
                content += token.value + " "
        
        return content.strip()

# 高级节点类型
class NamespaceNode(CHTLNode):
    """命名空间节点"""
    
    def __init__(self, name: str, elements: List[CHTLNode], line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.name = name
        self.elements = elements
    
    def accept(self, visitor: 'AdvancedCHTLVisitor') -> Any:
        return visitor.visit_namespace(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'NamespaceNode',
            'name': self.name,
            'elements': [elem.to_dict() for elem in self.elements],
            'line': self.line,
            'column': self.column
        }

class ImportNode(CHTLNode):
    """导入节点"""
    
    def __init__(self, namespace_name: str, line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.namespace_name = namespace_name
    
    def accept(self, visitor: 'AdvancedCHTLVisitor') -> Any:
        return visitor.visit_import(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'ImportNode',
            'namespace_name': self.namespace_name,
            'line': self.line,
            'column': self.column
        }

class OriginNode(CHTLNode):
    """原始嵌入节点基类"""
    
    def __init__(self, content: str, line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.content = content

class OriginHTMLNode(OriginNode):
    """原始HTML节点"""
    
    def accept(self, visitor: 'AdvancedCHTLVisitor') -> Any:
        return visitor.visit_origin_html(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'OriginHTMLNode',
            'content': self.content,
            'line': self.line,
            'column': self.column
        }

class OriginStyleNode(OriginNode):
    """原始样式节点"""
    
    def accept(self, visitor: 'AdvancedCHTLVisitor') -> Any:
        return visitor.visit_origin_style(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'OriginStyleNode',
            'content': self.content,
            'line': self.line,
            'column': self.column
        }

class OriginJavaScriptNode(OriginNode):
    """原始JavaScript节点"""
    
    def accept(self, visitor: 'AdvancedCHTLVisitor') -> Any:
        return visitor.visit_origin_javascript(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'OriginJavaScriptNode',
            'content': self.content,
            'line': self.line,
            'column': self.column
        }

class ConfigurationNode(CHTLNode):
    """配置节点"""
    
    def __init__(self, name: Optional[str], settings: Dict[str, Any], 
                 name_mappings: Dict[str, Any], line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.name = name
        self.settings = settings
        self.name_mappings = name_mappings
    
    def accept(self, visitor: 'AdvancedCHTLVisitor') -> Any:
        return visitor.visit_configuration(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'ConfigurationNode',
            'name': self.name,
            'settings': self.settings,
            'name_mappings': self.name_mappings,
            'line': self.line,
            'column': self.column
        }

class AdvancedCHTLVisitor(CHTLVisitor):
    """高级CHTL访问者模式基类"""
    
    @abstractmethod
    def visit_namespace(self, node: NamespaceNode) -> Any:
        pass
    
    @abstractmethod
    def visit_configuration(self, node: ConfigurationNode) -> Any:
        pass
    
    @abstractmethod
    def visit_import(self, node: ImportNode) -> Any:
        pass
    
    @abstractmethod
    def visit_origin_html(self, node: OriginHTMLNode) -> Any:
        pass
    
    @abstractmethod
    def visit_origin_style(self, node: OriginStyleNode) -> Any:
        pass
    
    @abstractmethod
    def visit_origin_javascript(self, node: OriginJavaScriptNode) -> Any:
        pass

class AdvancedCHTLGenerator(AdvancedCHTLVisitor):
    """高级CHTL代码生成器"""
    
    def __init__(self, context: AdvancedCHTLContext):
        self.context = context
        self.output = []
        self.indent_level = 0
    
    def generate(self, ast: List[CHTLNode]) -> str:
        """生成HTML代码"""
        self.output = []
        self.indent_level = 0
        
        for node in ast:
            node.accept(self)
        
        return '\n'.join(self.output)
    
    def indent(self):
        """增加缩进"""
        self.indent_level += 1
    
    def dedent(self):
        """减少缩进"""
        self.indent_level -= 1
    
    def write(self, text: str):
        """写入文本"""
        if text:
            self.output.append('  ' * self.indent_level + text)
    
    def visit_namespace(self, node: NamespaceNode) -> Any:
        """访问命名空间节点"""
        # 命名空间不直接生成HTML，而是管理作用域
        for element in node.elements:
            element.accept(self)
    
    def visit_configuration(self, node: ConfigurationNode) -> Any:
        """访问配置节点"""
        # 配置节点不直接生成HTML
        pass
    
    def visit_import(self, node: ImportNode) -> Any:
        """访问导入节点"""
        # 导入节点不直接生成HTML
        pass
    
    def visit_origin_html(self, node: OriginHTMLNode) -> Any:
        """访问原始HTML节点"""
        self.write(node.content)
    
    def visit_origin_style(self, node: OriginStyleNode) -> Any:
        """访问原始样式节点"""
        self.write(f'<style>{node.content}</style>')
    
    def visit_origin_javascript(self, node: OriginJavaScriptNode) -> Any:
        """访问原始JavaScript节点"""
        self.write(f'<script>{node.content}</script>')
    
    # 继承基础访问者方法
    def visit_element(self, node: ElementNode) -> Any:
        """访问元素节点"""
        # 生成开始标签
        tag_parts = [node.tag_name]
        
        # 添加属性
        for attr_name, attr_value in node.attributes.items():
            if isinstance(attr_value, str):
                tag_parts.append(f'{attr_name}="{attr_value}"')
            else:
                tag_parts.append(f'{attr_name}="{attr_value}"')
        
        if node.children:
            # 有子元素，生成完整标签
            self.write(f'<{" ".join(tag_parts)}>')
            self.indent()
            
            # 生成子元素
            for child in node.children:
                child.accept(self)
            
            self.dedent()
            self.write(f'</{node.tag_name}>')
        else:
            # 无子元素，生成自闭合标签
            self.write(f'<{" ".join(tag_parts)} />')
    
    def visit_text(self, node: TextNode) -> Any:
        """访问文本节点"""
        self.write(node.content)
    
    def visit_style(self, node: StyleNode) -> Any:
        """访问样式节点"""
        if node.properties:
            self.write('<style>')
            self.indent()
            for prop_name, prop_value in node.properties.items():
                self.write(f'{prop_name}: {prop_value};')
            self.dedent()
            self.write('</style>')
    
    def visit_script(self, node: 'ScriptNode') -> Any:
        """访问脚本节点"""
        if hasattr(node, 'content') and node.content:
            self.write('<script>')
            self.indent()
            self.write(node.content)
            self.dedent()
            self.write('</script>')
    
    def visit_style_template(self, node: StyleTemplateNode) -> Any:
        """访问样式组模板节点"""
        # 样式组模板不直接生成HTML，而是存储在上下文中
        pass
    
    def visit_element_template(self, node: ElementTemplateNode) -> Any:
        """访问元素模板节点"""
        # 元素模板不直接生成HTML，而是存储在上下文中
        pass
    
    def visit_var_template(self, node: VarTemplateNode) -> Any:
        """访问变量组模板节点"""
        # 变量组模板不直接生成HTML，而是存储在上下文中
        pass
    
    def visit_custom_style(self, node: CustomStyleNode) -> Any:
        """访问自定义样式组节点"""
        # 自定义样式组不直接生成HTML，而是存储在上下文中
        pass
    
    def visit_custom_element(self, node: CustomElementNode) -> Any:
        """访问自定义元素节点"""
        # 自定义元素不直接生成HTML，而是存储在上下文中
        pass
    
    def visit_custom_var(self, node: CustomVarNode) -> Any:
        """访问自定义变量节点"""
        # 自定义变量不直接生成HTML，而是存储在上下文中
        pass
    
    def visit_import(self, node: ImportNode) -> Any:
        """访问导入节点"""
        # 导入节点不直接生成HTML
        pass
    
    def visit_config(self, node: ConfigNode) -> Any:
        """访问配置节点"""
        # 配置节点不直接生成HTML
        pass
    
    def visit_use(self, node: UseNode) -> Any:
        """访问使用节点"""
        # 使用节点不直接生成HTML
        pass

def main():
    """测试高级CHTL编译器"""
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    use html5;
    
    [Configuration]
    {
        DEBUG_MODE = false;
        INDEX_INITIAL_COUNT = 0;
        
        [Name]
        {
            CUSTOM_STYLE = [@Style, @style];
            CUSTOM_ELEMENT = @Element;
        }
    }
    
    [Namespace] MyNamespace
    {
        [Template] @Style ButtonStyle
        {
            background-color: blue;
            color: white;
            padding: 10px 20px;
        }
        
        [Custom] @Element Button
        {
            button
            {
                class: btn;
                style
                {
                    @Style ButtonStyle;
                }
                text: "Click Me";
            }
        }
    }
    
    html
    {
        body
        {
            @Element Button from MyNamespace;
        }
    }
    """
    
    print("高级CHTL编译器测试")
    print("=" * 50)
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = AdvancedCHTLParser(tokens)
    ast = parser.parse()
    
    print("代码生成...")
    generator = AdvancedCHTLGenerator(parser.context)
    html = generator.generate(ast)
    
    print("生成的HTML:")
    print(html)

if __name__ == "__main__":
    main()