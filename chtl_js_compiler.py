#!/usr/bin/env python3
"""
CHTL JS编译器核心实现
基于CHTL.md规范实现完整的CHTL JS编译器

核心组件：
- CHTLJSLexer: CHTL JS词法分析器
- CHTLJSParser: CHTL JS语法分析器  
- CHTLJSGenerator: CHTL JS代码生成器
- CHTLJSContext: CHTL JS编译上下文
- CHTLJSNode: CHTL JS AST节点系统
"""

import re
import json
from typing import Dict, List, Tuple, Optional, Union, Any
from dataclasses import dataclass, field
from enum import Enum
from abc import ABC, abstractmethod

class CHTLJSTokenType(Enum):
    """CHTL JS标记类型"""
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
    AMPERSAND_ARROW = "AMPERSAND_ARROW"  # &->
    
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
    PIPE = "PIPE"          # |
    
    # 关键字
    SCRIPT_LOADER = "SCRIPT_LOADER"
    LISTEN = "LISTEN"
    ANIMATE = "ANIMATE"
    DELEGATE = "DELEGATE"
    ROUTER = "ROUTER"
    VIR = "VIR"
    SCRIPT = "SCRIPT"
    FUNCTION = "FUNCTION"
    RETURN = "RETURN"
    CONST = "CONST"
    LET = "LET"
    VAR = "VAR"
    IF = "IF"
    FOR = "FOR"
    WHILE = "WHILE"
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
    UNDERSCORE = "UNDERSCORE"  # _
    
    # CHTL JS特殊语法
    ENHANCED_SELECTOR = "ENHANCED_SELECTOR"  # {{...}}
    RESPONSIVE_VALUE = "RESPONSIVE_VALUE"    # $...$
    VIRTUAL_OBJECT = "VIRTUAL_OBJECT"        # Vir
    
    # 注释
    COMMENT_LINE = "COMMENT_LINE"    # //
    COMMENT_BLOCK = "COMMENT_BLOCK"  # /* */
    GENERATOR_COMMENT = "GENERATOR_COMMENT"  # #
    
    # 结束标记
    EOF = "EOF"
    NEWLINE = "NEWLINE"
    WHITESPACE = "WHITESPACE"

@dataclass
class CHTLJSToken:
    """CHTL JS标记数据结构"""
    type: CHTLJSTokenType
    value: str
    line: int
    column: int
    position: int

@dataclass
class CHTLJSContext:
    """CHTL JS编译上下文"""
    # 虚对象存储
    virtual_objects: Dict[str, 'VirtualObjectNode'] = field(default_factory=dict)
    
    # 路由存储
    routes: List['RouterNode'] = field(default_factory=list)
    
    # 脚本加载器
    script_loaders: List['ScriptLoaderNode'] = field(default_factory=list)
    
    # 响应式值
    responsive_values: Dict[str, str] = field(default_factory=dict)
    
    # 增强选择器
    enhanced_selectors: List[str] = field(default_factory=list)
    
    # 当前作用域
    current_scope: List[str] = field(default_factory=list)

class CHTLJSNode(ABC):
    """CHTL JS AST节点基类"""
    
    def __init__(self, line: int = 0, column: int = 0):
        self.line = line
        self.column = column
    
    @abstractmethod
    def accept(self, visitor: 'CHTLJSVisitor') -> Any:
        """接受访问者模式"""
        pass
    
    @abstractmethod
    def to_dict(self) -> Dict[str, Any]:
        """转换为字典表示"""
        pass

