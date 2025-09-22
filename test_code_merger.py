#!/usr/bin/env python3
"""
代码合并器测试用例
"""

import unittest
import os
import tempfile
import shutil
import sys
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from code_merger import CodeMerger, CompilationResult, CodeSegment
from unified_scanner import UnifiedScanner, TokenType

class TestCodeMerger(unittest.TestCase):
    """代码合并器测试类"""
    
    def setUp(self):
        """测试前准备"""
        self.scanner = UnifiedScanner()
        self.merger = CodeMerger(self.scanner)
        self.temp_dir = tempfile.mkdtemp()
    
    def tearDown(self):
        """测试后清理"""
        shutil.rmtree(self.temp_dir)
    
    def test_basic_compilation(self):
        """测试基本编译功能"""
        content = """
        <style>
            .box {
                width: 100px;
                height: 100px / 2;  /* CHTL语法 */
                @Style BoxStyle;    /* CHTL语法 */
            }
        </style>
        
        <script>
            {{v}} -> Listen {       /* CHTL JS语法 */
                click: () => { console.log({{v}}) }  /* JS语法 */
            }
        </script>
        """
        
        result = self.merger.compile(content)
        
        # 验证编译结果
        self.assertIsInstance(result, CompilationResult)
        self.assertIsNotNone(result.html_content)
        self.assertIsNotNone(result.css_content)
        self.assertIsNotNone(result.js_content)
        self.assertIsNotNone(result.inline_content)
        self.assertGreater(len(result.placeholders), 0)
        self.assertGreater(len(result.segments), 0)
    
    def test_code_segment_extraction(self):
        """测试代码片段提取"""
        content = """
        <style>
            .box {
                width: 100px / 2;  /* CHTL语法 */
            }
        </style>
        """
        
        result = self.merger.compile(content)
        segments = result.segments
        
        # 验证代码片段
        self.assertGreater(len(segments), 0)
        
        # 验证片段类型
        segment_types = set(segment.language for segment in segments)
        self.assertIn('css', segment_types)
    
    def test_language_grouping(self):
        """测试语言分组"""
        content = """
        <style>
            .box {
                width: 100px / 2;  /* CHTL语法 */
            }
        </style>
        
        <script>
            function test() {
                return "test";
            }
        </script>
        """
        
        result = self.merger.compile(content)
        grouped = self.merger.group_segments_by_language(result.segments)
        
        # 验证分组
        self.assertIn('css', grouped)
        self.assertIn('javascript', grouped)
        self.assertGreater(len(grouped['css']), 0)
        self.assertGreater(len(grouped['javascript']), 0)
    
    def test_css_generation(self):
        """测试CSS生成"""
        css_segments = [
            CodeSegment("width: 100px;", "css", 0, 15),
            CodeSegment("height: 200px;", "css", 16, 32),
        ]
        
        css_content = self.merger.generate_css_content(css_segments)
        
        self.assertIn("width: 100px;", css_content)
        self.assertIn("height: 200px;", css_content)
    
    def test_js_generation(self):
        """测试JavaScript生成"""
        js_segments = [
            CodeSegment("function test() { return 'test'; }", "javascript", 0, 35),
            CodeSegment("const x = 1;", "javascript", 36, 48),
        ]
        
        js_content = self.merger.generate_js_content(js_segments)
        
        self.assertIn("function test()", js_content)
        self.assertIn("const x = 1;", js_content)
    
    def test_html_generation(self):
        """测试HTML生成"""
        html_content = "<html><head></head><body></body></html>"
        css_content = "body { color: red; }"
        js_content = "console.log('test');"
        
        # 测试分离模式
        result = self.merger.generate_html_content(html_content, css_content, js_content, inline=False)
        self.assertEqual(result, html_content)
        
        # 测试内联模式
        inline_result = self.merger.generate_html_content(html_content, css_content, js_content, inline=True)
        self.assertIn("<style>", inline_result)
        self.assertIn("body { color: red; }", inline_result)
        self.assertIn("<script>", inline_result)
        self.assertIn("console.log('test');", inline_result)
    
    def test_placeholder_merging(self):
        """测试占位符合并"""
        processed_content = "Hello CSS_CODE_PLACEHOLDER_1 World"
        placeholders = {
            "CSS_CODE_PLACEHOLDER_1": type('obj', (object,), {
                'original_content': 'beautiful',
                'start_pos': 6,
                'end_pos': 6,
                'token_type': TokenType.CSS_SYNTAX
            })()
        }
        processed_placeholders = {
            "CSS_CODE_PLACEHOLDER_1": "wonderful"
        }
        
        result = self.merger.merge_placeholders(processed_content, placeholders, processed_placeholders)
        
        self.assertEqual(result, "Hello wonderful World")
    
    def test_file_saving(self):
        """测试文件保存"""
        content = """
        <!DOCTYPE html>
        <html>
        <head>
            <style>
                .box { width: 100px / 2; }
            </style>
        </head>
        <body>
            <script>
                function test() { return "test"; }
            </script>
        </body>
        </html>
        """
        
        result = self.merger.compile(content)
        file_paths = self.merger.save_to_files(result, self.temp_dir, "test")
        
        # 验证文件存在
        self.assertIn('html', file_paths)
        self.assertIn('css', file_paths)
        self.assertIn('js', file_paths)
        self.assertIn('inline', file_paths)
        self.assertIn('placeholders', file_paths)
        
        # 验证文件内容
        with open(file_paths['html'], 'r', encoding='utf-8') as f:
            html_content = f.read()
            self.assertIn('<html>', html_content)
        
        with open(file_paths['css'], 'r', encoding='utf-8') as f:
            css_content = f.read()
            self.assertIn('width:', css_content)
        
        with open(file_paths['js'], 'r', encoding='utf-8') as f:
            js_content = f.read()
            self.assertIn('function', js_content)
    
    def test_compilation_stats(self):
        """测试编译统计"""
        content = """
        <style>
            .box { width: 100px / 2; }
        </style>
        
        <script>
            function test() { return "test"; }
        </script>
        """
        
        result = self.merger.compile(content)
        stats = self.merger.get_compilation_stats(result)
        
        # 验证统计信息
        self.assertIn('total_placeholders', stats)
        self.assertIn('total_segments', stats)
        self.assertIn('html_size', stats)
        self.assertIn('css_size', stats)
        self.assertIn('js_size', stats)
        self.assertIn('inline_size', stats)
        self.assertIn('language_counts', stats)
        
        self.assertGreater(stats['total_placeholders'], 0)
        self.assertGreater(stats['total_segments'], 0)
    
    def test_complex_chtl_file(self):
        """测试复杂CHTL文件编译"""
        content = """
        use html5;
        
        [Custom] @Style BoxStyle {
            color: white;
            background-color: rgb(255, 192, 203);
        }
        
        html {
            head {
                style {
                    body {
                        data-theme: Theme(Sum);  /* CHTL语法 */
                    }
                    
                    .box {
                        width: 100px;
                        height: 100px / 2;  /* CHTL语法 */
                        @Style BoxStyle;    /* CHTL语法 */
                    }
                }
            }
            
            body {
                div {
                    id: $test$;  /* CHTL JS语法 */
                    
                    script {
                        let v = "Test";
                        {{v}} -> Listen {  /* CHTL JS语法 */
                            click: () => { console.log({{v}}) }  /* JS语法 */
                        }
                    }
                }
            }
        }
        """
        
        result = self.merger.compile(content)
        
        # 验证编译结果
        self.assertIsInstance(result, CompilationResult)
        self.assertGreater(len(result.placeholders), 0)
        self.assertGreater(len(result.segments), 0)
        
        # 验证不同语言的片段
        languages = set(segment.language for segment in result.segments)
        self.assertIn('css', languages)
        self.assertIn('javascript', languages)
        self.assertIn('chtl_js', languages)
    
    def test_placeholder_restoration(self):
        """测试占位符恢复"""
        content = """
        <style>
            .box { width: 100px / 2; }
        </style>
        """
        
        result = self.merger.compile(content)
        
        # 验证占位符被正确恢复
        self.assertNotIn('CSS_CODE_PLACEHOLDER_', result.html_content)
        self.assertIn('100px / 2', result.html_content)
    
    def test_empty_content(self):
        """测试空内容"""
        content = ""
        
        result = self.merger.compile(content)
        
        # 验证空内容处理
        self.assertEqual(result.html_content, "")
        self.assertEqual(result.css_content, "")
        self.assertEqual(result.js_content, "")
        self.assertEqual(len(result.placeholders), 0)
        self.assertEqual(len(result.segments), 0)

def run_performance_test():
    """性能测试"""
    import time
    
    scanner = UnifiedScanner()
    merger = CodeMerger(scanner)
    
    # 创建大型测试文件
    large_content = """
    <style>
        body { color: white; background-color: rgb(255, 192, 203); }
    </style>
    """ * 100  # 重复100次
    
    start_time = time.time()
    result = merger.compile(large_content)
    end_time = time.time()
    
    print(f"编译大型文件耗时: {end_time - start_time:.4f}秒")
    print(f"占位符数量: {len(result.placeholders)}")
    print(f"代码片段数量: {len(result.segments)}")

if __name__ == '__main__':
    # 运行单元测试
    unittest.main(verbosity=2)
    
    # 运行性能测试
    print("\n" + "="*50)
    print("性能测试:")
    run_performance_test()