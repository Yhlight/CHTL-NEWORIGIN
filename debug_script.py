#!/usr/bin/env python3
"""
调试script块检测
"""

import re

def test_script_patterns():
    """测试script块正则表达式"""
    content = """
    script {
        let v = "Test";
        {{v}} -> Listen {  /* CHTL JS语法 */
            click: () => { console.log({{v}}) }  /* JS语法 */
        }
    }
    """
    
    print("测试内容:")
    print(content)
    print("\n" + "="*50 + "\n")
    
    # 测试不同的正则表达式模式
    patterns = [
        (r'script\s*\{([^}]*)\}', '简单模式'),
        (r'script\s*\{([^{}]*(?:\{[^{}]*\}[^{}]*)*)\}', '嵌套模式'),
        (r'script\s*\{([^{}]*(?:\{[^{}]*\}[^{}]*)*)\}', '改进嵌套模式'),
    ]
    
    for pattern, name in patterns:
        print(f"测试模式: {name}")
        matches = list(re.finditer(pattern, content, re.MULTILINE | re.DOTALL))
        print(f"匹配数量: {len(matches)}")
        for i, match in enumerate(matches):
            print(f"  匹配 {i+1}: 位置 {match.start()}-{match.end()}")
            print(f"  内容: {match.group(1)}")
        print()

if __name__ == "__main__":
    test_script_patterns()