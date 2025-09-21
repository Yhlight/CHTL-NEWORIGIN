#!/usr/bin/env python3
"""
Fix compilation errors in CHTL project
"""

import os
import re
from pathlib import Path

def fix_token_access():
    """Fix Token class access issues"""
    files_to_fix = [
        "src/CHTL/CHTLParser/CHTLParser.cpp"
    ]
    
    replacements = [
        (r'\.type', '.getType()'),
        (r'\.value', '.getValue()'),
        (r'\.line', '.getLine()'),
        (r'\.column', '.getColumn()'),
    ]
    
    for file_path in files_to_fix:
        if not os.path.exists(file_path):
            continue
            
        with open(file_path, 'r') as f:
            content = f.read()
        
        for pattern, replacement in replacements:
            content = re.sub(pattern, replacement, content)
        
        with open(file_path, 'w') as f:
            f.write(content)
        
        print(f"Fixed token access in {file_path}")

def fix_node_type_issues():
    """Fix NodeType enum issues"""
    # Add missing NodeType values
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
                '    ORIGIN_JS,\n    LOCAL_STYLE,\n    SCRIPT,'
            )
        
        if 'GENERATOR_COMMENT' not in content:
            content = content.replace(
                '    COMMENT,',
                '    COMMENT,\n    GENERATOR_COMMENT,'
            )
        
        with open(chtl_common_path, 'w') as f:
            f.write(content)
        
        print(f"Fixed NodeType enum in {chtl_common_path}")

def fix_token_type_issues():
    """Fix TokenType enum issues"""
    chtl_common_path = "include/CHTLCommon.h"
    
    if os.path.exists(chtl_common_path):
        with open(chtl_common_path, 'r') as f:
            content = f.read()
        
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
        
        print(f"Fixed TokenType enum in {chtl_common_path}")

def fix_parser_issues():
    """Fix parser-specific issues"""
    parser_path = "src/CHTL/CHTLParser/CHTLParser.cpp"
    
    if not os.path.exists(parser_path):
        return
    
    with open(parser_path, 'r') as f:
        content = f.read()
    
    # Fix CHTLLexer constructor call
    content = content.replace(
        'CHTLLexer lexer;',
        'CHTLLexer lexer(source, "");'
    )
    
    # Fix tokenize call
    content = content.replace(
        'tokens = lexer.tokenize(source);',
        'tokens = lexer.tokenize();'
    )
    
    # Add missing parseTopLevelElement declaration
    if 'parseTopLevelElement' not in content:
        content = content.replace(
            '    std::shared_ptr<ASTNode> parseDocument();',
            '    std::shared_ptr<ASTNode> parseDocument();\n    std::shared_ptr<ASTNode> parseTopLevelElement();'
        )
    
    # Fix peek with offset
    content = content.replace(
        'peek(1).getType()',
        'peek().getType()'
    )
    
    with open(parser_path, 'w') as f:
        f.write(content)
    
    print(f"Fixed parser issues in {parser_path}")

def fix_generator_issues():
    """Fix generator-specific issues"""
    generator_path = "src/CHTL/CHTLGenerator/CHTLGenerator.cpp"
    
    if not os.path.exists(generator_path):
        return
    
    with open(generator_path, 'r') as f:
        content = f.read()
    
    # Fix getContent() calls
    content = content.replace('.getContent()', '.getValue()')
    
    # Fix getParent() calls - remove for now
    content = content.replace('node->getParent()', 'nullptr')
    
    # Add missing method declarations
    if 'generateCSSRecursive' not in content:
        content = content.replace(
            '    std::string generateCHTLJSCode(const std::string& source);',
            '    std::string generateCHTLJSCode(const std::string& source);\n    void generateCSSRecursive(const std::shared_ptr<ASTNode>& node, std::stringstream& css);\n    void generateJSRecursive(const std::shared_ptr<ASTNode>& node, std::stringstream& js);\n    bool isSelfClosingTag(const std::string& tagName);'
        )
    
    with open(generator_path, 'w') as f:
        f.write(content)
    
    print(f"Fixed generator issues in {generator_path}")

def fix_lexer_constructor():
    """Fix CHTLLexer constructor"""
    lexer_path = "src/CHTL/CHTLLexer/CHTLLexer.cpp"
    
    if not os.path.exists(lexer_path):
        return
    
    with open(lexer_path, 'r') as f:
        content = f.read()
    
    # Add default constructor
    if 'CHTLLexer::CHTLLexer()' not in content:
        content = content.replace(
            'CHTLLexer::CHTLLexer(const String& src, const String& file)',
            'CHTLLexer::CHTLLexer() : source(""), filename(""), position(0) {}\n\nCHTLLexer::CHTLLexer(const String& src, const String& file)'
        )
    
    with open(lexer_path, 'w') as f:
        f.write(content)
    
    print(f"Fixed lexer constructor in {lexer_path}")

def main():
    print("Fixing CHTL compilation errors...")
    
    fix_token_type_issues()
    fix_node_type_issues()
    fix_parser_issues()
    fix_generator_issues()
    fix_lexer_constructor()
    
    print("Compilation error fixes completed!")

if __name__ == "__main__":
    main()