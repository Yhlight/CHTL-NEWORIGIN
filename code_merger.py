#!/usr/bin/env python3
"""
CHTL代码合并器
用于将统一扫描器分离后的代码片段重新组合

功能：
1. 将占位符替换为处理后的代码
2. 合并不同语言的代码片段
3. 生成最终的HTML、CSS、JS文件
"""

import os
import json
from typing import Dict, List, Tuple, Optional, Union
from dataclasses import dataclass
from enum import Enum
from unified_scanner import UnifiedScanner, TokenType, PlaceholderInfo

class OutputFormat(Enum):
    """输出格式枚举"""
    HTML = "html"
    CSS = "css"
    JS = "js"
    INLINE = "inline"

@dataclass
class CodeSegment:
    """代码片段数据结构"""
    content: str
    language: str
    start_pos: int
    end_pos: int
    placeholder_id: Optional[str] = None

@dataclass
class CompilationResult:
    """编译结果数据结构"""
    html_content: str
    css_content: str
    js_content: str
    inline_content: str
    placeholders: Dict[str, PlaceholderInfo]
    segments: List[CodeSegment]

class CodeMerger:
    """代码合并器主类"""
    
    def __init__(self, scanner: Optional[UnifiedScanner] = None):
        self.scanner = scanner or UnifiedScanner()
        self.segments: List[CodeSegment] = []
    
    def merge_placeholders(self, processed_content: str, placeholders: Dict[str, PlaceholderInfo], 
                          processed_placeholders: Dict[str, str]) -> str:
        """合并占位符为处理后的代码"""
        result = processed_content
        
        # 按位置倒序排序，避免替换时位置偏移
        sorted_placeholders = sorted(placeholders.items(), 
                                   key=lambda x: x[1].start_pos, reverse=True)
        
        for placeholder_id, placeholder_info in sorted_placeholders:
            if placeholder_id in processed_placeholders:
                # 使用处理后的代码替换占位符
                processed_code = processed_placeholders[placeholder_id]
                result = result.replace(placeholder_id, processed_code)
            else:
                # 如果没有处理后的代码，使用原始内容
                result = result.replace(placeholder_id, placeholder_info.original_content)
        
        return result
    
    def extract_code_segments(self, content: str, placeholders: Dict[str, PlaceholderInfo]) -> List[CodeSegment]:
        """提取代码片段"""
        segments = []
        
        for placeholder_id, placeholder_info in placeholders.items():
            segment = CodeSegment(
                content=placeholder_info.original_content,
                language=self._get_language_from_token_type(placeholder_info.token_type),
                start_pos=placeholder_info.start_pos,
                end_pos=placeholder_info.end_pos,
                placeholder_id=placeholder_id
            )
            segments.append(segment)
        
        return segments
    
    def _get_language_from_token_type(self, token_type: TokenType) -> str:
        """根据标记类型获取语言名称"""
        if token_type == TokenType.CSS_SYNTAX:
            return "css"
        elif token_type == TokenType.JS_SYNTAX:
            return "javascript"
        elif token_type == TokenType.CHTL_SYNTAX:
            return "chtl"
        elif token_type == TokenType.CHTL_JS_SYNTAX:
            return "chtl_js"
        else:
            return "unknown"
    
    def group_segments_by_language(self, segments: List[CodeSegment]) -> Dict[str, List[CodeSegment]]:
        """按语言分组代码片段"""
        grouped = {}
        for segment in segments:
            if segment.language not in grouped:
                grouped[segment.language] = []
            grouped[segment.language].append(segment)
        return grouped
    
    def generate_css_content(self, css_segments: List[CodeSegment]) -> str:
        """生成CSS内容"""
        css_content = []
        
        for segment in css_segments:
            css_content.append(segment.content)
        
        return "\n\n".join(css_content)
    
    def generate_js_content(self, js_segments: List[CodeSegment]) -> str:
        """生成JavaScript内容"""
        js_content = []
        
        for segment in js_segments:
            js_content.append(segment.content)
        
        return "\n\n".join(js_content)
    
    def generate_html_content(self, html_content: str, css_content: str, js_content: str, 
                            inline: bool = False) -> str:
        """生成HTML内容"""
        if inline:
            # 内联模式：将CSS和JS嵌入到HTML中
            html_with_css = html_content.replace(
                '</head>', 
                f'<style>\n{css_content}\n</style>\n</head>'
            )
            html_with_js = html_with_css.replace(
                '</body>', 
                f'<script>\n{js_content}\n</script>\n</body>'
            )
            return html_with_js
        else:
            # 分离模式：生成独立的HTML文件
            return html_content
    
    def compile(self, content: str, processed_placeholders: Optional[Dict[str, str]] = None) -> CompilationResult:
        """编译CHTL内容"""
        # 使用统一扫描器扫描内容
        scan_result = self.scanner.scan(content)
        processed_content = scan_result['processed_content']
        placeholders = scan_result['placeholders']
        
        # 提取代码片段
        segments = self.extract_code_segments(content, placeholders)
        self.segments = segments
        
        # 按语言分组
        grouped_segments = self.group_segments_by_language(segments)
        
        # 生成各种语言的代码
        css_content = self.generate_css_content(grouped_segments.get('css', []))
        js_content = self.generate_js_content(grouped_segments.get('javascript', []))
        
        # 合并占位符
        if processed_placeholders:
            html_content = self.merge_placeholders(processed_content, placeholders, processed_placeholders)
        else:
            html_content = self.scanner.restore_placeholders(processed_content)
        
        # 生成内联内容
        inline_content = self.generate_html_content(html_content, css_content, js_content, inline=True)
        
        return CompilationResult(
            html_content=html_content,
            css_content=css_content,
            js_content=js_content,
            inline_content=inline_content,
            placeholders=placeholders,
            segments=segments
        )
    
    def save_to_files(self, result: CompilationResult, output_dir: str = "output", 
                     base_name: str = "output") -> Dict[str, str]:
        """保存编译结果到文件"""
        os.makedirs(output_dir, exist_ok=True)
        
        file_paths = {}
        
        # 保存HTML文件
        html_path = os.path.join(output_dir, f"{base_name}.html")
        with open(html_path, 'w', encoding='utf-8') as f:
            f.write(result.html_content)
        file_paths['html'] = html_path
        
        # 保存CSS文件
        if result.css_content.strip():
            css_path = os.path.join(output_dir, f"{base_name}.css")
            with open(css_path, 'w', encoding='utf-8') as f:
                f.write(result.css_content)
            file_paths['css'] = css_path
        
        # 保存JS文件
        if result.js_content.strip():
            js_path = os.path.join(output_dir, f"{base_name}.js")
            with open(js_path, 'w', encoding='utf-8') as f:
                f.write(result.js_content)
            file_paths['js'] = js_path
        
        # 保存内联文件
        inline_path = os.path.join(output_dir, f"{base_name}_inline.html")
        with open(inline_path, 'w', encoding='utf-8') as f:
            f.write(result.inline_content)
        file_paths['inline'] = inline_path
        
        # 保存占位符信息
        placeholders_path = os.path.join(output_dir, f"{base_name}_placeholders.json")
        placeholders_data = {}
        for placeholder_id, info in result.placeholders.items():
            placeholders_data[placeholder_id] = {
                'original_content': info.original_content,
                'token_type': info.token_type.value,
                'start_pos': info.start_pos,
                'end_pos': info.end_pos
            }
        
        with open(placeholders_path, 'w', encoding='utf-8') as f:
            json.dump(placeholders_data, f, ensure_ascii=False, indent=2)
        file_paths['placeholders'] = placeholders_path
        
        return file_paths
    
    def get_compilation_stats(self, result: CompilationResult) -> Dict[str, Union[int, str]]:
        """获取编译统计信息"""
        stats = {
            'total_placeholders': len(result.placeholders),
            'total_segments': len(result.segments),
            'html_size': len(result.html_content),
            'css_size': len(result.css_content),
            'js_size': len(result.js_content),
            'inline_size': len(result.inline_content)
        }
        
        # 按语言统计片段数量
        language_counts = {}
        for segment in result.segments:
            language_counts[segment.language] = language_counts.get(segment.language, 0) + 1
        
        stats['language_counts'] = language_counts
        
        return stats

