#!/usr/bin/env python3
"""
编译器调度器
协调统一扫描器、CHTL编译器、CHTL JS编译器和代码合并器的执行

基于CHTL.md规范实现完整的编译器调度系统
"""

import os
import time
from typing import Dict, List, Tuple, Optional, Any
from dataclasses import dataclass

# 导入各个编译器组件
from unified_scanner import UnifiedScanner, PlaceholderInfo
from code_merger import CodeMerger
from chtl_compiler import CHTLLexer, CHTLParser, CHTLGenerator, CHTLContext
from chtl_js_compiler import CHTLJSLexer, CHTLJSTokenType

@dataclass
class CompilationResult:
    """编译结果"""
    success: bool
    html_content: str
    css_content: str
    js_content: str
    error_message: Optional[str] = None
    compilation_time: float = 0.0
    placeholder_count: int = 0

class CompilerDispatcher:
    """编译器调度器"""
    
    def __init__(self):
        self.scanner = UnifiedScanner()
        self.code_merger = CodeMerger()
        self.chtl_lexer = CHTLLexer()
        self.chtl_js_lexer = CHTLJSLexer()
        
        # 编译统计
        self.stats = {
            'total_compilations': 0,
            'successful_compilations': 0,
            'failed_compilations': 0,
            'total_time': 0.0,
            'average_time': 0.0
        }
    
    def compile(self, chtl_content: str, output_dir: str = "output") -> CompilationResult:
        """编译CHTL内容"""
        start_time = time.time()
        
        try:
            # 1. 统一扫描 - 分离不同语言的代码片段
            print("🔍 开始统一扫描...")
            scan_result = self.scanner.scan(chtl_content)
            scanned_content = scan_result['processed_content']
            placeholders = scan_result['placeholders']
            print(f"✅ 扫描完成，检测到 {len(placeholders)} 个占位符")
            
            # 2. 处理CHTL代码片段
            print("🔧 处理CHTL代码片段...")
            chtl_segments = self._extract_chtl_segments(placeholders)
            processed_chtl = self._process_chtl_segments(chtl_segments)
            
            # 3. 处理CHTL JS代码片段
            print("🔧 处理CHTL JS代码片段...")
            chtl_js_segments = self._extract_chtl_js_segments(placeholders)
            processed_chtl_js = self._process_chtl_js_segments(chtl_js_segments)
            
            # 4. 处理CSS代码片段
            print("🎨 处理CSS代码片段...")
            css_segments = self._extract_css_segments(placeholders)
            processed_css = self._process_css_segments(css_segments)
            
            # 5. 处理JS代码片段
            print("⚡ 处理JS代码片段...")
            js_segments = self._extract_js_segments(placeholders)
            processed_js = self._process_js_segments(js_segments)
            
            # 6. 代码合并
            print("🔗 合并代码片段...")
            merged_content = self._merge_code_segments(
                scanned_content, placeholders,
                processed_chtl, processed_chtl_js,
                processed_css, processed_js
            )
            
            # 7. 生成最终文件
            print("📁 生成输出文件...")
            html_content, css_content, js_content = self._generate_final_files(merged_content)
            
            # 8. 保存文件
            if output_dir:
                self._save_files(output_dir, html_content, css_content, js_content)
            
            compilation_time = time.time() - start_time
            self._update_stats(True, compilation_time)
            
            print(f"✅ 编译完成！耗时: {compilation_time:.4f}秒")
            
            return CompilationResult(
                success=True,
                html_content=html_content,
                css_content=css_content,
                js_content=js_content,
                compilation_time=compilation_time,
                placeholder_count=len(placeholders)
            )
            
        except Exception as e:
            compilation_time = time.time() - start_time
            self._update_stats(False, compilation_time)
            
            error_msg = f"编译失败: {str(e)}"
            print(f"❌ {error_msg}")
            
            return CompilationResult(
                success=False,
                html_content="",
                css_content="",
                js_content="",
                error_message=error_msg,
                compilation_time=compilation_time
            )
    
    def _extract_chtl_segments(self, placeholders: Dict[str, PlaceholderInfo]) -> List[str]:
        """提取CHTL代码片段"""
        chtl_segments = []
        for placeholder_id, info in placeholders.items():
            if info.token_type.value == "CHTL_SYNTAX":
                chtl_segments.append(info.original_content)
        return chtl_segments
    
    def _extract_chtl_js_segments(self, placeholders: Dict[str, PlaceholderInfo]) -> List[str]:
        """提取CHTL JS代码片段"""
        chtl_js_segments = []
        for placeholder_id, info in placeholders.items():
            if info.token_type.value == "CHTL_JS_SYNTAX":
                chtl_js_segments.append(info.original_content)
        return chtl_js_segments
    
    def _extract_css_segments(self, placeholders: Dict[str, PlaceholderInfo]) -> List[str]:
        """提取CSS代码片段"""
        css_segments = []
        for placeholder_id, info in placeholders.items():
            if info.token_type.value == "CSS_SYNTAX":
                css_segments.append(info.original_content)
        return css_segments
    
    def _extract_js_segments(self, placeholders: Dict[str, PlaceholderInfo]) -> List[str]:
        """提取JS代码片段"""
        js_segments = []
        for placeholder_id, info in placeholders.items():
            if info.token_type.value == "JS_SYNTAX":
                js_segments.append(info.original_content)
        return js_segments
    
    def _process_chtl_segments(self, chtl_segments: List[str]) -> List[str]:
        """处理CHTL代码片段"""
        processed_segments = []
        
        for segment in chtl_segments:
            try:
                # 词法分析
                tokens = self.chtl_lexer.tokenize(segment)
                
                # 语法分析
                parser = CHTLParser(tokens)
                ast = parser.parse()
                
                # 代码生成
                generator = CHTLGenerator(parser.context)
                html = generator.generate(ast)
                
                processed_segments.append(html)
            except Exception as e:
                print(f"⚠️ CHTL片段处理失败: {e}")
                processed_segments.append(segment)  # 保持原样
        
        return processed_segments
    
    def _process_chtl_js_segments(self, chtl_js_segments: List[str]) -> List[str]:
        """处理CHTL JS代码片段"""
        processed_segments = []
        
        for segment in chtl_js_segments:
            try:
                # 词法分析
                tokens = self.chtl_js_lexer.tokenize(segment)
                
                # 简单的CHTL JS到JS转换
                js_code = self._convert_chtl_js_to_js(segment, tokens)
                
                processed_segments.append(js_code)
            except Exception as e:
                print(f"⚠️ CHTL JS片段处理失败: {e}")
                processed_segments.append(segment)  # 保持原样
        
        return processed_segments
    
    def _process_css_segments(self, css_segments: List[str]) -> List[str]:
        """处理CSS代码片段"""
        # CSS片段直接返回，可以在这里添加CSS处理逻辑
        return css_segments
    
    def _process_js_segments(self, js_segments: List[str]) -> List[str]:
        """处理JS代码片段"""
        # JS片段直接返回，可以在这里添加JS处理逻辑
        return js_segments
    
    def _convert_chtl_js_to_js(self, content: str, tokens: List) -> str:
        """将CHTL JS转换为JavaScript"""
        import re
        js_code = content
        
        # 处理增强选择器 {{...}}
        enhanced_selector_pattern = r'\{\{([^}]+)\}\}'
        
        def replace_enhanced_selector(match):
            selector = match.group(1)
            # 简单的选择器转换
            if selector.startswith('.'):
                return f'document.querySelector("{selector}")'
            elif selector.startswith('#'):
                return f'document.getElementById("{selector[1:]}")'
            else:
                return f'document.querySelector("{selector}")'
        
        js_code = re.sub(enhanced_selector_pattern, replace_enhanced_selector, js_code)
        
        # 处理响应式值 $...$
        responsive_value_pattern = r'\$([^$]+)\$'
        js_code = re.sub(responsive_value_pattern, r'\1', js_code)
        
        # 处理CHTL JS函数
        js_code = self._convert_chtl_js_functions(js_code)
        
        return js_code
    
    def _convert_chtl_js_functions(self, content: str) -> str:
        """转换CHTL JS函数为JavaScript"""
        js_code = content
        
        # 处理ScriptLoader
        scriptloader_pattern = r'ScriptLoader\s*\{([^}]+)\}'
        def replace_scriptloader(match):
            load_content = match.group(1)
            # 简单的脚本加载器实现
            return f'// ScriptLoader: {load_content}'
        
        js_code = re.sub(scriptloader_pattern, replace_scriptloader, js_code)
        
        # 处理Listen
        listen_pattern = r'(\{\{[^}]+\}\})->Listen\s*\{([^}]+)\}'
        def replace_listen(match):
            selector = match.group(1)
            events = match.group(2)
            # 简单的事件监听器实现
            return f'{selector}.addEventListener("click", function() {{ {events} }});'
        
        js_code = re.sub(listen_pattern, replace_listen, js_code)
        
        return js_code
    
    def _merge_code_segments(self, scanned_content: str, placeholders: Dict[str, PlaceholderInfo],
                           processed_chtl: List[str], processed_chtl_js: List[str],
                           processed_css: List[str], processed_js: List[str]) -> str:
        """合并代码片段"""
        merged_content = scanned_content
        
        # 创建占位符到处理结果的映射
        placeholder_map = {}
        
        chtl_index = 0
        chtl_js_index = 0
        css_index = 0
        js_index = 0
        
        for placeholder_id, info in placeholders.items():
            if info.token_type.value == "CHTL_SYNTAX" and chtl_index < len(processed_chtl):
                placeholder_map[placeholder_id] = processed_chtl[chtl_index]
                chtl_index += 1
            elif info.token_type.value == "CHTL_JS_SYNTAX" and chtl_js_index < len(processed_chtl_js):
                placeholder_map[placeholder_id] = processed_chtl_js[chtl_js_index]
                chtl_js_index += 1
            elif info.token_type.value == "CSS_SYNTAX" and css_index < len(processed_css):
                placeholder_map[placeholder_id] = processed_css[css_index]
                css_index += 1
            elif info.token_type.value == "JS_SYNTAX" and js_index < len(processed_js):
                placeholder_map[placeholder_id] = processed_js[js_index]
                js_index += 1
        
        # 替换占位符
        for placeholder_id, replacement in placeholder_map.items():
            merged_content = merged_content.replace(placeholder_id, replacement)
        
        return merged_content
    
    def _generate_final_files(self, merged_content: str) -> Tuple[str, str, str]:
        """生成最终文件"""
        # 简单的文件分离逻辑
        html_content = merged_content
        css_content = ""
        js_content = ""
        
        # 提取CSS内容
        import re
        css_pattern = r'<style[^>]*>(.*?)</style>'
        css_matches = re.findall(css_pattern, merged_content, re.DOTALL)
        css_content = '\n'.join(css_matches)
        
        # 提取JS内容
        js_pattern = r'<script[^>]*>(.*?)</script>'
        js_matches = re.findall(js_pattern, merged_content, re.DOTALL)
        js_content = '\n'.join(js_matches)
        
        # 清理HTML中的style和script标签
        html_content = re.sub(css_pattern, '', html_content, flags=re.DOTALL)
        html_content = re.sub(js_pattern, '', html_content, flags=re.DOTALL)
        
        return html_content, css_content, js_content
    
    def _save_files(self, output_dir: str, html_content: str, css_content: str, js_content: str):
        """保存文件"""
        os.makedirs(output_dir, exist_ok=True)
        
        # 保存HTML文件
        with open(os.path.join(output_dir, "index.html"), "w", encoding="utf-8") as f:
            f.write(html_content)
        
        # 保存CSS文件
        if css_content.strip():
            with open(os.path.join(output_dir, "styles.css"), "w", encoding="utf-8") as f:
                f.write(css_content)
        
        # 保存JS文件
        if js_content.strip():
            with open(os.path.join(output_dir, "script.js"), "w", encoding="utf-8") as f:
                f.write(js_content)
        
        print(f"📁 文件已保存到 {output_dir} 目录")
    
    def _update_stats(self, success: bool, compilation_time: float):
        """更新编译统计"""
        self.stats['total_compilations'] += 1
        if success:
            self.stats['successful_compilations'] += 1
        else:
            self.stats['failed_compilations'] += 1
        
        self.stats['total_time'] += compilation_time
        self.stats['average_time'] = self.stats['total_time'] / self.stats['total_compilations']
    
    def get_stats(self) -> Dict[str, Any]:
        """获取编译统计"""
        return self.stats.copy()

