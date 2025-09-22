#!/usr/bin/env python3
"""
统一扫描器测试用例
基于CHTL.md中的示例和规范进行测试
"""

import unittest
import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from unified_scanner import UnifiedScanner, TokenType

class TestUnifiedScanner(unittest.TestCase):
    """统一扫描器测试类"""
    
    def setUp(self):
        """测试前准备"""
        self.scanner = UnifiedScanner()
    
    def test_global_style_chtl_syntax_detection(self):
        """测试全局style块中CHTL语法检测"""
        content = """
        <style>
            body {
                data-theme: Theme(Sum);  /* CHTL语法：模板变量 */
            }
            
            .box {
                width: 100px;
                height: 100px / 2;  /* CHTL语法：属性运算 */
                @Style BoxStyle;    /* CHTL语法：模板样式组 */
            }
        </style>
        """
        
        result = self.scanner.scan(content)
        processed_content = result['processed_content']
        placeholders = result['placeholders']
        
        # 验证CHTL语法被替换为占位符
        self.assertIn('CSS_CODE_PLACEHOLDER_', processed_content)
        self.assertNotIn('Theme(Sum)', processed_content)
        self.assertNotIn('100px / 2', processed_content)
        self.assertNotIn('@Style BoxStyle', processed_content)
        
        # 验证占位符信息（全局style块中的CHTL语法使用CSS_SYNTAX类型）
        css_placeholders = [p for p in placeholders.values() if p.token_type == TokenType.CSS_SYNTAX]
        self.assertGreater(len(css_placeholders), 0)
    
    def test_local_script_mixed_syntax(self):
        """测试局部script中的混合语法"""
        content = """
        <div>
            <script>
                let v = "Test";
                {{v}} -> Listen {       /* CHTL JS语法 */
                    click: () => { console.log({{v}}) }  /* JS语法 */
                }
            </script>
        </div>
        """
        
        result = self.scanner.scan(content)
        processed_content = result['processed_content']
        placeholders = result['placeholders']
        
        # 验证混合语法被正确分离
        self.assertIn('JS_CODE_PLACEHOLDER_', processed_content)
        self.assertIn('CHTL_JS_CODE_PLACEHOLDER_', processed_content)
        
        # 验证占位符类型
        js_placeholders = [p for p in placeholders.values() if p.token_type == TokenType.JS_SYNTAX]
        chtl_js_placeholders = [p for p in placeholders.values() if p.token_type == TokenType.CHTL_JS_SYNTAX]
        
        self.assertGreater(len(js_placeholders), 0)
        self.assertGreater(len(chtl_js_placeholders), 0)
    
    def test_chtl_js_js_separation(self):
        """测试CHTL JS和JS代码分离"""
        content = """
        <script>
            function test(a,b,c) {
                return {{a}};
            }
            
            const a = {{a}};
        </script>
        """
        
        result = self.scanner.scan(content)
        processed_content = result['processed_content']
        placeholders = result['placeholders']
        
        # 验证JS代码被替换为占位符
        self.assertIn('JS_CODE_PLACEHOLDER_', processed_content)
        self.assertIn('CHTL_JS_CODE_PLACEHOLDER_', processed_content)
        
        # 验证原始JS代码被保留在占位符中
        js_placeholders = [p for p in placeholders.values() if p.token_type == TokenType.JS_SYNTAX]
        chtl_js_placeholders = [p for p in placeholders.values() if p.token_type == TokenType.CHTL_JS_SYNTAX]
        
        self.assertGreater(len(js_placeholders), 0)
        self.assertGreater(len(chtl_js_placeholders), 0)
    
    def test_placeholder_restoration(self):
        """测试占位符恢复功能"""
        original_content = """
        <style>
            .box {
                width: 100px / 2;
                @Style BoxStyle;
            }
        </style>
        """
        
        result = self.scanner.scan(original_content)
        processed_content = result['processed_content']
        
        # 恢复占位符
        restored_content = self.scanner.restore_placeholders(processed_content)
        
        # 验证恢复后的内容包含原始CHTL语法
        self.assertIn('100px / 2', restored_content)
        self.assertIn('@Style BoxStyle', restored_content)
    
    def test_complex_chtl_file(self):
        """测试复杂CHTL文件的扫描"""
        content = """
        use html5;
        
        [Custom] @Style BoxStyle {
            color: white;
            background-color: rgb(255, 192, 203);
        }
        
        [Custom] @Var Theme {
            Sum: white,
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
        
        result = self.scanner.scan(content)
        processed_content = result['processed_content']
        placeholders = result['placeholders']
        
        # 验证各种语法都被正确处理
        self.assertIn('CSS_CODE_PLACEHOLDER_', processed_content)
        self.assertIn('JS_CODE_PLACEHOLDER_', processed_content)
        self.assertIn('CHTL_JS_CODE_PLACEHOLDER_', processed_content)
        
        # 验证占位符数量
        self.assertGreater(len(placeholders), 0)
        
        # 验证不同类型的占位符都存在
        token_types = set(p.token_type for p in placeholders.values())
        self.assertIn(TokenType.CSS_SYNTAX, token_types)  # 全局style块中的CHTL语法
        self.assertIn(TokenType.CHTL_JS_SYNTAX, token_types)
        self.assertIn(TokenType.JS_SYNTAX, token_types)
    
    def test_chtl_js_functions(self):
        """测试CHTL JS函数检测"""
        content = """
        <script>
            Listen {
                click: () => { console.log('clicked') }
            }
            
            Animate {
                target: {{box}},
                duration: 100,
                easing: ease-in-out
            }
            
            ScriptLoader {
                load: ./module.cjjs,
                load: ./module2.cjjs
            }
        </script>
        """
        
        result = self.scanner.scan(content)
        processed_content = result['processed_content']
        placeholders = result['placeholders']
        
        # 验证CHTL JS函数被检测
        chtl_js_placeholders = [p for p in placeholders.values() if p.token_type == TokenType.CHTL_JS_SYNTAX]
        self.assertGreater(len(chtl_js_placeholders), 0)
        
        # 验证原始内容被保留在占位符中
        # 检查是否有任何占位符包含这些函数名
        all_placeholders = [p.original_content for p in chtl_js_placeholders]
        has_listen = any('Listen' in content for content in all_placeholders)
        has_animate = any('Animate' in content for content in all_placeholders)
        has_scriptloader = any('ScriptLoader' in content for content in all_placeholders)
        
        self.assertTrue(has_listen or has_animate or has_scriptloader, 
                       f"Expected to find Listen, Animate, or ScriptLoader in placeholders: {all_placeholders}")
    
    def test_js_boundary_detection(self):
        """测试JS语法边界检测"""
        content = """
        function testFunction() {
            return "test";
        }
        
        const testVar = () => {
            console.log("arrow function");
        };
        
        class TestClass {
            constructor() {
                this.value = "test";
            }
        }
        
        if (true) {
            console.log("if statement");
        }
        
        for (let i = 0; i < 10; i++) {
            console.log(i);
        }
        """
        
        result = self.scanner.scan(content)
        processed_content = result['processed_content']
        placeholders = result['placeholders']
        
        # 验证JS语法边界被检测
        js_placeholders = [p for p in placeholders.values() if p.token_type == TokenType.JS_SYNTAX]
        self.assertGreater(len(js_placeholders), 0)
        
        # 验证原始JS代码被保留
        all_placeholders = [p.original_content for p in js_placeholders]
        has_function = any('function' in content for content in all_placeholders)
        has_class = any('class' in content for content in all_placeholders)
        has_if = any('if' in content for content in all_placeholders)
        
        self.assertTrue(has_function or has_class or has_if, 
                       f"Expected to find function, class, or if in placeholders: {all_placeholders}")
    
    def test_placeholder_export_import(self):
        """测试占位符导出和导入功能"""
        content = """
        <style>
            .box {
                width: 100px / 2;
                @Style BoxStyle;
            }
        </style>
        """
        
        # 扫描并导出
        result = self.scanner.scan(content)
        self.scanner.export_placeholders('/tmp/test_placeholders.json')
        
        # 创建新的扫描器并导入
        new_scanner = UnifiedScanner()
        new_scanner.import_placeholders('/tmp/test_placeholders.json')
        
        # 验证导入的占位符信息
        self.assertEqual(len(new_scanner.placeholders), len(self.scanner.placeholders))
        
        for placeholder_id, info in new_scanner.placeholders.items():
            self.assertIn(placeholder_id, self.scanner.placeholders)
            original_info = self.scanner.placeholders[placeholder_id]
            self.assertEqual(info.original_content, original_info.original_content)
            self.assertEqual(info.token_type, original_info.token_type)
    
    def test_edge_cases(self):
        """测试边界情况"""
        # 空内容
        result = self.scanner.scan("")
        self.assertEqual(result['processed_content'], "")
        self.assertEqual(len(result['placeholders']), 0)
        
        # 只有HTML标签
        html_content = "<div><p>Hello World</p></div>"
        result = self.scanner.scan(html_content)
        self.assertEqual(result['processed_content'], html_content)
        self.assertEqual(len(result['placeholders']), 0)
        
        # 只有CSS
        css_content = """
        <style>
            .box { width: 100px; height: 100px; }
        </style>
        """
        result = self.scanner.scan(css_content)
        self.assertEqual(result['processed_content'], css_content)
        self.assertEqual(len(result['placeholders']), 0)
    
    def test_nested_syntax(self):
        """测试嵌套语法"""
        content = """
        <script>
            function outerFunction() {
                const innerFunction = () => {
                    {{box}} -> Listen {
                        click: () => {
                            console.log('nested click');
                        }
                    };
                };
                return innerFunction;
            }
        </script>
        """
        
        result = self.scanner.scan(content)
        processed_content = result['processed_content']
        placeholders = result['placeholders']
        
        # 验证嵌套语法被正确处理
        self.assertIn('JS_CODE_PLACEHOLDER_', processed_content)
        self.assertIn('CHTL_JS_CODE_PLACEHOLDER_', processed_content)
        
        # 验证占位符包含正确的嵌套内容
        js_placeholders = [p for p in placeholders.values() if p.token_type == TokenType.JS_SYNTAX]
        chtl_js_placeholders = [p for p in placeholders.values() if p.token_type == TokenType.CHTL_JS_SYNTAX]
        
        self.assertGreater(len(js_placeholders), 0)
        self.assertGreater(len(chtl_js_placeholders), 0)

def run_performance_test():
    """性能测试"""
    import time
    
    scanner = UnifiedScanner()
    
    # 创建大型测试文件
    large_content = """
    <style>
        body {
            color: white;
            background-color: rgb(255, 192, 203);
        }
    </style>
    """ * 1000  # 重复1000次
    
    start_time = time.time()
    result = scanner.scan(large_content)
    end_time = time.time()
    
    print(f"处理大型文件耗时: {end_time - start_time:.4f}秒")
    print(f"占位符数量: {len(result['placeholders'])}")

if __name__ == '__main__':
    # 运行单元测试
    unittest.main(verbosity=2)
    
    # 运行性能测试
    print("\n" + "="*50)
    print("性能测试:")
    run_performance_test()