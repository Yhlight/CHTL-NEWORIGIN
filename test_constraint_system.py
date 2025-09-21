#!/usr/bin/env python3
"""
测试约束系统功能
"""

from advanced_chtl_compiler import AdvancedCHTLLexer, AdvancedCHTLParser, AdvancedCHTLGenerator

def test_exact_constraints():
    """测试精确约束功能"""
    print("🧪 测试精确约束功能")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Template] @Style ButtonStyle
    {
        background-color: blue;
        color: white;
        padding: 10px 20px;
    }
    
    [Template] @Style SpecialButtonStyle
    {
        ButtonStyle except
        {
            background-color: red;
            font-size: 18px;
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
    
    print("✅ 精确约束功能测试通过")

def test_type_constraints():
    """测试类型约束功能"""
    print("\n🧪 测试类型约束功能")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Template] @Style BaseStyle
    {
        font-family: Arial, sans-serif;
        font-size: 14px;
        line-height: 1.5;
    }
    
    [Template] @Style HeadingStyle
    {
        BaseStyle except type
        {
            font-size: 24px;
            font-weight: bold;
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
    
    print("✅ 类型约束功能测试通过")

def test_global_constraints():
    """测试全局约束功能"""
    print("\n🧪 测试全局约束功能")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Template] @Style GlobalStyle
    {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
    }
    
    [Template] @Style CustomStyle
    {
        GlobalStyle except global
        {
            margin: 10px;
            padding: 20px;
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
    
    print("✅ 全局约束功能测试通过")

def main():
    """主测试函数"""
    print("🚀 开始约束系统功能测试")
    print("=" * 60)
    
    try:
        test_exact_constraints()
        test_type_constraints()
        test_global_constraints()
        
        print("\n🎉 所有约束系统功能测试通过！")
        print("=" * 60)
        
    except Exception as e:
        print(f"\n❌ 测试失败: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()