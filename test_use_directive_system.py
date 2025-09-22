#!/usr/bin/env python3
"""
测试增强的use指令系统
"""

import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from chtl_compiler import CHTLLexer, CHTLParser, CHTLGenerator
from use_directive_system import UseDirectiveProcessor

def test_use_directive_parsing():
    """测试use指令解析"""
    print("🧪 测试use指令解析")
    print("=" * 50)
    
    test_cases = [
        "use html5;",
        'use bootstrap("5.3.0");',
        'use react("18.2.0", "development");',
        'use custom("my-directive", "arg1", "arg2");',
        "use css3;",
        "use javascript;"
    ]
    
    lexer = CHTLLexer()
    
    for i, test_code in enumerate(test_cases, 1):
        print(f"\n测试用例 {i}: {test_code}")
        try:
            tokens = lexer.tokenize(test_code)
            print(f"词法分析结果: {len(tokens)} 个标记")
            
            parser = CHTLParser(tokens)
            ast = parser.parse()
            if ast and len(ast) > 0:
                use_node = ast[0]
                print(f"解析结果: 指令={use_node.directive}, 参数={use_node.args}")
            else:
                print("解析失败: 未找到use语句")
        except Exception as e:
            print(f"解析错误: {e}")

def test_use_directive_generation():
    """测试use指令代码生成"""
    print("\n🧪 测试use指令代码生成")
    print("=" * 50)
    
    test_cases = [
        ("html5", []),
        ("bootstrap", ["5.3.0"]),
        ("react", ["18.2.0", "development"]),
        ("css3", []),
        ("javascript", []),
        ("custom", ["my-directive", "arg1", "arg2"])
    ]
    
    from chtl_compiler import CHTLContext
    context = CHTLContext()
    generator = CHTLGenerator(context)
    use_system = UseDirectiveProcessor()
    
    for directive, args in test_cases:
        print(f"\n测试指令: {directive} {args}")
        try:
            # 使用增强的use指令系统
            generated_code = use_system.generate_directive_code(directive, args)
            print(f"生成的代码长度: {len(generated_code)} 字符")
            print(f"代码预览: {generated_code[:100]}...")
        except Exception as e:
            print(f"生成错误: {e}")

def test_use_directive_integration():
    """测试use指令与CHTL编译器的集成"""
    print("\n🧪 测试use指令与CHTL编译器集成")
    print("=" * 50)
    
    test_code = '''
use html5;
use bootstrap("5.3.0");

[Template] Button {
    text { "Click me" }
    style {
        background-color: #007bff;
        color: white;
        padding: 10px 20px;
        border: none;
        border-radius: 4px;
    }
}

[Custom] MyButton {
    Button {
        text { "Custom Button" }
    }
}

MyButton
'''
    
    try:
        lexer = CHTLLexer()
        from chtl_compiler import CHTLContext
        context = CHTLContext()
        generator = CHTLGenerator(context)
        
        print("测试代码:")
        print(test_code)
        
        # 词法分析
        tokens = lexer.tokenize(test_code)
        print(f"\n词法分析: {len(tokens)} 个标记")
        
        # 语法分析
        parser = CHTLParser(tokens)
        ast = parser.parse()
        print(f"语法分析: 成功解析 {len(ast)} 个语句")
        
        # 代码生成
        generated_html = generator.generate(ast)
        print(f"\n生成的HTML长度: {len(generated_html)} 字符")
        print("生成的HTML预览:")
        print(generated_html[:500] + "..." if len(generated_html) > 500 else generated_html)
        
    except Exception as e:
        print(f"集成测试错误: {e}")
        import traceback
        traceback.print_exc()

def test_use_directive_validation():
    """测试use指令验证"""
    print("\n🧪 测试use指令验证")
    print("=" * 50)
    
    use_system = UseDirectiveProcessor()
    
    # 测试有效指令
    valid_directives = ["html5", "css3", "javascript", "bootstrap", "react", "vue", "angular"]
    for directive in valid_directives:
        is_valid = use_system.is_valid_directive(directive)
        print(f"指令 '{directive}': {'✅ 有效' if is_valid else '❌ 无效'}")
    
    # 测试无效指令
    invalid_directives = ["invalid", "unknown", "test"]
    for directive in invalid_directives:
        is_valid = use_system.is_valid_directive(directive)
        print(f"指令 '{directive}': {'✅ 有效' if is_valid else '❌ 无效'}")

def test_use_directive_arguments():
    """测试use指令参数处理"""
    print("\n🧪 测试use指令参数处理")
    print("=" * 50)
    
    use_system = UseDirectiveProcessor()
    
    test_cases = [
        ("bootstrap", ["5.3.0"], "Bootstrap 5.3.0"),
        ("react", ["18.2.0", "development"], "React 18.2.0 开发模式"),
        ("vue", ["3.3.0"], "Vue 3.3.0"),
        ("angular", ["16.0.0"], "Angular 16.0.0"),
        ("custom", ["my-directive", "arg1", "arg2"], "自定义指令")
    ]
    
    for directive, args, description in test_cases:
        print(f"\n测试: {description}")
        try:
            generated_code = use_system.generate_directive_code(directive, args)
            print(f"参数: {args}")
            print(f"生成代码长度: {len(generated_code)} 字符")
            print(f"代码预览: {generated_code[:150]}...")
        except Exception as e:
            print(f"错误: {e}")

def main():
    """主测试函数"""
    print("🚀 CHTL Use指令系统综合测试")
    print("=" * 60)
    
    try:
        test_use_directive_parsing()
        test_use_directive_generation()
        test_use_directive_integration()
        test_use_directive_validation()
        test_use_directive_arguments()
        
        print("\n✅ 所有测试完成!")
        
    except Exception as e:
        print(f"\n❌ 测试失败: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()