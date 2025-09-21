#!/usr/bin/env python3
"""
测试原始嵌入系统功能
"""

from advanced_chtl_compiler import AdvancedCHTLLexer, AdvancedCHTLParser, AdvancedCHTLGenerator

def test_origin_html():
    """测试原始HTML嵌入"""
    print("🧪 测试原始HTML嵌入")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Origin HTML]
    {
        <div class="custom-html">
            <h1>Custom HTML Content</h1>
            <p>This is raw HTML content embedded in CHTL.</p>
        </div>
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
    assert "Custom HTML Content" in html
    assert "raw HTML content" in html
    assert "custom-html" in html
    
    print("✅ 原始HTML嵌入测试通过")
    print()

def test_origin_style():
    """测试原始样式嵌入"""
    print("🧪 测试原始样式嵌入")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Origin Style]
    {
        .custom-style {
            background: linear-gradient(45deg, #ff6b6b, #4ecdc4);
            border-radius: 10px;
            padding: 20px;
            box-shadow: 0 4px 8px rgba(0,0,0,0.1);
        }
        
        .custom-style:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 12px rgba(0,0,0,0.15);
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
    
    # 验证结果
    assert "linear-gradient" in html
    assert "custom-style" in html
    assert "box-shadow" in html
    
    print("✅ 原始样式嵌入测试通过")
    print()

def test_origin_javascript():
    """测试原始JavaScript嵌入"""
    print("🧪 测试原始JavaScript嵌入")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Origin JavaScript]
    {
        function customFunction() {
            console.log('Custom JavaScript function');
            document.querySelector('.custom-style').addEventListener('click', function() {
                alert('Custom JavaScript event handler');
            });
        }
        
        // 初始化
        document.addEventListener('DOMContentLoaded', customFunction);
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
    assert "customFunction" in html
    assert "addEventListener" in html
    assert "DOMContentLoaded" in html
    
    print("✅ 原始JavaScript嵌入测试通过")
    print()

def test_mixed_origin():
    """测试混合原始嵌入"""
    print("🧪 测试混合原始嵌入")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Origin HTML]
    {
        <div class="container">
            <h1>Mixed Content</h1>
        </div>
    }
    
    [Origin Style]
    {
        .container {
            max-width: 800px;
            margin: 0 auto;
            padding: 20px;
        }
    }
    
    [Origin JavaScript]
    {
        document.addEventListener('DOMContentLoaded', function() {
            console.log('Mixed origin content loaded');
        });
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
    assert "Mixed Content" in html
    assert "max-width" in html
    assert "DOMContentLoaded" in html
    
    print("✅ 混合原始嵌入测试通过")
    print()

def main():
    """主测试函数"""
    print("🚀 开始原始嵌入系统功能测试")
    print("=" * 60)
    
    try:
        test_origin_html()
        test_origin_style()
        test_origin_javascript()
        test_mixed_origin()
        
        print("🎉 所有原始嵌入系统功能测试通过！")
        print("=" * 60)
        
    except Exception as e:
        print(f"❌ 测试失败: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()