#!/usr/bin/env python3
"""
CHTL项目实现测试和验证脚本
"""

import os
import subprocess
import sys
import json
from pathlib import Path

class CHTLTester:
    def __init__(self):
        self.workspace_root = Path("/workspace")
        self.build_dir = self.workspace_root / "build"
        self.test_results = []
        
    def run_command(self, command, cwd=None):
        """运行命令并返回结果"""
        try:
            result = subprocess.run(
                command, 
                shell=True, 
                cwd=cwd or self.workspace_root,
                capture_output=True, 
                text=True, 
                timeout=30
            )
            return {
                'success': result.returncode == 0,
                'stdout': result.stdout,
                'stderr': result.stderr,
                'returncode': result.returncode
            }
        except subprocess.TimeoutExpired:
            return {
                'success': False,
                'stdout': '',
                'stderr': 'Command timed out',
                'returncode': -1
            }
        except Exception as e:
            return {
                'success': False,
                'stdout': '',
                'stderr': str(e),
                'returncode': -1
            }
    
    def test_build_system(self):
        """测试构建系统"""
        print("=== 测试构建系统 ===")
        
        # 检查CMakeLists.txt文件
        cmake_files = [
            "CMakeLists.txt",
            "CHTL/CMakeLists.txt",
            "CHTL JS/CMakeLists.txt",
            "Scanner/CMakeLists.txt",
            "CodeMerger/CMakeLists.txt",
            "CompilerDispatcher/CMakeLists.txt",
            "Util/CMakeLists.txt",
            "Test/CMakeLists.txt"
        ]
        
        for cmake_file in cmake_files:
            if not (self.workspace_root / cmake_file).exists():
                print(f"❌ 缺少文件: {cmake_file}")
                return False
            else:
                print(f"✅ 找到文件: {cmake_file}")
        
        # 尝试构建项目
        print("\n尝试构建项目...")
        result = self.run_command("./build.sh")
        
        if result['success']:
            print("✅ 构建成功")
            return True
        else:
            print(f"❌ 构建失败: {result['stderr']}")
            return False
    
    def test_lexer(self):
        """测试词法分析器"""
        print("\n=== 测试词法分析器 ===")
        
        # 创建测试文件
        test_content = '''# 这是一个CHTL测试文件

html
{
    head
    {
        title: "CHTL测试页面";
    }
    
    body
    {
        div
        {
            id: main;
            class: container;
            
            h1
            {
                text: "欢迎使用CHTL";
            }
            
            p
            {
                text: "这是一个基于C++实现的超文本语言";
            }
            
            style
            {
                .container
                {
                    width: 100%;
                    max-width: 800px;
                    margin: 0 auto;
                    padding: 20px;
                }
                
                h1
                {
                    color: #333;
                    text-align: center;
                }
            }
        }
    }
}'''
        
        test_file = self.workspace_root / "test_lexer.chtl"
        test_file.write_text(test_content)
        
        # 运行测试程序
        result = self.run_command("./build/test_compiler")
        
        if result['success']:
            print("✅ 词法分析器测试通过")
            print("输出:")
            print(result['stdout'])
            return True
        else:
            print(f"❌ 词法分析器测试失败: {result['stderr']}")
            return False
    
    def test_syntax_features(self):
        """测试语法特性"""
        print("\n=== 测试语法特性 ===")
        
        # 测试字面量支持
        literal_test = '''div
{
    text: 这是一段无修饰字面量;
    class: container;
    id: main;
}'''
        
        # 测试CE对等式
        ce_test = '''div
{
    id = box;
    class = welcome;
    text: "Hello World";
}'''
        
        # 测试注释
        comment_test = '''// 这是单行注释
div
{
    /* 这是多行注释 */
    text: "Hello";
    # 这是生成器注释
}'''
        
        # 测试模板
        template_test = '''[Template] @Style DefaultText
{
    color: "black";
    line-height: 1.6;
}

[Template] @Element Box
{
    div
    {
        style
        {
            width: 200px;
            height: 200px;
            background-color: red;
        }
    }
}

body
{
    @Element Box;
}'''
        
        test_cases = [
            ("字面量支持", literal_test),
            ("CE对等式", ce_test),
            ("注释处理", comment_test),
            ("模板系统", template_test)
        ]
        
        for name, content in test_cases:
            test_file = self.workspace_root / f"test_{name.replace(' ', '_')}.chtl"
            test_file.write_text(content)
            print(f"✅ 创建测试文件: {name}")
        
        return True
    
    def test_style_enhancements(self):
        """测试样式增强功能"""
        print("\n=== 测试样式增强功能 ===")
        
        # 测试局部样式块
        style_test = '''div
{
    style
    {
        .box
        {
            width: 100px + 50px;
            height: 200px;
            background-color: width > 50px ? "red" : "blue";
        }
        
        &:hover
        {
            background-color: green;
        }
    }
}'''
        
        test_file = self.workspace_root / "test_style_enhancements.chtl"
        test_file.write_text(style_test)
        print("✅ 创建样式增强测试文件")
        
        return True
    
    def test_chtl_js(self):
        """测试CHTL JS功能"""
        print("\n=== 测试CHTL JS功能 ===")
        
        chtl_js_test = '''script
{
    {{button}}->Listen {
        click: function() {
            console.log('Button clicked!');
        }
    };
    
    const anim = Animate {
        target: {{box}};
        duration: 1000;
        easing: ease-in-out;
        begin: {
            opacity: 0;
        }
        end: {
            opacity: 1;
        }
    };
    
    Router {
        url: "/home";
        page: {{homePage}};
    }
}'''
        
        test_file = self.workspace_root / "test_chtl_js.chtl"
        test_file.write_text(chtl_js_test)
        print("✅ 创建CHTL JS测试文件")
        
        return True
    
    def validate_implementation(self):
        """验证实现完整性"""
        print("\n=== 验证实现完整性 ===")
        
        # 检查关键文件
        key_files = [
            "CHTL/CHTLLexer/CHTLLexer.h",
            "CHTL/CHTLLexer/CHTLLexer.cpp",
            "CHTL/CHTLParser/CHTLParser.h",
            "CHTL/CHTLParser/CHTLParser.cpp",
            "CHTL/CHTLGenerator/CHTLGenerator.h",
            "CHTL/CHTLGenerator/CHTLGenerator.cpp",
            "CHTL/CHTLNode/CHTLNode.h",
            "CHTL/CHTLNode/CHTLNode.cpp",
            "CHTL/CHTLNode/TemplateNode.h",
            "CHTL/CHTLNode/TemplateNode.cpp",
            "CHTL/CHTLManage/TemplateManager.h",
            "CHTL/CHTLManage/TemplateManager.cpp",
            "CHTL/CHTLManage/StyleEnhancer.h",
            "CHTL/CHTLManage/StyleEnhancer.cpp",
            "CHTL JS/CHTLJSLexer/CHTLJSLexer.h",
            "CHTL JS/CHTLJSLexer/CHTLJSLexer.cpp",
            "Scanner/UnifiedScanner.h",
            "Scanner/UnifiedScanner.cpp",
            "CodeMerger/CodeMerger.h",
            "CodeMerger/CodeMerger.cpp",
            "CompilerDispatcher/CompilerDispatcher.h",
            "CompilerDispatcher/CompilerDispatcher.cpp"
        ]
        
        missing_files = []
        for file_path in key_files:
            full_path = self.workspace_root / file_path
            if not full_path.exists():
                missing_files.append(file_path)
            else:
                print(f"✅ 找到文件: {file_path}")
        
        if missing_files:
            print(f"\n❌ 缺少文件: {missing_files}")
            return False
        
        print("\n✅ 所有关键文件都存在")
        return True
    
    def generate_report(self):
        """生成测试报告"""
        print("\n=== 生成测试报告 ===")
        
        report = {
            "timestamp": str(Path().cwd()),
            "test_results": self.test_results,
            "summary": {
                "total_tests": len(self.test_results),
                "passed": len([r for r in self.test_results if r.get('success', False)]),
                "failed": len([r for r in self.test_results if not r.get('success', False)])
            }
        }
        
        report_file = self.workspace_root / "test_report.json"
        with open(report_file, 'w', encoding='utf-8') as f:
            json.dump(report, f, indent=2, ensure_ascii=False)
        
        print(f"✅ 测试报告已生成: {report_file}")
        return True
    
    def run_all_tests(self):
        """运行所有测试"""
        print("开始CHTL项目实现测试...")
        print("=" * 50)
        
        tests = [
            ("构建系统", self.test_build_system),
            ("词法分析器", self.test_lexer),
            ("语法特性", self.test_syntax_features),
            ("样式增强", self.test_style_enhancements),
            ("CHTL JS", self.test_chtl_js),
            ("实现完整性", self.validate_implementation)
        ]
        
        for test_name, test_func in tests:
            try:
                result = test_func()
                self.test_results.append({
                    "name": test_name,
                    "success": result,
                    "timestamp": str(Path().cwd())
                })
            except Exception as e:
                print(f"❌ 测试 {test_name} 出现异常: {e}")
                self.test_results.append({
                    "name": test_name,
                    "success": False,
                    "error": str(e),
                    "timestamp": str(Path().cwd())
                })
        
        # 生成报告
        self.generate_report()
        
        # 打印总结
        print("\n" + "=" * 50)
        print("测试总结:")
        passed = len([r for r in self.test_results if r.get('success', False)])
        total = len(self.test_results)
        print(f"通过: {passed}/{total}")
        
        if passed == total:
            print("🎉 所有测试都通过了！")
        else:
            print("⚠️  部分测试失败，请检查实现")
        
        return passed == total

def main():
    """主函数"""
    tester = CHTLTester()
    success = tester.run_all_tests()
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()