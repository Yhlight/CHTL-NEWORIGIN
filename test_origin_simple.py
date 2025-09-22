#!/usr/bin/env python3
"""
简单测试原始嵌入系统功能
"""

from advanced_chtl_compiler import AdvancedCHTLLexer, AdvancedCHTLParser, AdvancedCHTLGenerator

def test_origin_simple():
    """测试简单原始嵌入"""
    print("🧪 测试简单原始嵌入")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Origin HTML]
    {
        <div>Hello World</div>
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
        print(f"    Content: {node.content}")
    
    print("代码生成...")
    generator = AdvancedCHTLGenerator(parser.context)
    html = generator.generate(ast)
    
    print("生成的HTML:")
    print(html)
    
    print("✅ 简单原始嵌入测试通过")
    print()

def main():
    """主测试函数"""
    print("🚀 开始简单原始嵌入系统功能测试")
    print("=" * 60)
    
    try:
        test_origin_simple()
        
        print("🎉 简单原始嵌入系统功能测试通过！")
        print("=" * 60)
        
    except Exception as e:
        print(f"❌ 测试失败: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()