def main():
    """测试编译器调度器"""
    dispatcher = CompilerDispatcher()
    
    # 测试CHTL内容
    test_chtl = """
    use html5;
    
    [Template] @Style BoxStyle {
        color: white;
        background-color: rgb(255, 192, 203);
        padding: 20px;
        border-radius: 8px;
    }
    
    html {
        head {
            title {
                text: "CHTL编译器测试";
            }
        }
        body {
            div {
                class: box;
                style {
                    @Style BoxStyle;
                }
                script {
                    {{box}}->Listen {
                        click: () => {
                            console.log('Box clicked!');
                        }
                    }
                }
            }
        }
    }
    """
    
    print("🚀 开始CHTL编译测试")
    print("=" * 50)
    
    result = dispatcher.compile(test_chtl, "test_output")
    
    if result.success:
        print("\n✅ 编译成功！")
        print(f"📊 编译统计:")
        print(f"  - 编译时间: {result.compilation_time:.4f}秒")
        print(f"  - 占位符数量: {result.placeholder_count}")
        print(f"  - HTML长度: {len(result.html_content)} 字符")
        print(f"  - CSS长度: {len(result.css_content)} 字符")
        print(f"  - JS长度: {len(result.js_content)} 字符")
        
        print(f"\n📄 生成的HTML:")
        print(result.html_content)
        
        print(f"\n🎨 生成的CSS:")
        print(result.css_content)
        
        print(f"\n⚡ 生成的JS:")
        print(result.js_content)
    else:
        print(f"\n❌ 编译失败: {result.error_message}")
    
    print(f"\n📈 总体统计:")
    stats = dispatcher.get_stats()
    for key, value in stats.items():
        print(f"  - {key}: {value}")

if __name__ == "__main__":
    main()