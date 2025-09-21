#!/usr/bin/env python3
"""
CHTL编译器核心实现
基于CHTL.md规范实现完整的CHTL编译器

核心组件：
- CHTLLexer: 词法分析器
- CHTLParser: 语法分析器  
- CHTLGenerator: 代码生成器
- CHTLContext: 编译上下文
- CHTLNode: AST节点系统
"""

import re
import json
from typing import Dict, List, Tuple, Optional, Union, Any
from dataclasses import dataclass, field
from enum import Enum
from abc import ABC, abstractmethod

class TokenType(Enum):
    """CHTL标记类型"""
    # 基础标记
    IDENTIFIER = "IDENTIFIER"
    STRING = "STRING"
    NUMBER = "NUMBER"
    LITERAL = "LITERAL"  # 无修饰字面量
    
    # 符号
    LBRACE = "LBRACE"      # {
    RBRACE = "RBRACE"      # }
    LPAREN = "LPAREN"      # (
    RPAREN = "RPAREN"      # )
    LBRACKET = "LBRACKET"  # [
    RBRACKET = "RBRACKET"  # ]
    COLON = "COLON"        # :
    SEMICOLON = "SEMICOLON" # ;
    COMMA = "COMMA"        # ,
    DOT = "DOT"            # .
    EQUALS = "EQUALS"      # =
    ARROW = "ARROW"        # ->
    
    # 运算符
    PLUS = "PLUS"          # +
    MINUS = "MINUS"        # -
    MULTIPLY = "MULTIPLY"  # *
    DIVIDE = "DIVIDE"      # /
    MODULO = "MODULO"      # %
    POWER = "POWER"        # **
    AND = "AND"            # &&
    OR = "OR"              # ||
    NOT = "NOT"            # !
    GT = "GT"              # >
    LT = "LT"              # <
    GE = "GE"              # >=
    LE = "LE"              # <=
    EQ = "EQ"              # ==
    NE = "NE"              # !=
    QUESTION = "QUESTION"  # ?
    
    # 关键字
    TEMPLATE = "TEMPLATE"
    CUSTOM = "CUSTOM"
    STYLE = "STYLE"
    ELEMENT = "ELEMENT"
    VAR = "VAR"
    IMPORT = "IMPORT"
    CONFIG = "CONFIG"
    USE = "USE"
    DELETE = "DELETE"
    INHERIT = "INHERIT"
    FROM = "FROM"
    TEXT = "TEXT"
    SCRIPT = "SCRIPT"
    IF = "IF"
    FOR = "FOR"
    WHILE = "WHILE"
    FUNCTION = "FUNCTION"
    RETURN = "RETURN"
    CLASS = "CLASS"
    EXTENDS = "EXTENDS"
    IMPLEMENTS = "IMPLEMENTS"
    PUBLIC = "PUBLIC"
    PRIVATE = "PRIVATE"
    PROTECTED = "PROTECTED"
    STATIC = "STATIC"
    FINAL = "FINAL"
    ABSTRACT = "ABSTRACT"
    INTERFACE = "INTERFACE"
    PACKAGE = "PACKAGE"
    NAMESPACE = "NAMESPACE"
    
    # 特殊标记
    AT_SYMBOL = "AT_SYMBOL"    # @
    HASH = "HASH"              # #
    DOLLAR = "DOLLAR"          # $
    AMPERSAND = "AMPERSAND"    # &
    TILDE = "TILDE"            # ~
    CARET = "CARET"            # ^
    PIPE = "PIPE"              # |
    UNDERSCORE = "UNDERSCORE"  # _
    
    # 注释
    COMMENT_LINE = "COMMENT_LINE"    # //
    COMMENT_BLOCK = "COMMENT_BLOCK"  # /* */
    GENERATOR_COMMENT = "GENERATOR_COMMENT"  # #
    
    # 特殊语法
    TEMPLATE_VAR = "TEMPLATE_VAR"    # @Style, @Element, @Var
    CUSTOM_VAR = "CUSTOM_VAR"        # [Custom] @Style
    IMPORT_STMT = "IMPORT_STMT"      # [Import] @Chtl
    CONFIG_STMT = "CONFIG_STMT"      # [Configuration]
    USE_STMT = "USE_STMT"            # use html5;
    EXCEPT = "EXCEPT"                # except
    
    # 结束标记
    EOF = "EOF"
    NEWLINE = "NEWLINE"
    WHITESPACE = "WHITESPACE"

@dataclass
class Token:
    """标记数据结构"""
    type: TokenType
    value: str
    line: int
    column: int
    position: int

