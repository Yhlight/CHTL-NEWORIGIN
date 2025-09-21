#!/usr/bin/env python3
"""
测试导入系统功能
"""

from advanced_chtl_compiler import AdvancedCHTLLexer, AdvancedCHTLParser, AdvancedCHTLGenerator

def test_basic_import():
    """测试基本导入功能"""
    print("🧪 测试基本导入功能")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Import] UI;
    
    [Template] @Style ButtonStyle
    {
        background-color: blue;
        color: white;
        padding: 10px 20px;
    }
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = AdvancedCHTLParser(tokens)
    ast = parser.parse()
    
    print("AST节点:")
    for node in ast:
        print(f"  {node}")
    
    print("代码生成...")
    generator = AdvancedCHTLGenerator(parser.context)
    html = generator.generate(ast)
    
    print("生成的HTML:")
    print(html)
    
    # 验证结果
    assert "UI" in parser.context.imported_namespaces
    
    print("✅ 基本导入功能测试通过")
    print()

def test_namespace_import():
    """测试命名空间导入功能"""
    print("🧪 测试命名空间导入功能")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Namespace] UI
    {
        [Template] @Style ButtonStyle
        {
            background-color: blue;
            color: white;
            padding: 10px 20px;
        }
    }
    
    [Import] UI;
    
    [Template] @Style SpecialButtonStyle
    {
        @Style UI.ButtonStyle;
        border-radius: 5px;
    }
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = AdvancedCHTLParser(tokens)
    ast = parser.parse()
    
    print("AST节点:")
    for node in ast:
        print(f"  {node}")
    
    print("代码生成...")
    generator = AdvancedCHTLGenerator(parser.context)
    html = generator.generate(ast)
    
    print("生成的HTML:")
    print(html)
    
    # 验证结果
    assert "UI" in parser.context.imported_namespaces
    assert "UI" in parser.context.namespaces
    
    print("✅ 命名空间导入功能测试通过")
    print()

def test_multiple_imports():
    """测试多个导入功能"""
    print("🧪 测试多个导入功能")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Namespace] UI
    {
        [Template] @Style ButtonStyle
        {
            background-color: blue;
            color: white;
        }
    }
    
    [Namespace] Layout
    {
        [Template] @Style ContainerStyle
        {
            max-width: 1200px;
            margin: 0 auto;
        }
    }
    
    [Import] UI;
    [Import] Layout;
    
    [Template] @Style PageStyle
    {
        @Style UI.ButtonStyle;
        @Style Layout.ContainerStyle;
    }
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = AdvancedCHTLParser(tokens)
    ast = parser.parse()
    
    print("AST节点:")
    for node in ast:
        print(f"  {node}")
    
    print("代码生成...")
    generator = AdvancedCHTLGenerator(parser.context)
    html = generator.generate(ast)
    
    print("生成的HTML:")
    print(html)
    
    # 验证结果
    assert "UI" in parser.context.imported_namespaces
    assert "Layout" in parser.context.imported_namespaces
    assert "UI" in parser.context.namespaces
    assert "Layout" in parser.context.namespaces
    
    print("✅ 多个导入功能测试通过")
    print()

def test_import_error_handling():
    """测试导入错误处理"""
    print("🧪 测试导入错误处理")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Import] NonExistentNamespace;
    
    [Template] @Style ButtonStyle
    {
        background-color: blue;
    }
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = AdvancedCHTLParser(tokens)
    ast = parser.parse()
    
    print("AST节点:")
    for node in ast:
        print(f"  {node}")
    
    print("代码生成...")
    generator = AdvancedCHTLGenerator(parser.context)
    html = generator.generate(ast)
    
    print("生成的HTML:")
    print(html)
    
    # 验证结果 - 应该仍然工作，只是导入的命名空间不存在
    assert "NonExistentNamespace" in parser.context.imported_namespaces
    
    print("✅ 导入错误处理测试通过")
    print()

def main():
    """主测试函数"""
    print("🚀 开始导入系统功能测试")
    print("=" * 60)
    
    try:
        test_basic_import()
        test_namespace_import()
        test_multiple_imports()
        test_import_error_handling()
        
        print("🎉 所有导入系统功能测试通过！")
        print("=" * 60)
        
    except Exception as e:
        print(f"❌ 测试失败: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()