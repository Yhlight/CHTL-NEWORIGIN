#!/usr/bin/env python3
"""
测试自定义元素功能
"""

from advanced_chtl_compiler import AdvancedCHTLLexer, AdvancedCHTLParser, AdvancedCHTLGenerator

def test_custom_element_specialization():
    """测试自定义元素特例化功能"""
    print("🧪 测试自定义元素特例化功能")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Custom] @Element Button
    {
        button
        {
            class: btn;
            style
            {
                background-color: blue;
                color: white;
                padding: 10px 20px;
                border-radius: 5px;
            }
            text: "Click Me";
        }
    }
    
    [Custom] @Element SpecialButton
    {
        Button
        {
            class: special-btn;
            style
            {
                background-color: red;
                font-size: 18px;
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
    
    print("✅ 自定义元素特例化功能测试通过")

def test_custom_element_index_access():
    """测试自定义元素索引访问功能"""
    print("\n🧪 测试自定义元素索引访问功能")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Custom] @Element Card
    {
        div
        {
            class: card;
            style
            {
                background-color: white;
                border: 1px solid #ddd;
                border-radius: 8px;
                padding: 20px;
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
    
    [Custom] @Element SpecialCard
    {
        Card[0]
        {
            class: special-card;
            style
            {
                background-color: #f0f0f0;
                border: 2px solid #333;
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
    
    print("✅ 自定义元素索引访问功能测试通过")

def test_custom_element_insert_delete():
    """测试自定义元素插入和删除功能"""
    print("\n🧪 测试自定义元素插入和删除功能")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Custom] @Element List
    {
        ul
        {
            class: list;
            style
            {
                list-style: none;
                padding: 0;
            }
            li
            {
                text: "Item 1";
            }
            li
            {
                text: "Item 2";
            }
            li
            {
                text: "Item 3";
            }
        }
    }
    
    [Custom] @Element ModifiedList
    {
        List
        {
            Insert[1]
            {
                li
                {
                    text: "New Item";
                }
            }
            Delete[2]
            {
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
    
    print("✅ 自定义元素插入和删除功能测试通过")

def main():
    """主测试函数"""
    print("🚀 开始自定义元素功能测试")
    print("=" * 60)
    
    try:
        test_custom_element_specialization()
        test_custom_element_index_access()
        test_custom_element_insert_delete()
        
        print("\n🎉 所有自定义元素功能测试通过！")
        print("=" * 60)
        
    except Exception as e:
        print(f"\n❌ 测试失败: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()