@dataclass
class CHTLContext:
    """CHTL编译上下文"""
    # 模板存储
    style_templates: Dict[str, 'StyleTemplateNode'] = field(default_factory=dict)
    element_templates: Dict[str, 'ElementTemplateNode'] = field(default_factory=dict)
    var_templates: Dict[str, 'VarTemplateNode'] = field(default_factory=dict)
    
    # 自定义存储
    custom_styles: Dict[str, 'CustomStyleNode'] = field(default_factory=dict)
    custom_elements: Dict[str, 'CustomElementNode'] = field(default_factory=dict)
    custom_vars: Dict[str, 'CustomVarNode'] = field(default_factory=dict)
    
    # 命名空间
    namespaces: Dict[str, Dict[str, Any]] = field(default_factory=dict)
    
    # 配置
    config: Dict[str, Any] = field(default_factory=dict)
    
    # 导入的模块
    imported_modules: Dict[str, Any] = field(default_factory=dict)
    
    # 全局样式收集
    global_styles: List[str] = field(default_factory=list)
    
    # 当前作用域
    current_scope: List[str] = field(default_factory=list)

class CHTLNode(ABC):
    """CHTL AST节点基类"""
    
    def __init__(self, line: int = 0, column: int = 0):
        self.line = line
        self.column = column
    
    @abstractmethod
    def accept(self, visitor: 'CHTLVisitor') -> Any:
        """接受访问者模式"""
        pass
    
    @abstractmethod
    def to_dict(self) -> Dict[str, Any]:
        """转换为字典表示"""
        pass

