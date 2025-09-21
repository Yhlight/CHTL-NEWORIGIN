#!/usr/bin/env python3
"""
调试高级CHTL词法分析器
"""

from advanced_chtl_compiler import AdvancedCHTLLexer

def debug_lexer():
    """调试词法分析器"""
    lexer = AdvancedCHTLLexer()
    
    test_code = """
    [Configuration]
    {
        DEBUG_MODE = false;
        INDEX_INITIAL_COUNT = 0;
    }
    """
    
    print("词法分析结果:")
    tokens = lexer.tokenize(test_code)
    
    for i, token in enumerate(tokens):
        if token.type.value != "WHITESPACE":
            print(f"  {i:2}: {token.type.value:20} | {token.value:20} | Line {token.line:3}, Col {token.column:3}")

if __name__ == "__main__":
    debug_lexer()