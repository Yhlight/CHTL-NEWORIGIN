#!/usr/bin/env python3
"""
调试use语句解析问题
"""

import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from chtl_compiler import CHTLLexer, CHTLParser, TokenType

def debug_use_parsing():
    """调试use语句解析"""
    print("🔍 调试use语句解析")
    print("=" * 50)
    
    test_code = "use html5;"
    print(f"测试代码: {test_code}")
    
    # 词法分析
    lexer = CHTLLexer()
    tokens = lexer.tokenize(test_code)
    
    print(f"\n词法分析结果 ({len(tokens)} 个标记):")
    for i, token in enumerate(tokens):
        print(f"  {i}: {token.type} = '{token.value}' (line {token.line}, col {token.column})")
    
    # 语法分析
    parser = CHTLParser(tokens)
    try:
        ast = parser.parse()
        print(f"\n语法分析结果:")
        print(f"  解析的节点数: {len(ast)}")
        for i, node in enumerate(ast):
            print(f"  {i}: {type(node).__name__}")
            if hasattr(node, 'directive'):
                print(f"      指令: {node.directive}")
                print(f"      参数: {node.args}")
    except Exception as e:
        print(f"\n语法分析错误: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    debug_use_parsing()