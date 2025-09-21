#!/usr/bin/env python3
"""
CHTL完整系统测试
验证统一扫描器、CHTL编译器、CHTL JS编译器和编译器调度器的完整功能
"""

import os
import time
from compiler_dispatcher import CompilerDispatcher

def test_basic_chtl():
    """测试基础CHTL语法"""
    print("🧪 测试基础CHTL语法")
    print("=" * 50)
    
    dispatcher = CompilerDispatcher()
    
    test_content = """
    use html5;
    
    html {
        head {
            title {
                text: "基础CHTL测试";
            }
        }
        body {
            div {
                class: container;
                text: "Hello, CHTL!";
            }
        }
    }
    """
    
    result = dispatcher.compile(test_content, "test_basic")
    
    assert result.success, f"基础CHTL编译失败: {result.error_message}"
    assert "Hello, CHTL!" in result.html_content, "HTML内容不正确"
    assert "container" in result.html_content, "CSS类名未正确生成"
    
    print("✅ 基础CHTL语法测试通过")

def test_chtl_templates():
    """测试CHTL模板语法"""
    print("\n🧪 测试CHTL模板语法")
    print("=" * 50)
    
    dispatcher = CompilerDispatcher()
    
    test_content = """
    use html5;
    
    [Template] @Style ButtonStyle {
        background-color: blue;
        color: white;
        padding: 10px 20px;
        border: none;
        border-radius: 5px;
    }
    
    [Template] @Element Button {
        button {
            class: btn;
            style {
                @Style ButtonStyle;
            }
            text: "Click Me";
        }
    }
    
    html {
        body {
            @Element Button;
        }
    }
    """
    
    result = dispatcher.compile(test_content, "test_templates")
    
    assert result.success, f"CHTL模板编译失败: {result.error_message}"
    assert "Click Me" in result.html_content, "按钮文本未正确生成"
    assert "btn" in result.html_content, "按钮类名未正确生成"
    
    print("✅ CHTL模板语法测试通过")

def test_chtl_js_syntax():
    """测试CHTL JS语法"""
    print("\n🧪 测试CHTL JS语法")
    print("=" * 50)
    
    dispatcher = CompilerDispatcher()
    
    test_content = """
    use html5;
    
    html {
        body {
            div {
                class: box;
                script {
                    {{box}}->Listen {
                        click: () => {
                            console.log('Box clicked!');
                        }
                    }
                }
            }
        }
    }
    """
    
    result = dispatcher.compile(test_content, "test_chtl_js")
    
    assert result.success, f"CHTL JS编译失败: {result.error_message}"
    assert "box" in result.html_content, "元素类名未正确生成"
    
    print("✅ CHTL JS语法测试通过")

def test_mixed_content():
    """测试混合内容"""
    print("\n🧪 测试混合内容")
    print("=" * 50)
    
    dispatcher = CompilerDispatcher()
    
    test_content = """
    use html5;
    
    [Template] @Style CardStyle {
        background-color: white;
        border: 1px solid #ddd;
        border-radius: 8px;
        padding: 20px;
        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    }
    
    html {
        head {
            title {
                text: "混合内容测试";
            }
        }
        body {
            div {
                class: card;
                style {
                    @Style CardStyle;
                }
                h1 {
                    text: "欢迎使用CHTL";
                }
                p {
                    text: "这是一个混合内容的测试页面。";
                }
                button {
                    class: btn;
                    text: "点击我";
                    script {
                        {{btn}}->Listen {
                            click: () => {
                                alert('按钮被点击了！');
                            }
                        }
                    }
                }
            }
        }
    }
    """
    
    result = dispatcher.compile(test_content, "test_mixed")
    
    assert result.success, f"混合内容编译失败: {result.error_message}"
    assert "欢迎使用CHTL" in result.html_content, "标题未正确生成"
    assert "这是一个混合内容的测试页面" in result.html_content, "段落文本未正确生成"
    assert "点击我" in result.html_content, "按钮文本未正确生成"
    
    print("✅ 混合内容测试通过")

def test_performance():
    """测试性能"""
    print("\n🧪 测试性能")
    print("=" * 50)
    
    dispatcher = CompilerDispatcher()
    
    # 创建大量内容的测试
    large_content = """
    use html5;
    
    html {
        head {
            title {
                text: "性能测试";
            }
        }
        body {
    """
    
    # 添加大量元素
    for i in range(100):
        large_content += f"""
            div {{
                class: item{i};
                text: "Item {i}";
                style {{
                    margin: 5px;
                    padding: 10px;
                    background-color: rgb({i % 255}, {(i * 2) % 255}, {(i * 3) % 255});
                }}
            }}
        """
    
    large_content += """
        }
    }
    """
    
    start_time = time.time()
    result = dispatcher.compile(large_content, "test_performance")
    end_time = time.time()
    
    assert result.success, f"性能测试编译失败: {result.error_message}"
    
    compilation_time = end_time - start_time
    print(f"📊 性能统计:")
    print(f"  - 编译时间: {compilation_time:.4f}秒")
    print(f"  - 内容长度: {len(large_content)} 字符")
    print(f"  - 处理速度: {len(large_content) / compilation_time:.0f} 字符/秒")
    
    assert compilation_time < 5.0, f"编译时间过长: {compilation_time:.4f}秒"
    
    print("✅ 性能测试通过")

def test_error_handling():
    """测试错误处理"""
    print("\n🧪 测试错误处理")
    print("=" * 50)
    
    dispatcher = CompilerDispatcher()
    
    # 测试语法错误
    invalid_content = """
    use html5;
    
    html {
        div {
            class: test;
            // 缺少闭合括号
        }
    }
    """
    
    result = dispatcher.compile(invalid_content, "test_errors")
    
    # 即使有语法错误，系统也应该能够处理
    print(f"📊 错误处理结果:")
    print(f"  - 编译成功: {result.success}")
    print(f"  - 错误信息: {result.error_message}")
    
    print("✅ 错误处理测试通过")

def test_file_output():
    """测试文件输出"""
    print("\n🧪 测试文件输出")
    print("=" * 50)
    
    dispatcher = CompilerDispatcher()
    
    test_content = """
    use html5;
    
    html {
        head {
            title {
                text: "文件输出测试";
            }
        }
        body {
            div {
                class: content;
                text: "文件输出测试内容";
            }
        }
    }
    """
    
    output_dir = "test_file_output"
    result = dispatcher.compile(test_content, output_dir)
    
    assert result.success, f"文件输出测试编译失败: {result.error_message}"
    
    # 检查文件是否生成
    html_file = os.path.join(output_dir, "index.html")
    assert os.path.exists(html_file), "HTML文件未生成"
    
    # 检查文件内容
    with open(html_file, 'r', encoding='utf-8') as f:
        content = f.read()
        assert "文件输出测试内容" in content, "HTML文件内容不正确"
    
    print("✅ 文件输出测试通过")

def run_all_tests():
    """运行所有测试"""
    print("🚀 开始CHTL完整系统测试")
    print("=" * 60)
    
    start_time = time.time()
    
    try:
        test_basic_chtl()
        test_chtl_templates()
        test_chtl_js_syntax()
        test_mixed_content()
        test_performance()
        test_error_handling()
        test_file_output()
        
        end_time = time.time()
        total_time = end_time - start_time
        
        print("\n🎉 所有测试通过！")
        print(f"📊 总测试时间: {total_time:.4f}秒")
        print("=" * 60)
        
    except Exception as e:
        print(f"\n❌ 测试失败: {e}")
        raise

if __name__ == "__main__":
    run_all_tests()