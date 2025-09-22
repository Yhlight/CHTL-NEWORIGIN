#!/usr/bin/env python3
"""
调试统一扫描器
"""

import re
from unified_scanner import UnifiedScanner

def debug_patterns():
    """调试正则表达式模式"""
    scanner = UnifiedScanner()
    
    # 测试CHTL语法检测
    test_content = """
    <style>
        body {
            data-theme: Theme(Sum);  /* CHTL语法：模板变量 */
        }
        
        .box {
            width: 100px;
            height: 100px / 2;  /* CHTL语法：属性运算 */
            @Style BoxStyle;    /* CHTL语法：模板样式组 */
        }
    </style>
    """
    
    print("测试内容:")
    print(test_content)
    print("\n" + "="*50 + "\n")
    
    # 查找style块
    style_blocks = scanner.find_style_blocks(test_content)
    print("找到的style块:")
    for start, end, block_type in style_blocks:
        print(f"位置: {start}-{end}, 类型: {block_type}")
        print(f"内容: {test_content[start:end]}")
        print()
    
    # 测试CHTL语法检测
    if style_blocks:
        start, end, block_type = style_blocks[0]
        block_content = test_content[start:end]
        print(f"处理style块内容: {block_content}")
        
        chtl_tokens = scanner.detect_chtl_syntax(block_content, start)
        print(f"检测到的CHTL标记数量: {len(chtl_tokens)}")
        for token in chtl_tokens:
            print(f"  - {token.content} (位置: {token.start_pos}-{token.end_pos})")
    
    print("\n" + "="*50 + "\n")
    
    # 测试CHTL JS语法检测
    js_test_content = """
    {{v}} -> Listen {
        click: () => { console.log({{v}}) }
    }
    """
    
    print("测试CHTL JS内容:")
    print(js_test_content)
    
    chtl_js_tokens = scanner.detect_chtl_js_syntax(js_test_content)
    print(f"检测到的CHTL JS标记数量: {len(chtl_js_tokens)}")
    for token in chtl_js_tokens:
        print(f"  - {token.content} (位置: {token.start_pos}-{token.end_pos})")
    
    print("\n" + "="*50 + "\n")
    
    # 测试JS语法检测
    js_test_content2 = """
    function test(a,b,c) {
        return "test";
    }
    
    const testVar = () => {
        console.log("arrow function");
    };
    """
    
    print("测试JS内容:")
    print(js_test_content2)
    
    js_tokens = scanner.detect_js_boundaries(js_test_content2)
    print(f"检测到的JS标记数量: {len(js_tokens)}")
    for token in js_tokens:
        print(f"  - {token.content} (位置: {token.start_pos}-{token.end_pos})")

def test_individual_patterns():
    """测试单个正则表达式模式"""
    test_strings = [
        "height: 100px / 2;",  # 属性运算
        "Theme(Sum);",         # 模板变量
        "@Style BoxStyle;",    # 模板样式组
        "{{v}}",               # 增强选择器
        "{{v}} -> Listen {",   # 箭头CHTL JS
        "function test() {",   # 函数声明
        "() => {",             # 箭头函数
    ]
    
    patterns = [
        (r'[a-zA-Z-]+\s*:\s*[^;{}]*[\+\-\*\/\%\*\*][^;{}]*;', '属性运算'),
        (r'[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]+\)', '模板变量'),
        (r'@Style\s+[a-zA-Z_][a-zA-Z0-9_]*', '模板样式组'),
        (r'\{\{[^}]+\}\}', '增强选择器'),
        (r'[a-zA-Z_][a-zA-Z0-9_]*\s*->\s*[a-zA-Z_][a-zA-Z0-9_]*', '箭头CHTL JS'),
        (r'function\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]*\)\s*\{', '函数声明'),
        (r'[a-zA-Z_][a-zA-Z0-9_]*\s*=>\s*\{', '箭头函数'),
    ]
    
    for test_string in test_strings:
        print(f"测试字符串: {test_string}")
        for pattern, name in patterns:
            match = re.search(pattern, test_string)
            if match:
                print(f"  ✓ 匹配 {name}: {match.group(0)}")
            else:
                print(f"  ✗ 不匹配 {name}")
        print()

if __name__ == "__main__":
    print("调试正则表达式模式:")
    test_individual_patterns()
    
    print("\n" + "="*50 + "\n")
    print("调试扫描器:")
    debug_patterns()