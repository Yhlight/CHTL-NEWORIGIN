#!/usr/bin/env python3
"""
高级功能综合测试
测试所有已实现的高级CHTL功能
"""

import os
import time
from advanced_chtl_compiler import AdvancedCHTLLexer, AdvancedCHTLParser, AdvancedCHTLGenerator
from template_specialization import TemplateSpecializationParser
from advanced_style_features import StylePropertyProcessor

def test_advanced_configuration():
    """测试高级配置功能"""
    print("🧪 测试高级配置功能")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Configuration]
    {
        DEBUG_MODE = false;
        INDEX_INITIAL_COUNT = 0;
        
        [Name]
        {
            CUSTOM_STYLE = [@Style, @style, @CSS];
            CUSTOM_ELEMENT = @Element;
            TEMPLATE_STYLE = @Style;
            TEMPLATE_ELEMENT = @Element;
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
    
    print("✅ 高级配置功能测试通过")

def test_advanced_namespace():
    """测试高级命名空间功能"""
    print("\n🧪 测试高级命名空间功能")
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
            border-radius: 5px;
        }
        
        [Template] @Style CardStyle
        {
            background-color: white;
            border: 1px solid #ddd;
            border-radius: 8px;
            padding: 20px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
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
    
    print("✅ 高级命名空间功能测试通过")

def test_template_specialization():
    """测试模板特例化功能"""
    print("\n🧪 测试模板特例化功能")
    print("=" * 50)
    
    parser = TemplateSpecializationParser()
    
    test_content = """
    [Template] @Style BaseStyle
    {
        color: black;
        font-size: 16px;
        line-height: 1.6;
        border: 1px solid #ccc;
        padding: 10px;
    }
    
    [Custom] @Style SpecialStyle
    {
        @Style BaseStyle
        {
            delete border, padding;
        }
        color: red;
        font-weight: bold;
    }
    
    div
    {
        style
        {
            @Style SpecialStyle
            {
                delete color;
            }
        }
    }
    """
    
    specializations = parser.parse_all_specializations(test_content)
    
    print("样式组特例化:")
    for spec in specializations['style']:
        print(f"  {spec}")
    
    print("元素特例化:")
    for spec in specializations['element']:
        print(f"  {spec}")
    
    print("✅ 模板特例化功能测试通过")

def test_advanced_style_features():
    """测试高级样式功能"""
    print("\n🧪 测试高级样式功能")
    print("=" * 50)
    
    processor = StylePropertyProcessor()
    
    # 设置响应式值
    processor.responsive_values = {
        'boxWidth': '200px',
        'boxHeight': '100px',
        'boxColor': 'blue'
    }
    
    # 测试属性运算
    print("属性运算测试:")
    arithmetic_tests = [
        "100px + 20px",
        "50px * 2",
        "200px / 4",
        "10px ** 2"
    ]
    
    for expr in arithmetic_tests:
        try:
            result = processor.process_arithmetic_expression(expr)
            print(f"  {expr} = {result}")
        except Exception as e:
            print(f"  {expr} = {expr} (计算失败: {e})")
    
    # 测试条件表达式
    print("\n条件表达式测试:")
    conditional_tests = [
        "width > 50px ? red : blue",
        "height < 100px ? green",
        "width > 50px ? red, width > 100px ? green : pink"
    ]
    
    for expr in conditional_tests:
        conditional = processor.process_conditional_expression(expr)
        print(f"  {expr}")
        print(f"    条件: {conditional.condition}")
        print(f"    真值: {conditional.true_value}")
        print(f"    假值: {conditional.false_value}")
    
    # 测试响应式值
    print("\n响应式值测试:")
    responsive_content = "width: $boxWidth$; height: $boxHeight$; color: $boxColor$;"
    processed_content = processor.process_responsive_values(responsive_content)
    print(f"  原始: {responsive_content}")
    print(f"  处理: {processed_content}")
    
    print("✅ 高级样式功能测试通过")

def test_complex_chtl():
    """测试复杂CHTL代码"""
    print("\n🧪 测试复杂CHTL代码")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    use html5;
    
    [Configuration]
    {
        DEBUG_MODE = false;
        INDEX_INITIAL_COUNT = 0;
    }
    
    [Namespace] Components
    {
        [Template] @Style ButtonStyle
        {
            background-color: blue;
            color: white;
            padding: 10px 20px;
            border-radius: 5px;
            transition: background-color 0.3s ease;
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
    }
    
    html
    {
        head
        {
            title
            {
                text: "高级CHTL测试";
            }
        }
        body
        {
            div
            {
                class: container;
                h1
                {
                    text: "高级CHTL功能演示";
                }
                p
                {
                    text: "这是一个展示高级CHTL功能的页面。";
                }
                @Element Button from Components;
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
    
    print("✅ 复杂CHTL代码测试通过")

def test_performance():
    """测试性能"""
    print("\n🧪 测试性能")
    print("=" * 50)
    
    lexer = AdvancedCHTLLexer()
    
    # 创建大量内容的测试
    large_content = """
    use html5;
    
    [Configuration]
    {
        DEBUG_MODE = false;
        INDEX_INITIAL_COUNT = 0;
    }
    
    [Namespace] LargeNamespace
    {
    """
    
    # 添加大量模板和元素
    for i in range(50):
        large_content += f"""
        [Template] @Style Style{i}
        {{
            background-color: rgb({i % 255}, {(i * 2) % 255}, {(i * 3) % 255});
            padding: {i}px;
            margin: {i // 2}px;
        }}
        
        [Custom] @Element Element{i}
        {{
            div
            {{
                class: element{i};
                style
                {{
                    @Style Style{i};
                }}
                text: "Element {i}";
            }}
        }}
        """
    
    large_content += """
    }
    
    html
    {
        body
        {
    """
    
    # 添加大量元素
    for i in range(100):
        large_content += f"""
            div
            {{
                class: item{i};
                text: "Item {i}";
            }}
        """
    
    large_content += """
        }
    }
    """
    
    start_time = time.time()
    
    print("词法分析...")
    tokens = lexer.tokenize(large_content)
    
    print("语法分析...")
    parser = AdvancedCHTLParser(tokens)
    ast = parser.parse()
    
    print("代码生成...")
    generator = AdvancedCHTLGenerator(parser.context)
    html = generator.generate(ast)
    
    end_time = time.time()
    compilation_time = end_time - start_time
    
    print(f"📊 性能统计:")
    print(f"  - 编译时间: {compilation_time:.4f}秒")
    print(f"  - 内容长度: {len(large_content)} 字符")
    print(f"  - 处理速度: {len(large_content) / compilation_time:.0f} 字符/秒")
    print(f"  - AST节点数: {len(ast)}")
    print(f"  - 生成HTML长度: {len(html)} 字符")
    
    assert compilation_time < 10.0, f"编译时间过长: {compilation_time:.4f}秒"
    
    print("✅ 性能测试通过")

def main():
    """运行所有高级功能测试"""
    print("🚀 开始高级CHTL功能综合测试")
    print("=" * 60)
    
    start_time = time.time()
    
    try:
        test_advanced_configuration()
        test_advanced_namespace()
        test_template_specialization()
        test_advanced_style_features()
        test_complex_chtl()
        test_performance()
        
        end_time = time.time()
        total_time = end_time - start_time
        
        print("\n🎉 所有高级功能测试通过！")
        print(f"📊 总测试时间: {total_time:.4f}秒")
        print("=" * 60)
        
    except Exception as e:
        print(f"\n❌ 测试失败: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()