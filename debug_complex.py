#!/usr/bin/env python3
"""
调试复杂CHTL文件测试
"""

from unified_scanner import UnifiedScanner

def debug_complex_file():
    """调试复杂CHTL文件"""
    scanner = UnifiedScanner()
    
    content = """
    use html5;
    
    [Custom] @Style BoxStyle {
        color: white;
        background-color: rgb(255, 192, 203);
    }
    
    [Custom] @Var Theme {
        Sum: white,
    }
    
    html {
        head {
            style {
                body {
                    data-theme: Theme(Sum);  /* CHTL语法 */
                }
                
                .box {
                    width: 100px;
                    height: 100px / 2;  /* CHTL语法 */
                    @Style BoxStyle;    /* CHTL语法 */
                }
            }
        }
        
        body {
            div {
                id: $test$;  /* CHTL JS语法 */
                
                script {
                    let v = "Test";
                    {{v}} -> Listen {  /* CHTL JS语法 */
                        click: () => { console.log({{v}}) }  /* JS语法 */
                    }
                }
            }
        }
    }
    """
    
    print("原始内容:")
    print(content)
    print("\n" + "="*50 + "\n")
    
    # 查找style块
    style_blocks = scanner.find_style_blocks(content)
    print("找到的style块:")
    for start, end, block_type in style_blocks:
        print(f"位置: {start}-{end}, 类型: {block_type}")
        print(f"内容: {content[start:end]}")
        print()
    
    # 查找script块
    script_blocks = scanner.find_script_blocks(content)
    print("找到的script块:")
    for start, end, block_type in script_blocks:
        print(f"位置: {start}-{end}, 类型: {block_type}")
        print(f"内容: {content[start:end]}")
        print()
    
    # 执行扫描
    result = scanner.scan(content)
    print("扫描结果:")
    print(result['processed_content'])
    print(f"\n占位符数量: {len(result['placeholders'])}")
    for placeholder_id, info in result['placeholders'].items():
        print(f"  {placeholder_id}: {info.original_content} ({info.token_type.value})")

if __name__ == "__main__":
    debug_complex_file()