def main():
    """主函数，用于测试"""
    # 创建扫描器和合并器
    scanner = UnifiedScanner()
    merger = CodeMerger(scanner)
    
    # 测试用例
    test_content = """
    <!DOCTYPE html>
    <html>
    <head>
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
    </head>
    <body>
        <div class="box">
            <script>
                let v = "Test";
                {{v}} -> Listen {       /* CHTL JS语法 */
                    click: () => { console.log({{v}}) }  /* JS语法 */
                }
            </script>
        </div>
    </body>
    </html>
    """
    
    print("原始内容:")
    print(test_content)
    print("\n" + "="*50 + "\n")
    
    # 编译内容
    result = merger.compile(test_content)
    
    print("编译结果:")
    print(f"HTML内容长度: {len(result.html_content)}")
    print(f"CSS内容长度: {len(result.css_content)}")
    print(f"JS内容长度: {len(result.js_content)}")
    print(f"内联内容长度: {len(result.inline_content)}")
    
    print("\n占位符信息:")
    for placeholder_id, info in result.placeholders.items():
        print(f"  {placeholder_id}: {info.original_content[:50]}... ({info.token_type.value})")
    
    print("\n代码片段信息:")
    for segment in result.segments:
        print(f"  {segment.language}: {segment.content[:50]}...")
    
    # 保存到文件
    file_paths = merger.save_to_files(result, "test_output", "test")
    print(f"\n文件已保存到: {file_paths}")
    
    # 获取统计信息
    stats = merger.get_compilation_stats(result)
    print(f"\n编译统计: {stats}")

if __name__ == "__main__":
    main()