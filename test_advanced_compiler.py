#!/usr/bin/env python3
"""
测试高级CHTL编译器
"""

from advanced_chtl_compiler import AdvancedCHTLLexer, AdvancedCHTLParser, AdvancedCHTLGenerator

def test_simple_configuration():
    """测试简单配置"""
    print("🧪 测试简单配置")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Configuration]
    {
        DEBUG_MODE = false;
        INDEX_INITIAL_COUNT = 0;
    }
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = AdvancedCHTLParser(tokens)
    ast = parser.parse()
    
    print("AST节点:")
    for node in ast:
        print(f"  {node.to_dict()}")
    
    print("✅ 简单配置测试通过")

def test_simple_namespace():
    """测试简单命名空间"""
    print("\n🧪 测试简单命名空间")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Namespace] MyNamespace
    {
        [Template] @Style ButtonStyle
        {
            background-color: blue;
            color: white;
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
        print(f"  {node.to_dict()}")
    
    print("✅ 简单命名空间测试通过")

def test_simple_html():
    """测试简单HTML"""
    print("\n🧪 测试简单HTML")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    html
    {
        body
        {
            div
            {
                text: "Hello World";
            }
        }
    }
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = AdvancedCHTLParser(tokens)
    ast = parser.parse()
    
    print("代码生成...")
    generator = AdvancedCHTLGenerator(parser.context)
    html = generator.generate(ast)
    
    print("生成的HTML:")
    print(html)
    
    print("✅ 简单HTML测试通过")

def main():
    """运行所有测试"""
    print("🚀 开始高级CHTL编译器测试")
    print("=" * 60)
    
    try:
        test_simple_configuration()
        test_simple_namespace()
        test_simple_html()
        
        print("\n🎉 所有测试通过！")
        print("=" * 60)
        
    except Exception as e:
        print(f"\n❌ 测试失败: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()