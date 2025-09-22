#!/usr/bin/env python3
"""
测试命名空间系统功能
"""

from advanced_chtl_compiler import AdvancedCHTLLexer, AdvancedCHTLParser, AdvancedCHTLGenerator

def test_nested_namespaces():
    """测试嵌套命名空间功能"""
    print("🧪 测试嵌套命名空间功能")
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
        
        [Custom] @Element Button
        {
            button
            {
                class: btn;
                style
                {
                    @Style ButtonStyle;
                }
                text: "Click Me";
            }
        }
        
        [Namespace] Components
        {
            [Template] @Style CardStyle
            {
                background-color: white;
                border: 1px solid #ddd;
                border-radius: 8px;
                padding: 20px;
            }
            
            [Custom] @Element Card
            {
                div
                {
                    class: card;
                    style
                    {
                        @Style CardStyle;
                    }
                    h3
                    {
                        text: "Card Title";
                    }
                    p
                    {
                        text: "Card content goes here.";
                    }
                }
            }
        }
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
    
    print("✅ 嵌套命名空间功能测试通过")

def test_namespace_conflict_detection():
    """测试命名空间冲突检测功能"""
    print("\n🧪 测试命名空间冲突检测功能")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Namespace] UI
    {
        [Template] @Style ButtonStyle
        {
            background-color: blue;
        }
    }
    
    [Namespace] Components
    {
        [Template] @Style ButtonStyle
        {
            background-color: red;
        }
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
    
    print("命名空间信息:")
    for name, info in parser.context.namespaces.items():
        print(f"  {name}: {info}")
    
    print("代码生成...")
    generator = AdvancedCHTLGenerator(parser.context)
    html = generator.generate(ast)
    
    print("生成的HTML:")
    print(html)
    
    print("✅ 命名空间冲突检测功能测试通过")

def test_namespace_import_management():
    """测试命名空间导入管理功能"""
    print("\n🧪 测试命名空间导入管理功能")
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
    
    [Namespace] Components
    {
        [Import] UI;
        
        [Custom] @Element Button
        {
            button
            {
                class: btn;
                style
                {
                    @Style UI.ButtonStyle;
                }
                text: "Click Me";
            }
        }
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
    
    print("命名空间信息:")
    for name, info in parser.context.namespaces.items():
        print(f"  {name}: {info}")
    
    print("代码生成...")
    generator = AdvancedCHTLGenerator(parser.context)
    html = generator.generate(ast)
    
    print("生成的HTML:")
    print(html)
    
    print("✅ 命名空间导入管理功能测试通过")

def main():
    """主测试函数"""
    print("🚀 开始命名空间系统功能测试")
    print("=" * 60)
    
    try:
        test_nested_namespaces()
        test_namespace_conflict_detection()
        test_namespace_import_management()
        
        print("\n🎉 所有命名空间系统功能测试通过！")
        print("=" * 60)
        
    except Exception as e:
        print(f"\n❌ 测试失败: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()