class ElementNode(CHTLNode):
    """元素节点"""
    
    def __init__(self, tag_name: str, attributes: Dict[str, Any] = None, 
                 children: List[CHTLNode] = None, index: int = None, line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.tag_name = tag_name
        self.attributes = attributes or {}
        self.children = children or []
        self.index = index
    
    def accept(self, visitor: 'CHTLVisitor') -> Any:
        return visitor.visit_element(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'ElementNode',
            'tag_name': self.tag_name,
            'attributes': self.attributes,
            'children': [child.to_dict() for child in self.children],
            'line': self.line,
            'column': self.column
        }

class TextNode(CHTLNode):
    """文本节点"""
    
    def __init__(self, content: str, line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.content = content
    
    def accept(self, visitor: 'CHTLVisitor') -> Any:
        return visitor.visit_text(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'TextNode',
            'content': self.content,
            'line': self.line,
            'column': self.column
        }

class StyleNode(CHTLNode):
    """样式节点"""
    
    def __init__(self, properties: Dict[str, Any] = None, 
                 selectors: List[str] = None, line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.properties = properties or {}
        self.selectors = selectors or []
    
    def accept(self, visitor: 'CHTLVisitor') -> Any:
        return visitor.visit_style(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'StyleNode',
            'properties': self.properties,
            'selectors': self.selectors,
            'line': self.line,
            'column': self.column
        }

class ScriptNode(CHTLNode):
    """脚本节点"""
    
    def __init__(self, content: str, line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.content = content
    
    def accept(self, visitor: 'CHTLVisitor') -> Any:
        return visitor.visit_script(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'ScriptNode',
            'content': self.content,
            'line': self.line,
            'column': self.column
        }

class TemplateNode(CHTLNode):
    """模板节点基类"""
    
    def __init__(self, name: str, line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.name = name

class StyleTemplateNode(TemplateNode):
    """样式组模板节点"""
    
    def __init__(self, name: str, properties: Dict[str, Any] = None, 
                 line: int = 0, column: int = 0):
        super().__init__(name, line, column)
        self.properties = properties or {}
    
    def accept(self, visitor: 'CHTLVisitor') -> Any:
        return visitor.visit_style_template(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'StyleTemplateNode',
            'name': self.name,
            'properties': self.properties,
            'line': self.line,
            'column': self.column
        }

class ElementTemplateNode(TemplateNode):
    """元素模板节点"""
    
    def __init__(self, name: str, elements: List[CHTLNode] = None, 
                 line: int = 0, column: int = 0):
        super().__init__(name, line, column)
        self.elements = elements or []
    
    def accept(self, visitor: 'CHTLVisitor') -> Any:
        return visitor.visit_element_template(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'ElementTemplateNode',
            'name': self.name,
            'elements': [elem.to_dict() for elem in self.elements],
            'line': self.line,
            'column': self.column
        }

class VarTemplateNode(TemplateNode):
    """变量组模板节点"""
    
    def __init__(self, name: str, variables: Dict[str, Any] = None, 
                 line: int = 0, column: int = 0):
        super().__init__(name, line, column)
        self.variables = variables or {}
    
    def accept(self, visitor: 'CHTLVisitor') -> Any:
        return visitor.visit_var_template(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'VarTemplateNode',
            'name': self.name,
            'variables': self.variables,
            'line': self.line,
            'column': self.column
        }

class CustomNode(CHTLNode):
    """自定义节点基类"""
    
    def __init__(self, name: str, line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.name = name

class CustomStyleNode(CustomNode):
    """自定义样式组节点"""
    
    def __init__(self, name: str, properties: Dict[str, Any] = None, 
                 line: int = 0, column: int = 0):
        super().__init__(name, line, column)
        self.properties = properties or {}
    
    def accept(self, visitor: 'CHTLVisitor') -> Any:
        return visitor.visit_custom_style(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'CustomStyleNode',
            'name': self.name,
            'properties': self.properties,
            'line': self.line,
            'column': self.column
        }

class CustomElementNode(CustomNode):
    """自定义元素节点"""
    
    def __init__(self, name: str, elements: List[CHTLNode] = None, 
                 line: int = 0, column: int = 0):
        super().__init__(name, line, column)
        self.elements = elements or []
    
    def accept(self, visitor: 'CHTLVisitor') -> Any:
        return visitor.visit_custom_element(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'CustomElementNode',
            'name': self.name,
            'elements': [elem.to_dict() for elem in self.elements],
            'line': self.line,
            'column': self.column
        }

class CustomVarNode(CustomNode):
    """自定义变量节点"""
    
    def __init__(self, name: str, variables: Dict[str, Any] = None, 
                 line: int = 0, column: int = 0):
        super().__init__(name, line, column)
        self.variables = variables or {}
    
    def accept(self, visitor: 'CHTLVisitor') -> Any:
        return visitor.visit_custom_var(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'CustomVarNode',
            'name': self.name,
            'variables': self.variables,
            'line': self.line,
            'column': self.column
        }

class ImportNode(CHTLNode):
    """导入节点"""
    
    def __init__(self, module_type: str, module_name: str, 
                 line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.module_type = module_type
        self.module_name = module_name
    
    def accept(self, visitor: 'CHTLVisitor') -> Any:
        return visitor.visit_import(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'ImportNode',
            'module_type': self.module_type,
            'module_name': self.module_name,
            'line': self.line,
            'column': self.column
        }

class ConfigNode(CHTLNode):
    """配置节点"""
    
    def __init__(self, config: Dict[str, Any], line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.config = config
    
    def accept(self, visitor: 'CHTLVisitor') -> Any:
        return visitor.visit_config(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'ConfigNode',
            'config': self.config,
            'line': self.line,
            'column': self.column
        }

class UseNode(CHTLNode):
    """使用节点"""
    
    def __init__(self, directive: str, args: List[str] = None, line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.directive = directive
        self.args = args or []
    
    def accept(self, visitor: 'CHTLVisitor') -> Any:
        return visitor.visit_use(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'UseNode',
            'directive': self.directive,
            'line': self.line,
            'column': self.column
        }

class CHTLVisitor(ABC):
    """CHTL访问者模式基类"""
    
    @abstractmethod
    def visit_element(self, node: ElementNode) -> Any:
        pass
    
    @abstractmethod
    def visit_text(self, node: TextNode) -> Any:
        pass
    
    @abstractmethod
    def visit_style(self, node: StyleNode) -> Any:
        pass
    
    @abstractmethod
    def visit_script(self, node: ScriptNode) -> Any:
        pass
    
    @abstractmethod
    def visit_style_template(self, node: StyleTemplateNode) -> Any:
        pass
    
    @abstractmethod
    def visit_element_template(self, node: ElementTemplateNode) -> Any:
        pass
    
    @abstractmethod
    def visit_var_template(self, node: VarTemplateNode) -> Any:
        pass
    
    @abstractmethod
    def visit_custom_style(self, node: CustomStyleNode) -> Any:
        pass
    
    @abstractmethod
    def visit_custom_element(self, node: CustomElementNode) -> Any:
        pass
    
    @abstractmethod
    def visit_custom_var(self, node: CustomVarNode) -> Any:
        pass
    
    @abstractmethod
    def visit_import(self, node: ImportNode) -> Any:
        pass
    
    @abstractmethod
    def visit_config(self, node: ConfigNode) -> Any:
        pass
    
    @abstractmethod
    def visit_use(self, node: UseNode) -> Any:
        pass

class CHTLLexer:
    """CHTL词法分析器"""
    
    def __init__(self):
        # 关键字映射
        self.keywords = {
            'template': TokenType.TEMPLATE,
            'custom': TokenType.CUSTOM,
            'style': TokenType.STYLE,
            'element': TokenType.ELEMENT,
            'var': TokenType.VAR,
            'import': TokenType.IMPORT,
            'config': TokenType.CONFIG,
            'use': TokenType.USE,
            'delete': TokenType.DELETE,
            'inherit': TokenType.INHERIT,
            'from': TokenType.FROM,
            'text': TokenType.TEXT,
            'except': TokenType.EXCEPT,
            'script': TokenType.SCRIPT,
            'if': TokenType.IF,
            'for': TokenType.FOR,
            'while': TokenType.WHILE,
            'function': TokenType.FUNCTION,
            'return': TokenType.RETURN,
            'class': TokenType.CLASS,
            'extends': TokenType.EXTENDS,
            'implements': TokenType.IMPLEMENTS,
            'public': TokenType.PUBLIC,
            'private': TokenType.PRIVATE,
            'protected': TokenType.PROTECTED,
            'static': TokenType.STATIC,
            'final': TokenType.FINAL,
            'abstract': TokenType.ABSTRACT,
            'interface': TokenType.INTERFACE,
            'package': TokenType.PACKAGE,
            'namespace': TokenType.NAMESPACE,
        }
        
        # 符号映射
        self.symbols = {
            '{': TokenType.LBRACE,
            '}': TokenType.RBRACE,
            '(': TokenType.LPAREN,
            ')': TokenType.RPAREN,
            '[': TokenType.LBRACKET,
            ']': TokenType.RBRACKET,
            ':': TokenType.COLON,
            ';': TokenType.SEMICOLON,
            ',': TokenType.COMMA,
            '.': TokenType.DOT,
            '=': TokenType.EQUALS,
            '->': TokenType.ARROW,
            '+': TokenType.PLUS,
            '-': TokenType.MINUS,
            '*': TokenType.MULTIPLY,
            '/': TokenType.DIVIDE,
            '%': TokenType.MODULO,
            '**': TokenType.POWER,
            '&&': TokenType.AND,
            '||': TokenType.OR,
            '!': TokenType.NOT,
            '>': TokenType.GT,
            '<': TokenType.LT,
            '>=': TokenType.GE,
            '<=': TokenType.LE,
            '==': TokenType.EQ,
            '!=': TokenType.NE,
            '?': TokenType.QUESTION,
            '@': TokenType.AT_SYMBOL,
            '#': TokenType.HASH,
            '$': TokenType.DOLLAR,
            '&': TokenType.AMPERSAND,
            '~': TokenType.TILDE,
            '^': TokenType.CARET,
            '|': TokenType.PIPE,
            '_': TokenType.UNDERSCORE,
        }
    
    def tokenize(self, source: str) -> List[Token]:
        """词法分析主方法"""
        tokens = []
        position = 0
        line = 1
        column = 1
        
        while position < len(source):
            char = source[position]
            
            # 跳过空白字符
            if char.isspace():
                if char == '\n':
                    line += 1
                    column = 1
                else:
                    column += 1
                position += 1
                continue
            
            # 处理注释
            if char == '/' and position + 1 < len(source):
                next_char = source[position + 1]
                if next_char == '/':
                    # 单行注释
                    comment_start = position
                    position += 2
                    while position < len(source) and source[position] != '\n':
                        position += 1
                    tokens.append(Token(
                        TokenType.COMMENT_LINE,
                        source[comment_start:position],
                        line, column, position
                    ))
                    continue
                elif next_char == '*':
                    # 多行注释
                    comment_start = position
                    position += 2
                    while position + 1 < len(source):
                        if source[position] == '*' and source[position + 1] == '/':
                            position += 2
                            break
                        position += 1
                    tokens.append(Token(
                        TokenType.COMMENT_BLOCK,
                        source[comment_start:position],
                        line, column, position
                    ))
                    continue
            
            # 处理生成器注释或CSS颜色值
            if char == '#':
                comment_start = position
                position += 1
                
                # 检查是否是CSS颜色值（如 #ddd, #ffffff）
                if (position < len(source) and 
                    source[position].isalnum() and 
                    position + 1 < len(source) and 
                    source[position + 1].isalnum()):
                    # 这是CSS颜色值，不是注释
                    while (position < len(source) and 
                           (source[position].isalnum() or source[position] == ';')):
                        position += 1
                    # 如果以分号结尾，需要分离颜色值和分号
                    if source[position - 1] == ';':
                        tokens.append(Token(
                            TokenType.LITERAL,
                            source[comment_start:position - 1],
                            line, column, position - 1
                        ))
                        tokens.append(Token(
                            TokenType.SEMICOLON,
                            ';',
                            line, column, position
                        ))
                    else:
                        tokens.append(Token(
                            TokenType.LITERAL,
                            source[comment_start:position],
                            line, column, position
                        ))
                else:
                    # 这是注释
                    while position < len(source) and source[position] != '\n':
                        position += 1
                    tokens.append(Token(
                        TokenType.GENERATOR_COMMENT,
                        source[comment_start:position],
                        line, column, position
                    ))
                continue
            
            # 处理字符串
            if char in ['"', "'"]:
                string_start = position
                quote = char
                position += 1
                while position < len(source) and source[position] != quote:
                    if source[position] == '\\' and position + 1 < len(source):
                        position += 2  # 跳过转义字符
                    else:
                        position += 1
                if position < len(source):
                    position += 1  # 跳过结束引号
                tokens.append(Token(
                    TokenType.STRING,
                    source[string_start:position],
                    line, column, position
                ))
                continue
            
            # 处理数字
            if char.isdigit():
                number_start = position
                while position < len(source) and (source[position].isdigit() or source[position] == '.'):
                    position += 1
                tokens.append(Token(
                    TokenType.NUMBER,
                    source[number_start:position],
                    line, column, position
                ))
                continue
            
            # 处理点号（命名空间访问）
            if char == '.':
                tokens.append(Token(
                    TokenType.DOT,
                    '.',
                    line, column, position
                ))
                position += 1
                column += 1
                continue
            
            # 处理标识符和关键字
            if char.isalpha() or char == '_':
                identifier_start = position
                while position < len(source) and (source[position].isalnum() or source[position] == '_'):
                    position += 1
                identifier = source[identifier_start:position]
                
                # 检查是否是关键字
                token_type = self.keywords.get(identifier.lower(), TokenType.IDENTIFIER)
                tokens.append(Token(token_type, identifier, line, column, position))
                continue
            
            # 处理符号
            # 先检查双字符符号
            if position + 1 < len(source):
                two_char = source[position:position + 2]
                if two_char in self.symbols:
                    tokens.append(Token(
                        self.symbols[two_char],
                        two_char,
                        line, column, position
                    ))
                    position += 2
                    column += 2
                    continue
            
            # 检查单字符符号
            if char in self.symbols:
                tokens.append(Token(
                    self.symbols[char],
                    char,
                    line, column, position
                ))
                position += 1
                column += 1
                continue
            
            
            # 未知字符，跳过
            position += 1
            column += 1
        
        # 添加结束标记
        tokens.append(Token(TokenType.EOF, "", line, column, position))
        
        return tokens

class CHTLParser:
    """CHTL语法分析器"""
    
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.position = 0
        self.context = CHTLContext()
    
    def current_token(self) -> Token:
        """获取当前标记"""
        if self.position < len(self.tokens):
            return self.tokens[self.position]
        return Token(TokenType.EOF, "", 0, 0, 0)
    
    def peek_token(self, offset: int = 1) -> Token:
        """预览标记"""
        pos = self.position + offset
        if pos < len(self.tokens):
            return self.tokens[pos]
        return Token(TokenType.EOF, "", 0, 0, 0)
    
    def advance(self) -> Token:
        """前进到下一个标记"""
        if self.position < len(self.tokens):
            token = self.tokens[self.position]
            self.position += 1
            return token
        return Token(TokenType.EOF, "", 0, 0, 0)
    
    def expect(self, expected_type: TokenType) -> Token:
        """期望特定类型的标记"""
        token = self.current_token()
        if token.type == expected_type:
            return self.advance()
        else:
            raise SyntaxError(f"Expected {expected_type.value}, got {token.type.value} at line {token.line}, column {token.column}")
    
    def match(self, *token_types: TokenType) -> bool:
        """检查当前标记是否匹配指定类型"""
        return self.current_token().type in token_types
    
    def parse(self) -> List[CHTLNode]:
        """解析主方法"""
        nodes = []
        
        while not self.match(TokenType.EOF):
            node = self.parse_statement()
            if node:
                nodes.append(node)
        
        return nodes
    
    def parse_statement(self) -> Optional[CHTLNode]:
        """解析语句"""
        token = self.current_token()
        
        if token.type == TokenType.USE:
            return self.parse_use_statement()
        elif token.type == TokenType.LBRACKET:
            return self.parse_bracket_statement()
        elif token.type == TokenType.TEMPLATE:
            return self.parse_template_statement()
        elif token.type == TokenType.CUSTOM:
            return self.parse_custom_statement()
        elif token.type == TokenType.IDENTIFIER:
            return self.parse_element_statement()
        else:
            # 跳过未知标记
            self.advance()
            return None
    
    def parse_use_statement(self) -> UseNode:
        """解析use语句"""
        use_token = self.expect(TokenType.USE)
        directive = self.expect(TokenType.IDENTIFIER).value
        
        # 解析可选的参数
        args = []
        if self.match(TokenType.LPAREN):
            while not self.match(TokenType.RPAREN):
                if self.current_token.type == TokenType.STRING:
                    args.append(self.current_token.value.strip('"'))
                elif self.current_token.type == TokenType.IDENTIFIER:
                    args.append(self.current_token.value)
                self.advance()
                if self.current_token.type == TokenType.COMMA:
                    self.advance()
        
        self.expect(TokenType.SEMICOLON)
        
        return UseNode(directive, args, use_token.line, use_token.column)
    
    def parse_bracket_statement(self) -> Optional[CHTLNode]:
        """解析方括号语句"""
        self.expect(TokenType.LBRACKET)
        
        if self.match(TokenType.TEMPLATE):
            return self.parse_template_statement()
        elif self.match(TokenType.CUSTOM):
            return self.parse_custom_statement()
        elif self.match(TokenType.IMPORT):
            return self.parse_import_statement()
        elif self.match(TokenType.CONFIG):
            return self.parse_config_statement()
        else:
            # 跳过未知的方括号语句
            while not self.match(TokenType.RBRACKET, TokenType.EOF):
                self.advance()
            if self.match(TokenType.RBRACKET):
                self.advance()
            return None
    
    def parse_template_statement(self) -> Optional[CHTLNode]:
        """解析模板语句"""
        template_token = self.expect(TokenType.TEMPLATE)
        self.expect(TokenType.RBRACKET)
        
        at_token = self.expect(TokenType.AT_SYMBOL)
        
        if self.match(TokenType.STYLE):
            return self.parse_style_template()
        elif self.match(TokenType.ELEMENT):
            return self.parse_element_template()
        elif self.match(TokenType.VAR):
            return self.parse_var_template()
        else:
            # 跳过未知模板类型
            while not self.match(TokenType.LBRACE, TokenType.EOF):
                self.advance()
            if self.match(TokenType.LBRACE):
                self.parse_brace_block()
            return None
    
    def parse_style_template(self) -> StyleTemplateNode:
        """解析样式组模板"""
        self.expect(TokenType.STYLE)
        name = self.expect(TokenType.IDENTIFIER).value
        
        self.expect(TokenType.LBRACE)
        properties = self.parse_style_properties()
        self.expect(TokenType.RBRACE)
        
        node = StyleTemplateNode(name, properties, 0, 0)
        self.context.style_templates[name] = node
        return node
    
    def parse_element_template(self) -> ElementTemplateNode:
        """解析元素模板"""
        self.expect(TokenType.ELEMENT)
        name = self.expect(TokenType.IDENTIFIER).value
        
        self.expect(TokenType.LBRACE)
        elements = self.parse_element_block()
        self.expect(TokenType.RBRACE)
        
        node = ElementTemplateNode(name, elements, 0, 0)
        self.context.element_templates[name] = node
        return node
    
    def parse_var_template(self) -> VarTemplateNode:
        """解析变量组模板"""
        self.expect(TokenType.VAR)
        name = self.expect(TokenType.IDENTIFIER).value
        
        self.expect(TokenType.LBRACE)
        variables = self.parse_var_properties()
        self.expect(TokenType.RBRACE)
        
        node = VarTemplateNode(name, variables, 0, 0)
        self.context.var_templates[name] = node
        return node
    
    def parse_custom_statement(self) -> Optional[CHTLNode]:
        """解析自定义语句"""
        custom_token = self.expect(TokenType.CUSTOM)
        self.expect(TokenType.RBRACKET)
        
        at_token = self.expect(TokenType.AT_SYMBOL)
        
        if self.match(TokenType.STYLE):
            return self.parse_custom_style()
        elif self.match(TokenType.ELEMENT):
            return self.parse_custom_element()
        elif self.match(TokenType.VAR):
            return self.parse_custom_var()
        else:
            # 跳过未知自定义类型
            while not self.match(TokenType.LBRACE, TokenType.EOF):
                self.advance()
            if self.match(TokenType.LBRACE):
                self.parse_brace_block()
            return None
    
    def parse_custom_style(self) -> CustomStyleNode:
        """解析自定义样式组"""
        self.expect(TokenType.STYLE)
        name = self.expect(TokenType.IDENTIFIER).value
        
        self.expect(TokenType.LBRACE)
        properties = self.parse_style_properties()
        self.expect(TokenType.RBRACE)
        
        node = CustomStyleNode(name, properties, 0, 0)
        self.context.custom_styles[name] = node
        return node
    
    def parse_custom_element(self) -> CustomElementNode:
        """解析自定义元素"""
        self.expect(TokenType.ELEMENT)
        name = self.expect(TokenType.IDENTIFIER).value
        
        self.expect(TokenType.LBRACE)
        elements = self.parse_element_block()
        self.expect(TokenType.RBRACE)
        
        node = CustomElementNode(name, elements, 0, 0)
        self.context.custom_elements[name] = node
        return node
    
    def parse_custom_var(self) -> CustomVarNode:
        """解析自定义变量"""
        self.expect(TokenType.VAR)
        name = self.expect(TokenType.IDENTIFIER).value
        
        self.expect(TokenType.LBRACE)
        variables = self.parse_var_properties()
        self.expect(TokenType.RBRACE)
        
        node = CustomVarNode(name, variables, 0, 0)
        self.context.custom_vars[name] = node
        return node
    
    def parse_element_statement(self) -> ElementNode:
        """解析元素语句"""
        tag_name = self.expect(TokenType.IDENTIFIER).value
        
        # 检查是否有索引访问语法（如 Card[0]）
        index = None
        if self.match(TokenType.LBRACKET):
            self.advance()  # 跳过左括号
            if self.match(TokenType.NUMBER):
                index = int(self.advance().value)
            self.expect(TokenType.RBRACKET)
        
        self.expect(TokenType.LBRACE)
        
        attributes = {}
        children = []
        
        while not self.match(TokenType.RBRACE, TokenType.EOF):
            if self.match(TokenType.TEXT):
                children.append(self.parse_text_statement())
            elif self.match(TokenType.STYLE):
                children.append(self.parse_style_statement())
            elif self.match(TokenType.SCRIPT):
                children.append(self.parse_script_statement())
            elif self.match(TokenType.IDENTIFIER):
                # 可能是属性或子元素
                if self.peek_token().type == TokenType.COLON:
                    # 属性
                    attr_name = self.advance().value
                    self.expect(TokenType.COLON)
                    attr_value = self.parse_attribute_value()
                    attributes[attr_name] = attr_value
                    self.expect(TokenType.SEMICOLON)
                elif self.peek_token().type == TokenType.LBRACE:
                    # 子元素
                    children.append(self.parse_element_statement())
                else:
                    # 跳过未知标记
                    self.advance()
            else:
                # 跳过未知标记
                self.advance()
        
        self.expect(TokenType.RBRACE)
        return ElementNode(tag_name, attributes, children, index, 0, 0)
    
    def parse_text_statement(self) -> TextNode:
        """解析文本语句"""
        self.expect(TokenType.TEXT)
        self.expect(TokenType.COLON)
        content = self.parse_attribute_value()
        self.expect(TokenType.SEMICOLON)
        
        return TextNode(content)
    
    def parse_style_statement(self) -> StyleNode:
        """解析样式语句"""
        self.expect(TokenType.STYLE)
        self.expect(TokenType.LBRACE)
        
        properties = self.parse_style_properties()
        
        self.expect(TokenType.RBRACE)
        return StyleNode(properties)
    
    def parse_script_statement(self) -> ScriptNode:
        """解析脚本语句"""
        self.expect(TokenType.SCRIPT)
        self.expect(TokenType.LBRACE)
        
        content = self.parse_script_content()
        
        self.expect(TokenType.RBRACE)
        return ScriptNode(content)
    
    def parse_style_properties(self) -> Dict[str, Any]:
        """解析样式属性"""
        properties = {}
        
        while not self.match(TokenType.RBRACE, TokenType.EOF):
            if self.match(TokenType.AT_SYMBOL):
                # 处理@Style引用
                self.advance()  # 跳过@
                if self.match(TokenType.STYLE):
                    self.advance()  # 跳过Style
                    template_name = self.expect(TokenType.IDENTIFIER).value
                    
                    # 检查是否有命名空间访问语法（如 UI.ButtonStyle）
                    if self.match(TokenType.DOT):
                        self.advance()  # 跳过点号
                        template_name += "." + self.expect(TokenType.IDENTIFIER).value
                    
                    self.expect(TokenType.SEMICOLON)
                    # 这里应该展开模板，暂时跳过
                    continue
            elif self.current_token().type == TokenType.IDENTIFIER:
                # 检查是否是约束语法（如 ButtonStyle except）
                template_name = self.current_token().value
                next_token = self.peek_token()
                if next_token.type == TokenType.EXCEPT or (hasattr(next_token.type, 'value') and next_token.type.value == 'EXCEPT'):
                    # 解析约束语法
                    self.advance()  # 跳过标识符
                    self.advance()  # 跳过except
                    
                    # 解析约束类型
                    constraint_type = "exact"  # 默认精确约束
                    if self.match(TokenType.IDENTIFIER):
                        constraint_type = self.advance().value
                    
                    # 解析约束内容
                    self.expect(TokenType.LBRACE)
                    constraint_properties = {}
                    while not self.match(TokenType.RBRACE, TokenType.EOF):
                        if self.match(TokenType.IDENTIFIER):
                            # 解析约束属性
                            constraint_prop_name = self.advance().value
                            while self.match(TokenType.MINUS) and (self.peek_token().type == TokenType.IDENTIFIER or self.peek_token().type == TokenType.STYLE):
                                self.advance()  # 跳过连字符
                                constraint_prop_name += "-" + self.advance().value
                            
                            self.expect(TokenType.COLON)
                            constraint_prop_value = self.parse_style_value()
                            constraint_properties[constraint_prop_name] = constraint_prop_value
                            self.expect(TokenType.SEMICOLON)
                        else:
                            self.advance()
                    self.expect(TokenType.RBRACE)
                    
                    # 创建约束信息
                    constraint_info = {
                        'type': 'constraint',
                        'template_name': template_name,
                        'constraint_type': constraint_type,
                        'properties': constraint_properties
                    }
                    properties['_constraint'] = constraint_info
                    continue
                else:
                    # 解析普通属性名（可能包含连字符）
                    prop_name = self.advance().value
                    while self.match(TokenType.MINUS) and (self.peek_token().type == TokenType.IDENTIFIER or self.peek_token().type == TokenType.STYLE):
                        self.advance()  # 跳过连字符
                        prop_name += "-" + self.advance().value
                    
                    self.expect(TokenType.COLON)
                    prop_value = self.parse_style_value()
                    properties[prop_name] = prop_value
                    self.expect(TokenType.SEMICOLON)
            else:
                # 跳过未知标记
                self.advance()
        
        return properties
    
    def parse_var_properties(self) -> Dict[str, Any]:
        """解析变量属性"""
        variables = {}
        
        while not self.match(TokenType.RBRACE, TokenType.EOF):
            if self.match(TokenType.IDENTIFIER):
                var_name = self.advance().value
                self.expect(TokenType.COLON)
                var_value = self.parse_attribute_value()
                variables[var_name] = var_value
                if self.match(TokenType.COMMA):
                    self.advance()
            else:
                # 跳过未知标记
                self.advance()
        
        return variables
    
    def parse_element_block(self) -> List[CHTLNode]:
        """解析元素块"""
        elements = []
        
        while not self.match(TokenType.RBRACE, TokenType.EOF):
            if self.match(TokenType.IDENTIFIER):
                elements.append(self.parse_element_statement())
            else:
                # 跳过未知标记
                self.advance()
        
        return elements
    
    def parse_attribute_value(self) -> Any:
        """解析属性值"""
        token = self.current_token()
        
        if token.type == TokenType.STRING:
            return self.advance().value.strip('"\'')
        elif token.type == TokenType.NUMBER:
            # 处理带单位的数值（如 10px, 20px）
            number = self.advance().value
            if self.match(TokenType.IDENTIFIER):
                unit = self.advance().value
                return f"{number}{unit}"
            else:
                return float(number)
        elif token.type == TokenType.LITERAL:
            return self.advance().value
        elif token.type == TokenType.IDENTIFIER:
            # 可能是函数调用
            func_name = self.advance().value
            if self.match(TokenType.LPAREN):
                # 解析函数调用
                self.advance()  # 跳过左括号
                args = []
                while not self.match(TokenType.RPAREN, TokenType.EOF):
                    if self.match(TokenType.NUMBER):
                        args.append(float(self.advance().value))
                    elif self.match(TokenType.COMMA):
                        self.advance()  # 跳过逗号
                    else:
                        self.advance()  # 跳过其他标记
                if self.match(TokenType.RPAREN):
                    self.advance()  # 跳过右括号
                return f"{func_name}({', '.join(map(str, args))})"
            else:
                return func_name
        else:
            # 跳过未知标记
            self.advance()
            return None
    
    def parse_style_value(self) -> str:
        """解析样式值，支持多个值（如 padding: 10px 20px）"""
        values = []
        
        while True:
            token = self.current_token()
            
            if token.type == TokenType.NUMBER:
                # 处理带单位的数值
                number = self.advance().value
                if self.match(TokenType.IDENTIFIER):
                    unit = self.advance().value
                    values.append(f"{number}{unit}")
                else:
                    values.append(number)
            elif token.type == TokenType.IDENTIFIER:
                # 处理标识符或函数调用
                func_name = self.advance().value
                if self.match(TokenType.LPAREN):
                    # 解析函数调用
                    self.advance()  # 跳过左括号
                    args = []
                    while not self.match(TokenType.RPAREN, TokenType.EOF):
                        if self.match(TokenType.NUMBER):
                            args.append(self.advance().value)
                        elif self.match(TokenType.COMMA):
                            self.advance()  # 跳过逗号
                        else:
                            self.advance()  # 跳过其他标记
                    if self.match(TokenType.RPAREN):
                        self.advance()  # 跳过右括号
                    values.append(f"{func_name}({', '.join(args)})")
                else:
                    values.append(func_name)
            elif token.type == TokenType.STRING:
                # 处理字符串
                values.append(self.advance().value.strip('"\''))
            elif token.type == TokenType.LITERAL:
                # 处理字面量
                values.append(self.advance().value)
            elif token.type == TokenType.MINUS:
                # 处理连字符（如 box-shadow 中的 -）
                values.append(self.advance().value)
            elif token.type == TokenType.COMMA:
                # 处理逗号分隔符
                self.advance()
                values.append(",")
            elif token.type == TokenType.COLON:
                # 跳过冒号（样式值中不应该有冒号）
                self.advance()
            else:
                # 遇到分号或其他结束标记，停止解析
                break
        
        return " ".join(values)
    
    def parse_script_content(self) -> str:
        """解析脚本内容"""
        content = ""
        
        while not self.match(TokenType.RBRACE, TokenType.EOF):
            token = self.advance()
            content += token.value + " "
        
        return content.strip()
    
    def parse_brace_block(self):
        """解析大括号块（跳过）"""
        brace_count = 1
        while brace_count > 0 and not self.match(TokenType.EOF):
            token = self.advance()
            if token.type == TokenType.LBRACE:
                brace_count += 1
            elif token.type == TokenType.RBRACE:
                brace_count -= 1

class CHTLGenerator(CHTLVisitor):
    """CHTL代码生成器"""
    
    def __init__(self, context: CHTLContext):
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
    
    def visit_script(self, node: ScriptNode) -> Any:
        """访问脚本节点"""
        if node.content:
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
        try:
            from use_directive_system import UseDirectiveProcessor
            system = UseDirectiveProcessor()
            return system.generate_directive_code(node.directive, node.args)
        except ImportError:
            # 回退到基本实现
            if node.directive == "html5":
                return '<!DOCTYPE html>\n<html lang="en">\n<head>\n    <meta charset="UTF-8">\n    <meta name="viewport" content="width=device-width, initial-scale=1.0">\n    <title>Document</title>\n</head>\n<body>\n    <!-- Content goes here -->\n</body>\n</html>'
            return f"<!-- Use directive: {node.directive} -->"

def main():
    """测试词法分析器、语法分析器和代码生成器"""
    lexer = CHTLLexer()
    
    test_code = """
    use html5;
    
    [Template] @Style BoxStyle {
        color: white;
        background-color: rgb(255, 192, 203);
    }
    
    html {
        head {
            title {
                text: "Hello World";
            }
        }
        body {
            div {
                class: box;
                style {
                    @Style BoxStyle;
                }
            }
        }
    }
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = CHTLParser(tokens)
    ast = parser.parse()
    
    print("代码生成...")
    generator = CHTLGenerator(parser.context)
    html = generator.generate(ast)
    
    print("生成的HTML:")
    print(html)

if __name__ == "__main__":
    main()