#!/usr/bin/env python3
"""
CHTL统一扫描器实现
基于CHTL.md第2372-2716行的规范实现

核心功能：
1. 分离全局style块和局部script内部允许的部分CHTL语法
2. 完全分离JS和CHTL JS代码
3. 使用占位符机制隔离无法处理的代码
"""

import re
import json
from typing import Dict, List, Tuple, Optional, Union
from dataclasses import dataclass
from enum import Enum

class TokenType(Enum):
    """标记类型枚举"""
    CHTL_SYNTAX = "CHTL_SYNTAX"
    CHTL_JS_SYNTAX = "CHTL_JS_SYNTAX"
    CSS_SYNTAX = "CSS_SYNTAX"
    JS_SYNTAX = "JS_SYNTAX"
    PLACEHOLDER = "PLACEHOLDER"

@dataclass
class Token:
    """标记数据结构"""
    type: TokenType
    content: str
    start_pos: int
    end_pos: int
    placeholder_id: Optional[str] = None

@dataclass
class PlaceholderInfo:
    """占位符信息"""
    id: str
    original_content: str
    token_type: TokenType
    start_pos: int
    end_pos: int

class UnifiedScanner:
    """统一扫描器主类"""
    
    def __init__(self):
        self.placeholders: Dict[str, PlaceholderInfo] = {}
        self.placeholder_counter = 0
        
        # CHTL语法模式（全局style块中允许的）
        self.chtl_patterns = {
            # 属性运算 - 更精确的模式
            'property_operation': r'[a-zA-Z-]+\s*:\s*[^;{}]*[\+\-\*\/\%\*\*][^;{}]*;',
            # 属性条件表达式
            'property_condition': r'[a-zA-Z-]+\s*:\s*[^;{}]*\?[^;{}]*:[^;{}]*;',
            # 模板变量
            'template_variable': r'[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]+\)',
            # 自定义变量
            'custom_variable': r'@Var\s+[a-zA-Z_][a-zA-Z0-9_]*',
            # 模板样式组
            'template_style': r'@Style\s+[a-zA-Z_][a-zA-Z0-9_]*',
            # 自定义样式组
            'custom_style': r'\[Custom\]\s*@Style\s+[a-zA-Z_][a-zA-Z0-9_]*',
            # 生成器注释
            'generator_comment': r'#\s+[^\n]+',
            # 全缀名
            'full_qualified_name': r'[a-zA-Z_][a-zA-Z0-9_]*\.[a-zA-Z_][a-zA-Z0-9_]*',
            # 原始嵌入
            'raw_embed': r'`[^`]+`',
            # from语法
            'from_syntax': r'from\s+[a-zA-Z_][a-zA-Z0-9_]*',
        }
        
        # CHTL JS语法模式
        self.chtl_js_patterns = {
            # 增强选择器
            'enhanced_selector': r'\{\{[^}]+\}\}',
            # CHTL JS函数 - 更精确的模式
            'chtl_js_function': r'[a-zA-Z_][a-zA-Z0-9_]*\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}',
            # 虚对象
            'virtual_object': r'Vir\s+[a-zA-Z_][a-zA-Z0-9_]*\s*=',
            # 事件监听器
            'event_listener': r'Listen\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}',
            # 动画
            'animation': r'Animate\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}',
            # 脚本加载器
            'script_loader': r'ScriptLoader\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}',
            # 事件委托
            'event_delegate': r'Delegate\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}',
            # 响应式值
            'reactive_value': r'\$[a-zA-Z_][a-zA-Z0-9_]*\$',
            # 箭头函数中的CHTL JS
            'arrow_chtl_js': r'[a-zA-Z_][a-zA-Z0-9_]*\s*->\s*[a-zA-Z_][a-zA-Z0-9_]*',
        }
        
        # JS语法边界模式
        self.js_boundary_patterns = {
            'function_declaration': r'function\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]*\)\s*\{',
            'function_expression': r'[a-zA-Z_][a-zA-Z0-9_]*\s*=\s*function\s*\([^)]*\)\s*\{',
            'arrow_function': r'[a-zA-Z_][a-zA-Z0-9_]*\s*=>\s*\{',
            'class_declaration': r'class\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\{',
            'if_statement': r'if\s*\([^)]+\)\s*\{',
            'for_loop': r'for\s*\([^)]+\)\s*\{',
            'while_loop': r'while\s*\([^)]+\)\s*\{',
            'try_catch': r'try\s*\{',
            'object_literal': r'\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}',
            # 更通用的JS代码块
            'js_code_block': r'[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]*\)\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}',
        }
        
        # CSS语法模式
        self.css_patterns = {
            'selector': r'[^{]+\{',
            'property': r'[a-zA-Z-]+:\s*[^;]+;',
            'comment': r'/\*[^*]*\*+(?:[^/*][^*]*\*+)*/',
        }

    def generate_placeholder_id(self, token_type: TokenType) -> str:
        """生成占位符ID"""
        self.placeholder_counter += 1
        if token_type == TokenType.CSS_SYNTAX:
            return f"CSS_CODE_PLACEHOLDER_{self.placeholder_counter}"
        elif token_type == TokenType.JS_SYNTAX:
            return f"JS_CODE_PLACEHOLDER_{self.placeholder_counter}"
        elif token_type == TokenType.CHTL_SYNTAX:
            return f"CHTL_CODE_PLACEHOLDER_{self.placeholder_counter}"
        elif token_type == TokenType.CHTL_JS_SYNTAX:
            return f"CHTL_JS_CODE_PLACEHOLDER_{self.placeholder_counter}"
        else:
            return f"PLACEHOLDER_{self.placeholder_counter}"

    def create_placeholder(self, content: str, token_type: TokenType, start_pos: int, end_pos: int) -> str:
        """创建占位符"""
        placeholder_id = self.generate_placeholder_id(token_type)
        self.placeholders[placeholder_id] = PlaceholderInfo(
            id=placeholder_id,
            original_content=content,
            token_type=token_type,
            start_pos=start_pos,
            end_pos=end_pos
        )
        return placeholder_id

    def detect_chtl_syntax(self, content: str, start_pos: int = 0) -> List[Token]:
        """检测CHTL语法"""
        tokens = []
        
        # 简化的CHTL语法检测
        patterns = [
            # 属性运算
            (r'[a-zA-Z-]+\s*:\s*[^;{}]*[\+\-\*\/\%\*\*][^;{}]*;', 'property_operation'),
            # 模板变量
            (r'[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]+\)', 'template_variable'),
            # 模板样式组
            (r'@Style\s+[a-zA-Z_][a-zA-Z0-9_]*', 'template_style'),
            # 自定义样式组
            (r'\[Custom\]\s*@Style\s+[a-zA-Z_][a-zA-Z0-9_]*', 'custom_style'),
            # 自定义变量
            (r'@Var\s+[a-zA-Z_][a-zA-Z0-9_]*', 'custom_variable'),
        ]
        
        for pattern, name in patterns:
            for match in re.finditer(pattern, content, re.MULTILINE | re.DOTALL):
                token = Token(
                    type=TokenType.CHTL_SYNTAX,
                    content=match.group(0),
                    start_pos=match.start() + start_pos,
                    end_pos=match.end() + start_pos
                )
                tokens.append(token)
        
        return tokens

    def detect_chtl_js_syntax(self, content: str, start_pos: int = 0) -> List[Token]:
        """检测CHTL JS语法"""
        tokens = []
        
        # 简化的CHTL JS语法检测
        patterns = [
            # 增强选择器
            (r'\{\{[^}]+\}\}', 'enhanced_selector'),
            # 响应式值
            (r'\$[a-zA-Z_][a-zA-Z0-9_]*\$', 'reactive_value'),
            # 箭头函数中的CHTL JS
            (r'[a-zA-Z_][a-zA-Z0-9_]*\s*->\s*[a-zA-Z_][a-zA-Z0-9_]*', 'arrow_chtl_js'),
            # CHTL JS函数 - 改进的模式，支持更复杂的嵌套
            (r'Listen\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}', 'event_listener'),
            (r'Animate\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}', 'animation'),
            (r'ScriptLoader\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}', 'script_loader'),
            (r'Delegate\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}', 'event_delegate'),
            # 更通用的CHTL JS函数模式
            (r'[A-Z][a-zA-Z0-9_]*\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}', 'chtl_js_function'),
        ]
        
        for pattern, name in patterns:
            for match in re.finditer(pattern, content, re.MULTILINE | re.DOTALL):
                token = Token(
                    type=TokenType.CHTL_JS_SYNTAX,
                    content=match.group(0),
                    start_pos=match.start() + start_pos,
                    end_pos=match.end() + start_pos
                )
                tokens.append(token)
        
        return tokens

    def detect_js_boundaries(self, content: str, start_pos: int = 0) -> List[Token]:
        """检测JS语法边界"""
        tokens = []
        
        # 简化的JS语法检测
        patterns = [
            # 函数声明
            (r'function\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]*\)\s*\{', 'function_declaration'),
            # 箭头函数 - 改进模式
            (r'\([^)]*\)\s*=>\s*\{', 'arrow_function'),
            (r'[a-zA-Z_][a-zA-Z0-9_]*\s*=>\s*\{', 'arrow_function_var'),
            # 类声明
            (r'class\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\{', 'class_declaration'),
            # 控制结构
            (r'if\s*\([^)]+\)\s*\{', 'if_statement'),
            (r'for\s*\([^)]+\)\s*\{', 'for_loop'),
            (r'while\s*\([^)]+\)\s*\{', 'while_loop'),
            (r'try\s*\{', 'try_catch'),
        ]
        
        for pattern, name in patterns:
            for match in re.finditer(pattern, content, re.MULTILINE | re.DOTALL):
                token = Token(
                    type=TokenType.JS_SYNTAX,
                    content=match.group(0),
                    start_pos=match.start() + start_pos,
                    end_pos=match.end() + start_pos
                )
                tokens.append(token)
        
        return tokens

    def detect_css_syntax(self, content: str, start_pos: int = 0) -> List[Token]:
        """检测CSS语法"""
        tokens = []
        
        for pattern_name, pattern in self.css_patterns.items():
            for match in re.finditer(pattern, content, re.MULTILINE | re.DOTALL):
                token = Token(
                    type=TokenType.CSS_SYNTAX,
                    content=match.group(0),
                    start_pos=match.start() + start_pos,
                    end_pos=match.end() + start_pos
                )
                tokens.append(token)
        
        return tokens

    def find_style_blocks(self, content: str) -> List[Tuple[int, int, str]]:
        """查找所有style块"""
        style_blocks = []
        
        # 查找全局style块（HTML标签）
        global_style_pattern = r'<style[^>]*>(.*?)</style>'
        for match in re.finditer(global_style_pattern, content, re.MULTILINE | re.DOTALL):
            style_blocks.append((
                match.start(1),
                match.end(1),
                'global'
            ))
        
        # 查找局部style块（CHTL语法）
        local_style_pattern = r'style\s*\{([^{}]*(?:\{[^{}]*\}[^{}]*)*)\}'
        for match in re.finditer(local_style_pattern, content, re.MULTILINE | re.DOTALL):
            style_blocks.append((
                match.start(1),
                match.end(1),
                'local'
            ))
        
        return style_blocks

    def find_script_blocks(self, content: str) -> List[Tuple[int, int, str]]:
        """查找所有script块"""
        script_blocks = []
        
        # 查找全局script块（HTML标签）
        global_script_pattern = r'<script[^>]*>(.*?)</script>'
        for match in re.finditer(global_script_pattern, content, re.MULTILINE | re.DOTALL):
            script_blocks.append((
                match.start(1),
                match.end(1),
                'global'
            ))
        
        # 查找局部script块（CHTL语法）
        # 使用更简单的方法：先找到script {，然后找到匹配的}
        script_start_pattern = r'script\s*\{'
        for match in re.finditer(script_start_pattern, content, re.MULTILINE | re.DOTALL):
            start_pos = match.end()
            # 从开始位置开始，计算大括号的匹配
            brace_count = 0
            end_pos = start_pos
            for i, char in enumerate(content[start_pos:], start_pos):
                if char == '{':
                    brace_count += 1
                elif char == '}':
                    if brace_count == 0:
                        end_pos = i
                        break
                    brace_count -= 1
            
            if end_pos > start_pos:
                script_blocks.append((
                    start_pos,
                    end_pos,
                    'local'
                ))
        
        return script_blocks

    def process_global_style_block(self, content: str, start_pos: int, end_pos: int) -> str:
        """处理全局style块中的CHTL语法"""
        block_content = content[start_pos:end_pos]
        result = block_content
        
        # 检测CHTL语法
        chtl_tokens = self.detect_chtl_syntax(block_content, start_pos)
        
        # 按位置倒序排序，避免替换时位置偏移
        chtl_tokens.sort(key=lambda x: x.start_pos, reverse=True)
        
        for token in chtl_tokens:
            # 全局style块中的CHTL语法使用CSS_CODE_PLACEHOLDER_
            placeholder = self.create_placeholder(
                token.content,
                TokenType.CSS_SYNTAX,  # 使用CSS_SYNTAX类型
                token.start_pos,
                token.end_pos
            )
            
            # 计算在块内的相对位置
            relative_start = token.start_pos - start_pos
            relative_end = token.end_pos - start_pos
            
            result = result[:relative_start] + placeholder + result[relative_end:]
        
        return result

    def process_script_block(self, content: str, start_pos: int, end_pos: int, block_type: str) -> str:
        """处理script块中的混合语法"""
        block_content = content[start_pos:end_pos]
        result = block_content
        
        if block_type == 'local':
            # 局部script允许CHTL、CHTL JS、JS三种语法
            # 先检测CHTL语法
            chtl_tokens = self.detect_chtl_syntax(block_content, start_pos)
            
            # 检测CHTL JS语法
            chtl_js_tokens = self.detect_chtl_js_syntax(block_content, start_pos)
            
            # 检测JS语法边界
            js_tokens = self.detect_js_boundaries(block_content, start_pos)
            
            # 合并所有标记并按位置排序
            all_tokens = chtl_tokens + chtl_js_tokens + js_tokens
            all_tokens.sort(key=lambda x: x.start_pos, reverse=True)
            
            for token in all_tokens:
                placeholder = self.create_placeholder(
                    token.content,
                    token.type,
                    token.start_pos,
                    token.end_pos
                )
                
                # 计算在块内的相对位置
                relative_start = token.start_pos - start_pos
                relative_end = token.end_pos - start_pos
                
                result = result[:relative_start] + placeholder + result[relative_end:]
        
        else:
            # 全局script只允许CHTL JS和JS
            # 检测CHTL JS语法
            chtl_js_tokens = self.detect_chtl_js_syntax(block_content, start_pos)
            
            # 检测JS语法边界
            js_tokens = self.detect_js_boundaries(block_content, start_pos)
            
            # 合并标记并按位置排序
            all_tokens = chtl_js_tokens + js_tokens
            all_tokens.sort(key=lambda x: x.start_pos, reverse=True)
            
            for token in all_tokens:
                placeholder = self.create_placeholder(
                    token.content,
                    token.type,
                    token.start_pos,
                    token.end_pos
                )
                
                # 计算在块内的相对位置
                relative_start = token.start_pos - start_pos
                relative_end = token.end_pos - start_pos
                
                result = result[:relative_start] + placeholder + result[relative_end:]
        
        return result

    def separate_chtl_js_and_js(self, content: str) -> str:
        """分离CHTL JS和JS代码"""
        result = content
        
        # 检测CHTL JS语法
        chtl_js_tokens = self.detect_chtl_js_syntax(content)
        
        # 检测JS语法边界
        js_tokens = self.detect_js_boundaries(content)
        
        # 合并标记并按位置排序
        all_tokens = chtl_js_tokens + js_tokens
        all_tokens.sort(key=lambda x: x.start_pos, reverse=True)
        
        for token in all_tokens:
            placeholder = self.create_placeholder(
                token.content,
                token.type,
                token.start_pos,
                token.end_pos
            )
            
            result = result[:token.start_pos] + placeholder + result[token.end_pos:]
        
        return result

    def scan(self, content: str) -> Dict[str, Union[str, Dict[str, PlaceholderInfo]]]:
        """主扫描方法"""
        result = content
        self.placeholders.clear()
        self.placeholder_counter = 0
        
        # 处理style块
        style_blocks = self.find_style_blocks(content)
        for start_pos, end_pos, block_type in style_blocks:
            if block_type == 'global':
                processed_block = self.process_global_style_block(content, start_pos, end_pos)
                result = result[:start_pos] + processed_block + result[end_pos:]
            elif block_type == 'local':
                # 局部style块中的CHTL语法也使用CSS_CODE_PLACEHOLDER_
                processed_block = self.process_global_style_block(content, start_pos, end_pos)
                result = result[:start_pos] + processed_block + result[end_pos:]
        
        # 处理script块
        script_blocks = self.find_script_blocks(content)
        for start_pos, end_pos, block_type in script_blocks:
            processed_block = self.process_script_block(content, start_pos, end_pos, block_type)
            result = result[:start_pos] + processed_block + result[end_pos:]
        
        # 处理独立的JS代码（不在script标签内）
        # 检测纯JS代码模式
        js_patterns = [
            r'function\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]*\)\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}',
            r'const\s+[a-zA-Z_][a-zA-Z0-9_]*\s*=\s*\([^)]*\)\s*=>\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}',
            r'class\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}',
        ]
        
        for pattern in js_patterns:
            for match in re.finditer(pattern, result, re.MULTILINE | re.DOTALL):
                placeholder = self.create_placeholder(
                    match.group(0),
                    TokenType.JS_SYNTAX,
                    match.start(),
                    match.end()
                )
                result = result[:match.start()] + placeholder + result[match.end():]
        
        return {
            'processed_content': result,
            'placeholders': self.placeholders
        }

    def restore_placeholders(self, content: str) -> str:
        """恢复占位符为原始内容"""
        result = content
        
        for placeholder_id, placeholder_info in self.placeholders.items():
            result = result.replace(placeholder_id, placeholder_info.original_content)
        
        return result

    def get_placeholder_info(self, placeholder_id: str) -> Optional[PlaceholderInfo]:
        """获取占位符信息"""
        return self.placeholders.get(placeholder_id)

    def export_placeholders(self, filepath: str):
        """导出占位符信息到文件"""
        export_data = {}
        for placeholder_id, info in self.placeholders.items():
            export_data[placeholder_id] = {
                'original_content': info.original_content,
                'token_type': info.token_type.value,
                'start_pos': info.start_pos,
                'end_pos': info.end_pos
            }
        
        with open(filepath, 'w', encoding='utf-8') as f:
            json.dump(export_data, f, ensure_ascii=False, indent=2)

    def import_placeholders(self, filepath: str):
        """从文件导入占位符信息"""
        with open(filepath, 'r', encoding='utf-8') as f:
            import_data = json.load(f)
        
        self.placeholders.clear()
        for placeholder_id, info_data in import_data.items():
            self.placeholders[placeholder_id] = PlaceholderInfo(
                id=placeholder_id,
                original_content=info_data['original_content'],
                token_type=TokenType(info_data['token_type']),
                start_pos=info_data['start_pos'],
                end_pos=info_data['end_pos']
            )

def main():
    """主函数，用于测试"""
    scanner = UnifiedScanner()
    
    # 测试用例
    test_content = """
    <style>
        body {
            color: white;
            background-color: rgb(255, 192, 203);
        }
        
        .box {
            width: 100px;
            height: 100px / 2;  /* CHTL语法：属性运算 */
            @Style BoxStyle;    /* CHTL语法：模板样式组 */
        }
    </style>
    
    <script>
        let v = "Test";
        {{v}} -> Listen {       /* CHTL JS语法 */
            click: () => { console.log({{v}}) }  /* JS语法 */
        }
    </script>
    """
    
    print("原始内容:")
    print(test_content)
    print("\n" + "="*50 + "\n")
    
    # 执行扫描
    result = scanner.scan(test_content)
    
    print("扫描结果:")
    print(result['processed_content'])
    print("\n" + "="*50 + "\n")
    
    print("占位符信息:")
    for placeholder_id, info in result['placeholders'].items():
        print(f"{placeholder_id}: {info.original_content} ({info.token_type.value})")
    
    print("\n" + "="*50 + "\n")
    
    # 恢复占位符
    restored = scanner.restore_placeholders(result['processed_content'])
    print("恢复后的内容:")
    print(restored)

if __name__ == "__main__":
    main()