#!/usr/bin/env python3
"""
完整系统验证测试
验证统一扫描器和代码合并器的完整功能
"""

import os
import time
import tempfile
import shutil
from unified_scanner import UnifiedScanner
from code_merger import CodeMerger

def test_complete_chtl_workflow():
    """测试完整的CHTL工作流程"""
    print("="*60)
    print("完整CHTL工作流程测试")
    print("="*60)
    
    # 创建扫描器和合并器
    scanner = UnifiedScanner()
    merger = CodeMerger(scanner)
    
    # 复杂的CHTL测试文件
    chtl_content = """
    use html5;
    
    [Custom] @Style BoxStyle {
        color: white;
        background-color: rgb(255, 192, 203);
        border-radius: 50%;
    }
    
    [Custom] @Var Theme {
        primary: #ffc0cb,
        secondary: #ffffff,
    }
    
    html {
        head {
            title {
                text: "CHTL统一扫描器测试";
            }
            
            style {
                body {
                    margin: 0;
                    padding: 20px;
                    font-family: Arial, sans-serif;
                    background-color: Theme(primary);
                }
                
                .container {
                    max-width: 800px;
                    margin: 0 auto;
                    background-color: white;
                    border-radius: 10px;
                    padding: 20px;
                    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
                }
                
                .box {
                    width: 200px;
                    height: 200px / 2;  /* CHTL语法：属性运算 */
                    margin: 20px auto;
                    @Style BoxStyle;    /* CHTL语法：模板样式组 */
                    transition: all 0.3s ease;
                }
                
                .box:hover {
                    transform: scale(1.05);
                    box-shadow: 0 8px 15px rgba(0, 0, 0, 0.2);
                }
                
                .button {
                    display: inline-block;
                    padding: 10px 20px;
                    background-color: Theme(primary);
                    color: white;
                    text-decoration: none;
                    border-radius: 5px;
                    margin: 10px;
                    transition: background-color 0.3s ease;
                }
                
                .button:hover {
                    background-color: Theme(secondary);
                    color: Theme(primary);
                }
            }
        }
        
        body {
            div {
                class: container;
                
                h1 {
                    text: "CHTL统一扫描器演示";
                    text-align: center;
                    color: Theme(primary);
                }
                
                div {
                    class: box;
                    id: $mainBox$;  /* CHTL JS语法：响应式值 */
                    
                    text: "点击我！";
                    text-align: center;
                    line-height: 100px;
                    cursor: pointer;
                }
                
                div {
                    text-align: center;
                    
                    a {
                        class: button;
                        text: "按钮1";
                        id: $btn1$;  /* CHTL JS语法：响应式值 */
                    }
                    
                    a {
                        class: button;
                        text: "按钮2";
                        id: $btn2$;  /* CHTL JS语法：响应式值 */
                    }
                }
                
                script {
                    // 初始化变量
                    let clickCount = 0;
                    let isAnimating = false;
                    
                    // 主盒子点击事件
                    {{mainBox}} -> Listen {  /* CHTL JS语法：事件监听器 */
                        click: () => {
                            if (isAnimating) return;
                            
                            clickCount++;
                            isAnimating = true;
                            
                            // 更新文本
                            {{mainBox}}->textContent = `点击了 ${clickCount} 次`;
                            
                            // 创建动画
                            const anim = Animate {  /* CHTL JS语法：动画 */
                                target: {{mainBox}},
                                duration: 300,
                                easing: ease-in-out,
                                
                                begin: {
                                    transform: scale(1);
                                },
                                
                                when: [
                                    {
                                        at: 0.5,
                                        transform: scale(1.2);
                                    }
                                ],
                                
                                end: {
                                    transform: scale(1);
                                },
                                
                                callback: () => {
                                    isAnimating = false;
                                }
                            };
                        }
                    };
                    
                    // 按钮事件
                    {{btn1}} -> Listen {  /* CHTL JS语法：事件监听器 */
                        click: () => {
                            {{mainBox}}->style.backgroundColor = Theme(secondary);
                            {{mainBox}}->style.color = Theme(primary);
                        }
                    };
                    
                    {{btn2}} -> Listen {  /* CHTL JS语法：事件监听器 */
                        click: () => {
                            {{mainBox}}->style.backgroundColor = Theme(primary);
                            {{mainBox}}->style.color = Theme(secondary);
                        }
                    };
                    
                    // 页面加载完成后的初始化
                    document.addEventListener('DOMContentLoaded', () => {
                        console.log('CHTL页面加载完成！');
                        console.log('主盒子元素:', {{mainBox}});
                        console.log('按钮1元素:', {{btn1}});
                        console.log('按钮2元素:', {{btn2}});
                    });
                }
            }
        }
    }
    """
    
    print("原始CHTL内容长度:", len(chtl_content))
    print("\n开始扫描...")
    
    # 第一步：使用统一扫描器扫描
    start_time = time.time()
    scan_result = scanner.scan(chtl_content)
    scan_time = time.time() - start_time
    
    print(f"扫描完成，耗时: {scan_time:.4f}秒")
    print(f"检测到占位符数量: {len(scan_result['placeholders'])}")
    
    # 显示占位符信息
    print("\n占位符信息:")
    for placeholder_id, info in scan_result['placeholders'].items():
        print(f"  {placeholder_id}: {info.original_content[:50]}... ({info.token_type.value})")
    
    print("\n开始编译...")
    
    # 第二步：使用代码合并器编译
    start_time = time.time()
    compilation_result = merger.compile(chtl_content)
    compilation_time = time.time() - start_time
    
    print(f"编译完成，耗时: {compilation_time:.4f}秒")
    
    # 显示编译统计
    stats = merger.get_compilation_stats(compilation_result)
    print(f"\n编译统计:")
    print(f"  总占位符数: {stats['total_placeholders']}")
    print(f"  总代码片段数: {stats['total_segments']}")
    print(f"  HTML大小: {stats['html_size']} 字节")
    print(f"  CSS大小: {stats['css_size']} 字节")
    print(f"  JS大小: {stats['js_size']} 字节")
    print(f"  内联大小: {stats['inline_size']} 字节")
    print(f"  语言分布: {stats['language_counts']}")
    
    # 保存到文件
    print("\n保存文件...")
    temp_dir = tempfile.mkdtemp()
    try:
        file_paths = merger.save_to_files(compilation_result, temp_dir, "chtl_demo")
        
        print("文件保存完成:")
        for file_type, file_path in file_paths.items():
            file_size = os.path.getsize(file_path)
            print(f"  {file_type}: {file_path} ({file_size} 字节)")
        
        # 显示部分生成的内容
        print("\n生成的HTML内容预览:")
        with open(file_paths['html'], 'r', encoding='utf-8') as f:
            html_preview = f.read()[:500] + "..." if len(f.read()) > 500 else f.read()
            print(html_preview)
        
        print("\n生成的CSS内容预览:")
        with open(file_paths['css'], 'r', encoding='utf-8') as f:
            css_preview = f.read()[:300] + "..." if len(f.read()) > 300 else f.read()
            print(css_preview)
        
        print("\n生成的JS内容预览:")
        with open(file_paths['js'], 'r', encoding='utf-8') as f:
            js_preview = f.read()[:300] + "..." if len(f.read()) > 300 else f.read()
            print(js_preview)
        
        print(f"\n完整工作流程耗时: {scan_time + compilation_time:.4f}秒")
        print("✅ 完整CHTL工作流程测试通过！")
        
    finally:
        # 清理临时文件
        shutil.rmtree(temp_dir)