class ScriptLoaderNode(CHTLJSNode):
    """脚本加载器节点"""
    
    def __init__(self, files: List[str], line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.files = files
    
    def accept(self, visitor: 'CHTLJSVisitor') -> Any:
        return visitor.visit_script_loader(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'ScriptLoaderNode',
            'files': self.files,
            'line': self.line,
            'column': self.column
        }

class ListenNode(CHTLJSNode):
    """监听器节点"""
    
    def __init__(self, target: str, events: Dict[str, str], line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.target = target
        self.events = events
    
    def accept(self, visitor: 'CHTLJSVisitor') -> Any:
        return visitor.visit_listen(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'ListenNode',
            'target': self.target,
            'events': self.events,
            'line': self.line,
            'column': self.column
        }

class AnimateNode(CHTLJSNode):
    """动画节点"""
    
    def __init__(self, target: str, duration: int = 1000, easing: str = "ease-in-out",
                 begin: Dict[str, Any] = None, when: List[Dict[str, Any]] = None,
                 end: Dict[str, Any] = None, loop: int = 1, direction: str = "normal",
                 delay: int = 0, callback: str = None, line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.target = target
        self.duration = duration
        self.easing = easing
        self.begin = begin or {}
        self.when = when or []
        self.end = end or {}
        self.loop = loop
        self.direction = direction
        self.delay = delay
        self.callback = callback
    
    def accept(self, visitor: 'CHTLJSVisitor') -> Any:
        return visitor.visit_animate(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'AnimateNode',
            'target': self.target,
            'duration': self.duration,
            'easing': self.easing,
            'begin': self.begin,
            'when': self.when,
            'end': self.end,
            'loop': self.loop,
            'direction': self.direction,
            'delay': self.delay,
            'callback': self.callback,
            'line': self.line,
            'column': self.column
        }

class DelegateNode(CHTLJSNode):
    """事件委托节点"""
    
    def __init__(self, parent: str, target: Union[str, List[str]], 
                 events: Dict[str, str], line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.parent = parent
        self.target = target
        self.events = events
    
    def accept(self, visitor: 'CHTLJSVisitor') -> Any:
        return visitor.visit_delegate(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'DelegateNode',
            'parent': self.parent,
            'target': self.target,
            'events': self.events,
            'line': self.line,
            'column': self.column
        }

class RouterNode(CHTLJSNode):
    """路由节点"""
    
    def __init__(self, url: Union[str, List[str]], page: Union[str, List[str]], 
                 root: str = None, mode: str = "hash", line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.url = url
        self.page = page
        self.root = root
        self.mode = mode
    
    def accept(self, visitor: 'CHTLJSVisitor') -> Any:
        return visitor.visit_router(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'RouterNode',
            'url': self.url,
            'page': self.page,
            'root': self.root,
            'mode': self.mode,
            'line': self.line,
            'column': self.column
        }

class VirtualObjectNode(CHTLJSNode):
    """虚对象节点"""
    
    def __init__(self, name: str, content: Dict[str, Any], line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.name = name
        self.content = content
    
    def accept(self, visitor: 'CHTLJSVisitor') -> Any:
        return visitor.visit_virtual_object(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'VirtualObjectNode',
            'name': self.name,
            'content': self.content,
            'line': self.line,
            'column': self.column
        }

class EnhancedSelectorNode(CHTLJSNode):
    """增强选择器节点"""
    
    def __init__(self, selector: str, line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.selector = selector
    
    def accept(self, visitor: 'CHTLJSVisitor') -> Any:
        return visitor.visit_enhanced_selector(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'EnhancedSelectorNode',
            'selector': self.selector,
            'line': self.line,
            'column': self.column
        }

class ResponsiveValueNode(CHTLJSNode):
    """响应式值节点"""
    
    def __init__(self, variable: str, line: int = 0, column: int = 0):
        super().__init__(line, column)
        self.variable = variable
    
    def accept(self, visitor: 'CHTLJSVisitor') -> Any:
        return visitor.visit_responsive_value(self)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'type': 'ResponsiveValueNode',
            'variable': self.variable,
            'line': self.line,
            'column': self.column
        }

class CHTLJSVisitor(ABC):
    """CHTL JS访问者模式基类"""
    
    @abstractmethod
    def visit_script_loader(self, node: ScriptLoaderNode) -> Any:
        pass
    
    @abstractmethod
    def visit_listen(self, node: ListenNode) -> Any:
        pass
    
    @abstractmethod
    def visit_animate(self, node: AnimateNode) -> Any:
        pass
    
    @abstractmethod
    def visit_delegate(self, node: DelegateNode) -> Any:
        pass
    
    @abstractmethod
    def visit_router(self, node: RouterNode) -> Any:
        pass
    
    @abstractmethod
    def visit_virtual_object(self, node: VirtualObjectNode) -> Any:
        pass
    
    @abstractmethod
    def visit_enhanced_selector(self, node: EnhancedSelectorNode) -> Any:
        pass
    
    @abstractmethod
    def visit_responsive_value(self, node: ResponsiveValueNode) -> Any:
        pass

class CHTLJSLexer:
    """CHTL JS词法分析器"""
    
    def __init__(self):
        # 关键字映射
        self.keywords = {
            'scriptloader': CHTLJSTokenType.SCRIPT_LOADER,
            'listen': CHTLJSTokenType.LISTEN,
            'animate': CHTLJSTokenType.ANIMATE,
            'delegate': CHTLJSTokenType.DELEGATE,
            'router': CHTLJSTokenType.ROUTER,
            'vir': CHTLJSTokenType.VIR,
            'script': CHTLJSTokenType.SCRIPT,
            'function': CHTLJSTokenType.FUNCTION,
            'return': CHTLJSTokenType.RETURN,
            'const': CHTLJSTokenType.CONST,
            'let': CHTLJSTokenType.LET,
            'var': CHTLJSTokenType.VAR,
            'if': CHTLJSTokenType.IF,
            'for': CHTLJSTokenType.FOR,
            'while': CHTLJSTokenType.WHILE,
            'class': CHTLJSTokenType.CLASS,
            'extends': CHTLJSTokenType.EXTENDS,
            'implements': CHTLJSTokenType.IMPLEMENTS,
            'public': CHTLJSTokenType.PUBLIC,
            'private': CHTLJSTokenType.PRIVATE,
            'protected': CHTLJSTokenType.PROTECTED,
            'static': CHTLJSTokenType.STATIC,
            'final': CHTLJSTokenType.FINAL,
            'abstract': CHTLJSTokenType.ABSTRACT,
            'interface': CHTLJSTokenType.INTERFACE,
            'package': CHTLJSTokenType.PACKAGE,
            'namespace': CHTLJSTokenType.NAMESPACE,
        }
        
        # 符号映射
        self.symbols = {
            '{': CHTLJSTokenType.LBRACE,
            '}': CHTLJSTokenType.RBRACE,
            '(': CHTLJSTokenType.LPAREN,
            ')': CHTLJSTokenType.RPAREN,
            '[': CHTLJSTokenType.LBRACKET,
            ']': CHTLJSTokenType.RBRACKET,
            ':': CHTLJSTokenType.COLON,
            ';': CHTLJSTokenType.SEMICOLON,
            ',': CHTLJSTokenType.COMMA,
            '.': CHTLJSTokenType.DOT,
            '=': CHTLJSTokenType.EQUALS,
            '->': CHTLJSTokenType.ARROW,
            '=>': CHTLJSTokenType.ARROW,
            '&->': CHTLJSTokenType.AMPERSAND_ARROW,
            '+': CHTLJSTokenType.PLUS,
            '-': CHTLJSTokenType.MINUS,
            '*': CHTLJSTokenType.MULTIPLY,
            '/': CHTLJSTokenType.DIVIDE,
            '%': CHTLJSTokenType.MODULO,
            '**': CHTLJSTokenType.POWER,
            '&&': CHTLJSTokenType.AND,
            '||': CHTLJSTokenType.OR,
            '!': CHTLJSTokenType.NOT,
            '>': CHTLJSTokenType.GT,
            '<': CHTLJSTokenType.LT,
            '>=': CHTLJSTokenType.GE,
            '<=': CHTLJSTokenType.LE,
            '==': CHTLJSTokenType.EQ,
            '!=': CHTLJSTokenType.NE,
            '?': CHTLJSTokenType.QUESTION,
            '|': CHTLJSTokenType.PIPE,
            '@': CHTLJSTokenType.AT_SYMBOL,
            '#': CHTLJSTokenType.HASH,
            '$': CHTLJSTokenType.DOLLAR,
            '&': CHTLJSTokenType.AMPERSAND,
            '~': CHTLJSTokenType.TILDE,
            '^': CHTLJSTokenType.CARET,
            '_': CHTLJSTokenType.UNDERSCORE,
        }
    
    def tokenize(self, source: str) -> List[CHTLJSToken]:
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
                    tokens.append(CHTLJSToken(
                        CHTLJSTokenType.COMMENT_LINE,
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
                    tokens.append(CHTLJSToken(
                        CHTLJSTokenType.COMMENT_BLOCK,
                        source[comment_start:position],
                        line, column, position
                    ))
                    continue
            
            # 处理生成器注释
            if char == '#':
                comment_start = position
                position += 1
                while position < len(source) and source[position] != '\n':
                    position += 1
                tokens.append(CHTLJSToken(
                    CHTLJSTokenType.GENERATOR_COMMENT,
                    source[comment_start:position],
                    line, column, position
                ))
                continue
            
            # 处理增强选择器 {{...}}
            if char == '{' and position + 1 < len(source) and source[position + 1] == '{':
                selector_start = position
                position += 2
                while position + 1 < len(source):
                    if source[position] == '}' and source[position + 1] == '}':
                        position += 2
                        break
                    position += 1
                tokens.append(CHTLJSToken(
                    CHTLJSTokenType.ENHANCED_SELECTOR,
                    source[selector_start:position],
                    line, column, position
                ))
                continue
            
            # 处理响应式值 $...$
            if char == '$':
                value_start = position
                position += 1
                while position < len(source) and source[position] != '$':
                    position += 1
                if position < len(source):
                    position += 1  # 跳过结束$
                tokens.append(CHTLJSToken(
                    CHTLJSTokenType.RESPONSIVE_VALUE,
                    source[value_start:position],
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
                tokens.append(CHTLJSToken(
                    CHTLJSTokenType.STRING,
                    source[string_start:position],
                    line, column, position
                ))
                continue
            
            # 处理数字
            if char.isdigit() or char == '.':
                number_start = position
                while position < len(source) and (source[position].isdigit() or source[position] == '.'):
                    position += 1
                tokens.append(CHTLJSToken(
                    CHTLJSTokenType.NUMBER,
                    source[number_start:position],
                    line, column, position
                ))
                continue
            
            # 处理标识符和关键字
            if char.isalpha() or char == '_':
                identifier_start = position
                while position < len(source) and (source[position].isalnum() or source[position] == '_'):
                    position += 1
                identifier = source[identifier_start:position]
                
                # 检查是否是关键字
                token_type = self.keywords.get(identifier.lower(), CHTLJSTokenType.IDENTIFIER)
                tokens.append(CHTLJSToken(token_type, identifier, line, column, position))
                continue
            
            # 处理符号
            # 先检查三字符符号
            if position + 2 < len(source):
                three_char = source[position:position + 3]
                if three_char in self.symbols:
                    tokens.append(CHTLJSToken(
                        self.symbols[three_char],
                        three_char,
                        line, column, position
                    ))
                    position += 3
                    column += 3
                    continue
            
            # 先检查双字符符号
            if position + 1 < len(source):
                two_char = source[position:position + 2]
                if two_char in self.symbols:
                    tokens.append(CHTLJSToken(
                        self.symbols[two_char],
                        two_char,
                        line, column, position
                    ))
                    position += 2
                    column += 2
                    continue
            
            # 检查单字符符号
            if char in self.symbols:
                tokens.append(CHTLJSToken(
                    self.symbols[char],
                    char,
                    line, column, position
                ))
                position += 1
                column += 1
                continue
            
            # 处理无修饰字面量（以字母开头，不包含特殊字符的连续字符）
            if char.isalpha():
                literal_start = position
                while position < len(source) and (source[position].isalnum() or source[position] in ['-', '_']):
                    position += 1
                tokens.append(CHTLJSToken(
                    CHTLJSTokenType.LITERAL,
                    source[literal_start:position],
                    line, column, position
                ))
                continue
            
            # 未知字符，跳过
            position += 1
            column += 1
        
        # 添加结束标记
        tokens.append(CHTLJSToken(CHTLJSTokenType.EOF, "", line, column, position))
        
        return tokens

class CHTLJSParser:
    """CHTL JS语法分析器"""
    
    def __init__(self, tokens: List[CHTLJSToken]):
        self.tokens = tokens
        self.position = 0
        self.context = CHTLJSContext()
    
    def current_token(self) -> CHTLJSToken:
        """获取当前标记"""
        if self.position >= len(self.tokens):
            return self.tokens[-1]  # 返回EOF标记
        return self.tokens[self.position]
    
    def advance(self) -> CHTLJSToken:
        """前进到下一个标记"""
        if self.position < len(self.tokens):
            token = self.tokens[self.position]
            self.position += 1
            return token
        return self.tokens[-1]  # 返回EOF标记
    
    def peek(self, offset: int = 1) -> CHTLJSToken:
        """查看指定偏移的标记"""
        peek_pos = self.position + offset
        if peek_pos >= len(self.tokens):
            return self.tokens[-1]  # 返回EOF标记
        return self.tokens[peek_pos]
    
    def match(self, *token_types: CHTLJSTokenType) -> bool:
        """检查当前标记是否匹配指定类型"""
        return self.current_token().type in token_types
    
    def expect(self, token_type: CHTLJSTokenType) -> CHTLJSToken:
        """期望指定类型的标记"""
        if self.match(token_type):
            return self.advance()
        else:
            current = self.current_token()
            raise SyntaxError(f"Expected {token_type.value}, got {current.type.value} at line {current.line}, column {current.column}")
    
    def parse(self) -> List[CHTLJSNode]:
        """解析主方法"""
        nodes = []
        
        while not self.match(CHTLJSTokenType.EOF):
            node = self.parse_statement()
            if node:
                nodes.append(node)
        
        return nodes
    
    def parse_statement(self) -> Optional[CHTLJSNode]:
        """解析语句"""
        token = self.current_token()
        
        if token.type == CHTLJSTokenType.SCRIPT_LOADER:
            return self.parse_script_loader()
        elif token.type == CHTLJSTokenType.LISTEN:
            return self.parse_listen()
        elif token.type == CHTLJSTokenType.ANIMATE:
            return self.parse_animate()
        elif token.type == CHTLJSTokenType.DELEGATE:
            return self.parse_delegate()
        elif token.type == CHTLJSTokenType.ROUTER:
            return self.parse_router()
        elif token.type == CHTLJSTokenType.VIR:
            return self.parse_virtual_object()
        elif token.type == CHTLJSTokenType.ENHANCED_SELECTOR:
            # 检查是否是选择器后跟箭头的情况
            if self.peek().type == CHTLJSTokenType.ARROW:
                return self.parse_selector_with_arrow()
            else:
                return self.parse_enhanced_selector()
        elif token.type == CHTLJSTokenType.RESPONSIVE_VALUE:
            return self.parse_responsive_value()
        else:
            # 跳过未知标记
            self.advance()
            return None
    
    def parse_selector_with_arrow(self) -> Optional[CHTLJSNode]:
        """解析选择器后跟箭头的情况"""
        # 解析选择器
        selector = ""
        if self.match(CHTLJSTokenType.ENHANCED_SELECTOR):
            selector = self.advance().value
        elif self.match(CHTLJSTokenType.IDENTIFIER):
            selector = self.advance().value
        else:
            raise SyntaxError(f"Expected selector, got {self.current_token().type.value}")
        
        # 解析箭头
        self.expect(CHTLJSTokenType.ARROW)
        
        # 检查后面的关键字
        next_token = self.current_token()
        if next_token.type == CHTLJSTokenType.LISTEN:
            return self.parse_listen_with_selector(selector)
        elif next_token.type == CHTLJSTokenType.ANIMATE:
            return self.parse_animate_with_selector(selector)
        else:
            raise SyntaxError(f"Expected LISTEN or ANIMATE after arrow, got {next_token.type.value}")
    
    def parse_listen_with_selector(self, selector: str) -> ListenNode:
        """解析带选择器的监听器"""
        self.expect(CHTLJSTokenType.LISTEN)
        self.expect(CHTLJSTokenType.LBRACE)
        
        events = {}
        while not self.match(CHTLJSTokenType.RBRACE, CHTLJSTokenType.EOF):
            if self.match(CHTLJSTokenType.IDENTIFIER):
                event_name = self.advance().value
                self.expect(CHTLJSTokenType.COLON)
                
                # 解析事件处理函数
                handler = self.parse_function_expression()
                events[event_name] = handler
                
                if self.match(CHTLJSTokenType.COMMA):
                    self.advance()
            else:
                self.advance()
        
        self.expect(CHTLJSTokenType.RBRACE)
        
        node = ListenNode(selector, events)
        return node
    
    def parse_animate_with_selector(self, selector: str) -> AnimateNode:
        """解析带选择器的动画"""
        self.expect(CHTLJSTokenType.ANIMATE)
        self.expect(CHTLJSTokenType.LBRACE)
        
        # 解析动画属性
        duration = 1000
        easing = "ease-in-out"
        begin = {}
        when = []
        end = {}
        loop = 1
        direction = "normal"
        delay = 0
        callback = None
        
        while not self.match(CHTLJSTokenType.RBRACE, CHTLJSTokenType.EOF):
            if self.match(CHTLJSTokenType.IDENTIFIER):
                prop_name = self.advance().value
                self.expect(CHTLJSTokenType.COLON)
                
                if prop_name == "duration":
                    duration = int(self.expect(CHTLJSTokenType.NUMBER).value)
                elif prop_name == "easing":
                    easing = self.expect(CHTLJSTokenType.STRING).value.strip('"\'')
                elif prop_name == "begin":
                    begin = self.parse_object()
                elif prop_name == "when":
                    when = self.parse_array()
                elif prop_name == "end":
                    end = self.parse_object()
                elif prop_name == "loop":
                    loop = int(self.expect(CHTLJSTokenType.NUMBER).value)
                elif prop_name == "direction":
                    direction = self.expect(CHTLJSTokenType.STRING).value.strip('"\'')
                elif prop_name == "delay":
                    delay = int(self.expect(CHTLJSTokenType.NUMBER).value)
                elif prop_name == "callback":
                    callback = self.expect(CHTLJSTokenType.STRING).value.strip('"\'')
                
                if self.match(CHTLJSTokenType.COMMA):
                    self.advance()
            else:
                self.advance()
        
        self.expect(CHTLJSTokenType.RBRACE)
        
        node = AnimateNode(selector, duration, easing, begin, when, end, loop, direction, delay, callback)
        return node
    
    def parse_script_loader(self) -> ScriptLoaderNode:
        """解析脚本加载器"""
        self.expect(CHTLJSTokenType.SCRIPT_LOADER)
        self.expect(CHTLJSTokenType.LBRACE)
        
        files = []
        while not self.match(CHTLJSTokenType.RBRACE, CHTLJSTokenType.EOF):
            if self.match(CHTLJSTokenType.IDENTIFIER):
                if self.advance().value == "load":
                    self.expect(CHTLJSTokenType.COLON)
                    file_path = self.expect(CHTLJSTokenType.STRING).value.strip('"\'')
                    files.append(file_path)
                    self.expect(CHTLJSTokenType.SEMICOLON)
                else:
                    self.advance()
            else:
                self.advance()
        
        self.expect(CHTLJSTokenType.RBRACE)
        
        node = ScriptLoaderNode(files)
        self.context.script_loaders.append(node)
        return node
    
    def parse_listen(self) -> ListenNode:
        """解析监听器"""
        # 解析选择器
        selector = ""
        if self.match(CHTLJSTokenType.ENHANCED_SELECTOR):
            selector = self.advance().value
        elif self.match(CHTLJSTokenType.IDENTIFIER):
            selector = self.advance().value
        else:
            raise SyntaxError(f"Expected selector, got {self.current_token().type.value}")
        
        # 解析箭头
        self.expect(CHTLJSTokenType.ARROW)
        self.expect(CHTLJSTokenType.LISTEN)
        self.expect(CHTLJSTokenType.LBRACE)
        
        events = {}
        while not self.match(CHTLJSTokenType.RBRACE, CHTLJSTokenType.EOF):
            if self.match(CHTLJSTokenType.IDENTIFIER):
                event_name = self.advance().value
                self.expect(CHTLJSTokenType.COLON)
                
                # 解析事件处理函数
                handler = self.parse_function_expression()
                events[event_name] = handler
                
                if self.match(CHTLJSTokenType.COMMA):
                    self.advance()
            else:
                self.advance()
        
        self.expect(CHTLJSTokenType.RBRACE)
        
        node = ListenNode(selector, events)
        return node
    
    def parse_animate(self) -> AnimateNode:
        """解析动画"""
        # 解析选择器
        selector = ""
        if self.match(CHTLJSTokenType.ENHANCED_SELECTOR):
            selector = self.advance().value
        elif self.match(CHTLJSTokenType.IDENTIFIER):
            selector = self.advance().value
        else:
            raise SyntaxError(f"Expected selector, got {self.current_token().type.value}")
        
        # 解析箭头
        self.expect(CHTLJSTokenType.ARROW)
        self.expect(CHTLJSTokenType.ANIMATE)
        self.expect(CHTLJSTokenType.LBRACE)
        
        # 解析动画属性
        duration = 1000
        easing = "ease-in-out"
        begin = {}
        when = []
        end = {}
        loop = 1
        direction = "normal"
        delay = 0
        callback = None
        
        while not self.match(CHTLJSTokenType.RBRACE, CHTLJSTokenType.EOF):
            if self.match(CHTLJSTokenType.IDENTIFIER):
                prop_name = self.advance().value
                self.expect(CHTLJSTokenType.COLON)
                
                if prop_name == "duration":
                    duration = int(self.expect(CHTLJSTokenType.NUMBER).value)
                elif prop_name == "easing":
                    easing = self.expect(CHTLJSTokenType.STRING).value.strip('"\'')
                elif prop_name == "begin":
                    begin = self.parse_object()
                elif prop_name == "when":
                    when = self.parse_array()
                elif prop_name == "end":
                    end = self.parse_object()
                elif prop_name == "loop":
                    loop = int(self.expect(CHTLJSTokenType.NUMBER).value)
                elif prop_name == "direction":
                    direction = self.expect(CHTLJSTokenType.STRING).value.strip('"\'')
                elif prop_name == "delay":
                    delay = int(self.expect(CHTLJSTokenType.NUMBER).value)
                elif prop_name == "callback":
                    callback = self.expect(CHTLJSTokenType.STRING).value.strip('"\'')
                
                if self.match(CHTLJSTokenType.COMMA):
                    self.advance()
            else:
                self.advance()
        
        self.expect(CHTLJSTokenType.RBRACE)
        
        node = AnimateNode(selector, duration, easing, begin, when, end, loop, direction, delay, callback)
        return node
    
    def parse_delegate(self) -> DelegateNode:
        """解析事件委托"""
        self.expect(CHTLJSTokenType.DELEGATE)
        self.expect(CHTLJSTokenType.LBRACE)
        
        parent = ""
        target = []
        events = {}
        
        while not self.match(CHTLJSTokenType.RBRACE, CHTLJSTokenType.EOF):
            if self.match(CHTLJSTokenType.IDENTIFIER):
                prop_name = self.advance().value
                self.expect(CHTLJSTokenType.COLON)
                
                if prop_name == "parent":
                    parent = self.expect(CHTLJSTokenType.STRING).value.strip('"\'')
                elif prop_name == "target":
                    target = self.parse_array()
                elif prop_name == "events":
                    events = self.parse_object()
                
                if self.match(CHTLJSTokenType.COMMA):
                    self.advance()
            else:
                self.advance()
        
        self.expect(CHTLJSTokenType.RBRACE)
        
        node = DelegateNode(parent, target, events)
        return node
    
    def parse_router(self) -> RouterNode:
        """解析路由"""
        self.expect(CHTLJSTokenType.ROUTER)
        self.expect(CHTLJSTokenType.LBRACE)
        
        url = []
        page = []
        root = None
        mode = "hash"
        
        while not self.match(CHTLJSTokenType.RBRACE, CHTLJSTokenType.EOF):
            if self.match(CHTLJSTokenType.IDENTIFIER):
                prop_name = self.advance().value
                self.expect(CHTLJSTokenType.COLON)
                
                if prop_name == "url":
                    url = self.parse_array()
                elif prop_name == "page":
                    page = self.parse_array()
                elif prop_name == "root":
                    root = self.expect(CHTLJSTokenType.STRING).value.strip('"\'')
                elif prop_name == "mode":
                    mode = self.expect(CHTLJSTokenType.STRING).value.strip('"\'')
                
                if self.match(CHTLJSTokenType.COMMA):
                    self.advance()
            else:
                self.advance()
        
        self.expect(CHTLJSTokenType.RBRACE)
        
        node = RouterNode(url, page, root, mode)
        self.context.routes.append(node)
        return node
    
    def parse_virtual_object(self) -> VirtualObjectNode:
        """解析虚对象"""
        self.expect(CHTLJSTokenType.VIR)
        
        name = self.expect(CHTLJSTokenType.IDENTIFIER).value
        self.expect(CHTLJSTokenType.LBRACE)
        
        content = {}
        while not self.match(CHTLJSTokenType.RBRACE, CHTLJSTokenType.EOF):
            if self.match(CHTLJSTokenType.IDENTIFIER):
                key = self.advance().value
                self.expect(CHTLJSTokenType.COLON)
                
                if self.match(CHTLJSTokenType.STRING):
                    value = self.advance().value.strip('"\'')
                elif self.match(CHTLJSTokenType.NUMBER):
                    value = float(self.advance().value)
                elif self.match(CHTLJSTokenType.LBRACE):
                    value = self.parse_object()
                elif self.match(CHTLJSTokenType.LBRACKET):
                    value = self.parse_array()
                else:
                    value = self.advance().value
                
                content[key] = value
                
                if self.match(CHTLJSTokenType.SEMICOLON):
                    self.advance()
            else:
                self.advance()
        
        self.expect(CHTLJSTokenType.RBRACE)
        
        node = VirtualObjectNode(name, content)
        self.context.virtual_objects[name] = node
        return node
    
    def parse_enhanced_selector(self) -> EnhancedSelectorNode:
        """解析增强选择器"""
        selector = self.expect(CHTLJSTokenType.ENHANCED_SELECTOR).value
        node = EnhancedSelectorNode(selector)
        self.context.enhanced_selectors.append(selector)
        return node
    
    def parse_responsive_value(self) -> ResponsiveValueNode:
        """解析响应式值"""
        value = self.expect(CHTLJSTokenType.RESPONSIVE_VALUE).value
        variable = value.strip('$')
        node = ResponsiveValueNode(variable)
        self.context.responsive_values[variable] = value
        return node
    
    def parse_function_expression(self) -> str:
        """解析函数表达式"""
        if self.match(CHTLJSTokenType.LPAREN):
            # 箭头函数
            self.advance()  # 跳过左括号
            self.expect(CHTLJSTokenType.RPAREN)  # 跳过右括号
            self.expect(CHTLJSTokenType.ARROW)  # 跳过箭头
            self.expect(CHTLJSTokenType.LBRACE)  # 跳过左大括号
            
            body = ""
            brace_count = 1
            while brace_count > 0 and not self.match(CHTLJSTokenType.EOF):
                token = self.advance()
                if token.type == CHTLJSTokenType.LBRACE:
                    brace_count += 1
                elif token.type == CHTLJSTokenType.RBRACE:
                    brace_count -= 1
                body += token.value
            
            return f"() => {{{body}}}"
        else:
            # 普通函数调用
            return self.expect(CHTLJSTokenType.STRING).value.strip('"\'')
    
    def parse_object(self) -> Dict[str, Any]:
        """解析对象"""
        obj = {}
        self.expect(CHTLJSTokenType.LBRACE)
        
        while not self.match(CHTLJSTokenType.RBRACE, CHTLJSTokenType.EOF):
            if self.match(CHTLJSTokenType.IDENTIFIER):
                key = self.advance().value
                self.expect(CHTLJSTokenType.COLON)
                
                if self.match(CHTLJSTokenType.STRING):
                    value = self.advance().value.strip('"\'')
                elif self.match(CHTLJSTokenType.NUMBER):
                    value = float(self.advance().value)
                elif self.match(CHTLJSTokenType.LBRACE):
                    value = self.parse_object()
                elif self.match(CHTLJSTokenType.LBRACKET):
                    value = self.parse_array()
                elif self.match(CHTLJSTokenType.LPAREN):
                    # 处理箭头函数
                    value = self.parse_function_expression()
                else:
                    value = self.advance().value
                
                obj[key] = value
                
                if self.match(CHTLJSTokenType.COMMA):
                    self.advance()
            else:
                self.advance()
        
        self.expect(CHTLJSTokenType.RBRACE)
        return obj
    
    def parse_array(self) -> List[Any]:
        """解析数组"""
        arr = []
        self.expect(CHTLJSTokenType.LBRACKET)
        
        while not self.match(CHTLJSTokenType.RBRACKET, CHTLJSTokenType.EOF):
            if self.match(CHTLJSTokenType.STRING):
                value = self.advance().value.strip('"\'')
            elif self.match(CHTLJSTokenType.NUMBER):
                value = float(self.advance().value)
            elif self.match(CHTLJSTokenType.LBRACE):
                value = self.parse_object()
            elif self.match(CHTLJSTokenType.LBRACKET):
                value = self.parse_array()
            else:
                value = self.advance().value
            
            arr.append(value)
            
            if self.match(CHTLJSTokenType.COMMA):
                self.advance()
        
        self.expect(CHTLJSTokenType.RBRACKET)
        return arr

class CHTLJSGenerator(CHTLJSVisitor):
    """CHTL JS代码生成器"""
    
    def __init__(self, context: CHTLJSContext):
        self.context = context
        self.output = []
        self.indent_level = 0
    
    def generate(self, ast: List[CHTLJSNode]) -> str:
        """生成JavaScript代码"""
        self.output = []
        
        for node in ast:
            node.accept(self)
        
        return '\n'.join(self.output)
    
    def write(self, text: str):
        """写入代码"""
        self.output.append('  ' * self.indent_level + text)
    
    def visit_script_loader(self, node: ScriptLoaderNode) -> Any:
        """访问脚本加载器节点"""
        for file_path in node.files:
            self.write(f'// Load script: {file_path}')
            self.write(f'const script = document.createElement("script");')
            self.write(f'script.src = "{file_path}";')
            self.write(f'document.head.appendChild(script);')
            self.write('')
    
    def visit_listen(self, node: ListenNode) -> Any:
        """访问监听器节点"""
        self.write(f'// Event listener for {node.target}')
        for event_name, handler in node.events.items():
            self.write(f'document.querySelector("{node.target}").addEventListener("{event_name}", {handler});')
        self.write('')
    
    def visit_animate(self, node: AnimateNode) -> Any:
        """访问动画节点"""
        self.write(f'// Animation for {node.target}')
        self.write(f'const element = document.querySelector("{node.target}");')
        self.write(f'const animation = element.animate({{')
        self.indent_level += 1
        
        if node.begin:
            self.write(f'from: {json.dumps(node.begin)},')
        if node.end:
            self.write(f'to: {json.dumps(node.end)},')
        
        self.write(f'duration: {node.duration},')
        self.write(f'easing: "{node.easing}",')
        self.write(f'iterations: {node.loop},')
        self.write(f'direction: "{node.direction}",')
        self.write(f'delay: {node.delay}')
        
        self.indent_level -= 1
        self.write(f'}});')
        
        if node.callback:
            self.write(f'animation.addEventListener("finish", {node.callback});')
        self.write('')
    
    def visit_delegate(self, node: DelegateNode) -> Any:
        """访问事件委托节点"""
        self.write(f'// Event delegation for {node.parent}')
        self.write(f'const parent = document.querySelector("{node.parent}");')
        
        for event_name, handler in node.events.items():
            self.write(f'parent.addEventListener("{event_name}", (e) => {{')
            self.indent_level += 1
            self.write(f'if ({json.dumps(node.target)}.includes(e.target.tagName.toLowerCase())) {{')
            self.indent_level += 1
            self.write(f'{handler}')
            self.indent_level -= 1
            self.write(f'}}')
            self.indent_level -= 1
            self.write(f'}});')
        self.write('')
    
    def visit_router(self, node: RouterNode) -> Any:
        """访问路由节点"""
        self.write('// Router configuration')
        self.write('const router = {')
        self.indent_level += 1
        self.write(f'urls: {json.dumps(node.url)},')
        self.write(f'pages: {json.dumps(node.page)},')
        self.write(f'root: "{node.root or ""}",')
        self.write(f'mode: "{node.mode}"')
        self.indent_level -= 1
        self.write('};')
        self.write('')
    
    def visit_virtual_object(self, node: VirtualObjectNode) -> Any:
        """访问虚对象节点"""
        self.write(f'// Virtual object: {node.name}')
        self.write(f'const {node.name} = {json.dumps(node.content, indent=2)};')
        self.write('')
    
    def visit_enhanced_selector(self, node: EnhancedSelectorNode) -> Any:
        """访问增强选择器节点"""
        self.write(f'// Enhanced selector: {node.selector}')
        self.write('')
    
    def visit_responsive_value(self, node: ResponsiveValueNode) -> Any:
        """访问响应式值节点"""
        self.write(f'// Responsive value: {node.variable}')
        self.write('')

def main():
    """测试CHTL JS词法分析器"""
    lexer = CHTLJSLexer()
    
    test_code = """
    ScriptLoader {
        load: ./module.cjjs,
        load: ./module2.cjjs
    }
    
    {{box}}->Listen {
        click: () => {
            console.log('Box clicked!');
        }
    }
    
    let boxClass = "box";
    let boxWidth = 100;
    """
    
    tokens = lexer.tokenize(test_code)
    
    print("CHTL JS词法分析结果:")
    for token in tokens:
        if token.type != CHTLJSTokenType.WHITESPACE:
            print(f"  {token.type.value:25} | {token.value:20} | Line {token.line:3}, Col {token.column:3}")

if __name__ == "__main__":
    main()