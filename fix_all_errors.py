#!/usr/bin/env python3
"""
Comprehensive fix for all CHTL compilation errors
"""

import os
import re
from pathlib import Path

def fix_chtl_common():
    """Fix CHTLCommon.h enum issues"""
    chtl_common_path = "include/CHTLCommon.h"
    
    if os.path.exists(chtl_common_path):
        with open(chtl_common_path, 'r') as f:
            content = f.read()
        
        # Add missing NodeType values
        if 'DOCUMENT' not in content:
            content = content.replace(
                '    // Basic nodes\n    ELEMENT,',
                '    // Basic nodes\n    DOCUMENT,\n    ELEMENT,'
            )
        
        if 'LOCAL_STYLE' not in content:
            content = content.replace(
                '    ORIGIN_JS,',
                '    ORIGIN_JS,\n    LOCAL_STYLE,\n    SCRIPT,\n    STYLE_PROPERTY,'
            )
        
        if 'GENERATOR_COMMENT' not in content:
            content = content.replace(
                '    COMMENT,',
                '    COMMENT,\n    GENERATOR_COMMENT,'
            )
        
        # Add missing TokenType values
        if 'UNQUOTED_STRING' not in content:
            content = content.replace(
                '    STRING_LITERAL,',
                '    STRING_LITERAL,\n    UNQUOTED_STRING,'
            )
        
        if 'WHITESPACE' not in content:
            content = content.replace(
                '    EOF_TOKEN,',
                '    EOF_TOKEN,\n    WHITESPACE,'
            )
        
        if 'ERROR' not in content:
            content = content.replace(
                '    EOF_TOKEN,',
                '    EOF_TOKEN,\n    ERROR,'
            )
        
        with open(chtl_common_path, 'w') as f:
            f.write(content)
        
        print(f"Fixed CHTLCommon.h enums")

def fix_token_access():
    """Fix all Token class access issues"""
    files_to_fix = [
        "src/CHTL/CHTLParser/CHTLParser.cpp"
    ]
    
    for file_path in files_to_fix:
        if not os.path.exists(file_path):
            continue
            
        with open(file_path, 'r') as f:
            content = f.read()
        
        # Fix all token access patterns
        replacements = [
            (r'\.type', '.getType()'),
            (r'\.value', '.getValue()'),
            (r'\.line', '.getLine()'),
            (r'\.column', '.getColumn()'),
        ]
        
        for pattern, replacement in replacements:
            content = re.sub(pattern, replacement, content)
        
        with open(file_path, 'w') as f:
            f.write(content)
        
        print(f"Fixed token access in {file_path}")

def fix_parser_issues():
    """Fix parser-specific issues"""
    parser_path = "src/CHTL/CHTLParser/CHTLParser.cpp"
    
    if not os.path.exists(parser_path):
        return
    
    with open(parser_path, 'r') as f:
        content = f.read()
    
    # Fix token assignment issue
    content = content.replace(
        'tokens = lexer.tokenize();',
        'auto tokenVector = lexer.tokenize();\n    tokens.clear();\n    for (const auto& tokenPtr : tokenVector) {\n        tokens.push_back(*tokenPtr);\n    }'
    )
    
    with open(parser_path, 'w') as f:
        f.write(content)
    
    print(f"Fixed parser token assignment in {parser_path}")

def fix_generator_issues():
    """Fix generator-specific issues"""
    generator_path = "src/CHTL/CHTLGenerator/CHTLGenerator.cpp"
    
    if not os.path.exists(generator_path):
        return
    
    with open(generator_path, 'r') as f:
        content = f.read()
    
    # Fix getContent() calls
    content = content.replace('.getContent()', '.getValue()')
    
    with open(generator_path, 'w') as f:
        f.write(content)
    
    print(f"Fixed generator issues in {generator_path}")

def fix_lexer_constructor():
    """Fix CHTLLexer constructor"""
    lexer_header_path = "src/CHTL/CHTLLexer/CHTLLexer.h"
    
    if not os.path.exists(lexer_header_path):
        return
    
    with open(lexer_header_path, 'r') as f:
        content = f.read()
    
    # Add default constructor declaration
    if 'CHTLLexer();' not in content:
        content = content.replace(
            '    CHTLLexer(const String& src, const String& file = "");',
            '    CHTLLexer();\n    CHTLLexer(const String& src, const String& file = "");'
        )
    
    with open(lexer_header_path, 'w') as f:
        f.write(content)
    
    print(f"Fixed lexer constructor declaration in {lexer_header_path}")

def fix_ast_node_content():
    """Add getContent method to ASTNode"""
    ast_header_path = "include/ASTNode.h"
    
    if not os.path.exists(ast_header_path):
        return
    
    with open(ast_header_path, 'r') as f:
        content = f.read()
    
    # Add getContent method
    if 'getContent()' not in content:
        content = content.replace(
            '    const String& getValue() const { return value; }',
            '    const String& getValue() const { return value; }\n    const String& getContent() const { return value; }'
        )
    
    with open(ast_header_path, 'w') as f:
        f.write(content)
    
    print(f"Added getContent method to ASTNode")

def main():
    print("Comprehensive CHTL compilation error fix...")
    
    fix_chtl_common()
    fix_token_access()
    fix_parser_issues()
    fix_generator_issues()
    fix_lexer_constructor()
    fix_ast_node_content()
    
    print("All compilation error fixes completed!")

if __name__ == "__main__":
    main()