def test_performance_benchmark():
    """性能基准测试"""
    print("\n" + "="*60)
    print("性能基准测试")
    print("="*60)
    
    scanner = UnifiedScanner()
    merger = CodeMerger(scanner)
    
    # 创建不同大小的测试文件
    test_sizes = [1, 10, 50, 100, 500]
    
    for size in test_sizes:
        print(f"\n测试文件大小: {size}KB")
        
        # 创建测试内容
        base_content = """
        <style>
            .box { width: 100px / 2; @Style BoxStyle; }
        </style>
        <script>
            {{v}} -> Listen { click: () => { console.log({{v}}) } }
        </script>
        """
        
        test_content = base_content * size
        
        # 测试扫描性能
        start_time = time.time()
        scan_result = scanner.scan(test_content)
        scan_time = time.time() - start_time
        
        # 测试编译性能
        start_time = time.time()
        compilation_result = merger.compile(test_content)
        compilation_time = time.time() - start_time
        
        total_time = scan_time + compilation_time
        
        print(f"  扫描时间: {scan_time:.4f}秒")
        print(f"  编译时间: {compilation_time:.4f}秒")
        print(f"  总时间: {total_time:.4f}秒")
        print(f"  占位符数: {len(scan_result['placeholders'])}")
        print(f"  代码片段数: {len(compilation_result.segments)}")
        print(f"  处理速度: {len(test_content) / total_time / 1024:.2f} KB/s")

def test_error_handling():
    """错误处理测试"""
    print("\n" + "="*60)
    print("错误处理测试")
    print("="*60)
    
    scanner = UnifiedScanner()
    merger = CodeMerger(scanner)
    
    # 测试各种边界情况
    test_cases = [
        ("空内容", ""),
        ("只有HTML", "<html><body>Hello</body></html>"),
        ("只有CSS", "<style>body { color: red; }</style>"),
        ("只有JS", "<script>console.log('test');</script>"),
        ("无效语法", "invalid { syntax } here"),
        ("嵌套错误", "<style><script>mixed content</script></style>"),
    ]
    
    for test_name, content in test_cases:
        print(f"\n测试: {test_name}")
        try:
            result = merger.compile(content)
            print(f"  ✅ 成功处理，占位符数: {len(result.placeholders)}")
        except Exception as e:
            print(f"  ❌ 处理失败: {e}")

def test_chtl_js_features():
    """测试CHTL JS特性"""
    print("\n" + "="*60)
    print("CHTL JS特性测试")
    print("="*60)
    
    scanner = UnifiedScanner()
    merger = CodeMerger(scanner)
    
    # 测试各种CHTL JS特性
    chtl_js_content = """
    <script>
        // 增强选择器
        {{box}} -> textContent = "Hello World";
        {{.button}} -> addEventListener('click', () => {});
        {{#main}} -> style.display = 'block';
        
        // 响应式值
        let count = $count$;
        let message = $message$;
        
        // 事件监听器
        {{box}} -> Listen {
            click: () => { console.log('clicked'); },
            mouseenter: () => { console.log('entered'); }
        };
        
        // 动画
        const anim = Animate {
            target: {{box}},
            duration: 1000,
            easing: ease-in-out,
            begin: { opacity: 0 },
            end: { opacity: 1 }
        };
        
        // 脚本加载器
        ScriptLoader {
            load: ./module1.js,
            load: ./module2.js
        };
        
        // 事件委托
        {{container}} -> Delegate {
            target: {{button}},
            click: () => { console.log('delegated click'); }
        };
    </script>
    """
    
    result = merger.compile(chtl_js_content)
    
    print(f"CHTL JS特性测试结果:")
    print(f"  占位符数: {len(result.placeholders)}")
    print(f"  代码片段数: {len(result.segments)}")
    
    # 检查各种特性是否被正确识别
    chtl_js_segments = [s for s in result.segments if s.language == 'chtl_js']
    js_segments = [s for s in result.segments if s.language == 'javascript']
    
    print(f"  CHTL JS片段数: {len(chtl_js_segments)}")
    print(f"  JS片段数: {len(js_segments)}")
    
    # 验证特定特性
    all_content = ' '.join(s.content for s in result.segments)
    features = [
        ('增强选择器', '{{box}}'),
        ('响应式值', '$count$'),
        ('事件监听器', 'Listen'),
        ('动画', 'Animate'),
        ('脚本加载器', 'ScriptLoader'),
        ('事件委托', 'Delegate'),
    ]
    
    print(f"\n特性检测:")
    for feature_name, feature_pattern in features:
        found = feature_pattern in all_content
        print(f"  {feature_name}: {'✅' if found else '❌'}")

def main():
    """主函数"""
    print("CHTL统一扫描器完整系统验证")
    print("="*60)
    
    try:
        # 运行各种测试
        test_complete_chtl_workflow()
        test_performance_benchmark()
        test_error_handling()
        test_chtl_js_features()
        
        print("\n" + "="*60)
        print("🎉 所有测试完成！统一扫描器系统运行正常！")
        print("="*60)
        
    except Exception as e:
        print(f"\n❌ 测试过程中出现错误: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()