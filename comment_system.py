#!/usr/bin/env python3
"""
CHTL注释系统实现
支持生成器注释、多语言注释生成等功能
"""

import re
from typing import Dict, List, Optional, Tuple
from enum import Enum
from dataclasses import dataclass

class CommentType(Enum):
    """注释类型"""
    GENERATOR = "generator"      # 生成器注释 #
    LINE = "line"               # 单行注释 //
    BLOCK = "block"             # 多行注释 /* */
    HTML = "html"               # HTML注释 <!-- -->
    CSS = "css"                 # CSS注释 /* */
    JS = "js"                   # JavaScript注释 // 或 /* */

@dataclass
class Comment:
    """注释数据结构"""
    type: CommentType
    content: str
    line: int
    column: int
    language: str = "auto"  # 目标语言
    preserved: bool = True  # 是否保留在输出中

class CommentProcessor:
    """注释处理器"""
    
    def __init__(self):
        self.comments: List[Comment] = []
        self.language_configs = {
            "html": {
                "line": "<!-- {} -->",
                "block": "<!-- {} -->",
                "generator": "<!-- {} -->"
            },
            "css": {
                "line": "/* {} */",
                "block": "/* {} */",
                "generator": "/* {} */"
            },
            "js": {
                "line": "// {}",
                "block": "/* {} */",
                "generator": "// {}"
            },
            "python": {
                "line": "# {}",
                "block": '""" {} """',
                "generator": "# {}"
            }
        }
    
    def extract_comments(self, source: str, language: str = "auto") -> Tuple[str, List[Comment]]:
        """从源代码中提取注释"""
        self.comments = []
        processed_source = source
        
        # 提取生成器注释
        processed_source = self._extract_generator_comments(processed_source, language)
        
        # 提取单行注释
        processed_source = self._extract_line_comments(processed_source, language)
        
        # 提取多行注释
        processed_source = self._extract_block_comments(processed_source, language)
        
        return processed_source, self.comments
    
    def _extract_generator_comments(self, source: str, language: str) -> str:
        """提取生成器注释 (# 注释)"""
        lines = source.split('\n')
        processed_lines = []
        
        for line_num, line in enumerate(lines, 1):
            # 查找生成器注释
            if line.strip().startswith('#'):
                comment_content = line.strip()[1:].strip()
                if comment_content:
                    comment = Comment(
                        type=CommentType.GENERATOR,
                        content=comment_content,
                        line=line_num,
                        column=line.find('#') + 1,
                        language=language
                    )
                    self.comments.append(comment)
                    processed_lines.append('')  # 移除注释行
                else:
                    processed_lines.append(line)
            else:
                processed_lines.append(line)
        
        return '\n'.join(processed_lines)
    
    def _extract_line_comments(self, source: str, language: str) -> str:
        """提取单行注释 (// 注释)"""
        lines = source.split('\n')
        processed_lines = []
        
        for line_num, line in enumerate(lines, 1):
            # 查找单行注释
            if '//' in line:
                comment_start = line.find('//')
                comment_content = line[comment_start + 2:].strip()
                
                if comment_content:
                    comment = Comment(
                        type=CommentType.LINE,
                        content=comment_content,
                        line=line_num,
                        column=comment_start + 1,
                        language=language
                    )
                    self.comments.append(comment)
                    processed_lines.append(line[:comment_start].rstrip())
                else:
                    processed_lines.append(line)
            else:
                processed_lines.append(line)
        
        return '\n'.join(processed_lines)
    
    def _extract_block_comments(self, source: str, language: str) -> str:
        """提取多行注释 (/* 注释 */)"""
        processed_source = source
        comment_pattern = r'/\*([^*]|\*(?!/))*\*/'
        
        # 从后往前处理，避免位置偏移问题
        matches = list(re.finditer(comment_pattern, processed_source, re.DOTALL))
        matches.reverse()
        
        for match in matches:
            comment_content = match.group(0)[2:-2].strip()
            if comment_content:
                # 计算行号和列号
                before_match = processed_source[:match.start()]
                line_num = before_match.count('\n') + 1
                column_num = len(before_match) - before_match.rfind('\n')
                
                comment = Comment(
                    type=CommentType.BLOCK,
                    content=comment_content,
                    line=line_num,
                    column=column_num,
                    language=language
                )
                self.comments.append(comment)
                
                # 移除注释
                processed_source = processed_source[:match.start()] + processed_source[match.end():]
        
        return processed_source
    
    def generate_comments(self, target_language: str, comments: List[Comment] = None) -> str:
        """生成指定语言的注释"""
        if comments is None:
            comments = self.comments
        
        if target_language not in self.language_configs:
            target_language = "html"  # 默认使用HTML注释
        
        config = self.language_configs[target_language]
        generated_comments = []
        
        for comment in comments:
            if not comment.preserved:
                continue
            
            # 根据注释类型选择格式
            if comment.type == CommentType.GENERATOR:
                format_str = config.get("generator", config["line"])
            elif comment.type == CommentType.LINE:
                format_str = config["line"]
            elif comment.type == CommentType.BLOCK:
                format_str = config["block"]
            else:
                format_str = config["line"]
            
            # 生成注释
            generated_comment = format_str.format(comment.content)
            generated_comments.append(generated_comment)
        
        return '\n'.join(generated_comments)
    
    def inject_comments(self, source: str, target_language: str, 
                       position: str = "top", comments: List[Comment] = None) -> str:
        """将注释注入到源代码中"""
        if comments is None:
            comments = self.comments
        
        generated_comments = self.generate_comments(target_language, comments)
        
        if not generated_comments:
            return source
        
        if position == "top":
            return f"{generated_comments}\n{source}"
        elif position == "bottom":
            return f"{source}\n{generated_comments}"
        else:
            return source
    
    def filter_comments(self, comment_types: List[CommentType] = None) -> List[Comment]:
        """过滤注释"""
        if comment_types is None:
            return self.comments
        
        return [comment for comment in self.comments if comment.type in comment_types]
    
    def get_comments_by_language(self, language: str) -> List[Comment]:
        """获取指定语言的注释"""
        return [comment for comment in self.comments if comment.language == language]
    
    def preserve_comment(self, comment_index: int, preserved: bool = True):
        """设置注释是否保留"""
        if 0 <= comment_index < len(self.comments):
            self.comments[comment_index].preserved = preserved
    
    def add_comment(self, content: str, comment_type: CommentType = CommentType.GENERATOR,
                   line: int = 0, column: int = 0, language: str = "auto"):
        """添加注释"""
        comment = Comment(
            type=comment_type,
            content=content,
            line=line,
            column=column,
            language=language
        )
        self.comments.append(comment)

class MultiLanguageCommentGenerator:
    """多语言注释生成器"""
    
    def __init__(self):
        self.processor = CommentProcessor()
        self.language_templates = {
            "html": {
                "header": "<!-- Generated HTML from CHTL -->",
                "footer": "<!-- End of generated HTML -->"
            },
            "css": {
                "header": "/* Generated CSS from CHTL */",
                "footer": "/* End of generated CSS */"
            },
            "js": {
                "header": "// Generated JavaScript from CHTL",
                "footer": "// End of generated JavaScript"
            }
        }
    
    def generate_multi_language_comments(self, source: str, 
                                       target_languages: List[str]) -> Dict[str, str]:
        """生成多语言注释"""
        # 提取注释
        processed_source, comments = self.processor.extract_comments(source)
        
        results = {}
        
        for language in target_languages:
            # 生成该语言的注释
            generated_comments = self.processor.generate_comments(language, comments)
            
            # 添加语言特定的头部和尾部注释
            header = self.language_templates.get(language, {}).get("header", "")
            footer = self.language_templates.get(language, {}).get("footer", "")
            
            if header:
                generated_comments = f"{header}\n{generated_comments}" if generated_comments else header
            if footer:
                generated_comments = f"{generated_comments}\n{footer}" if generated_comments else footer
            
            # 将注释注入到源代码中
            results[language] = self.processor.inject_comments(
                processed_source, language, "top", comments
            )
        
        return results
    
    def generate_documentation_comments(self, source: str, 
                                      documentation_type: str = "api") -> str:
        """生成文档注释"""
        if documentation_type == "api":
            return self._generate_api_documentation(source)
        elif documentation_type == "tutorial":
            return self._generate_tutorial_documentation(source)
        else:
            return source
    
    def _generate_api_documentation(self, source: str) -> str:
        """生成API文档注释"""
        # 这里可以实现API文档生成逻辑
        return source
    
    def _generate_tutorial_documentation(self, source: str) -> str:
        """生成教程文档注释"""
        # 这里可以实现教程文档生成逻辑
        return source

def main():
    """测试注释系统"""
    print("🧪 测试CHTL注释系统")
    print("=" * 50)
    
    # 测试源代码
    test_source = """
    # 这是生成器注释
    [Template] @Style ButtonStyle
    {
        /* 这是CSS注释 */
        background-color: blue;
        color: white; // 这是行内注释
        padding: 10px 20px;
    }
    
    /* 这是多行注释
       可以跨越多行 */
    
    [Template] @Element Card
    {
        <div class="card">
            <h2>Card Title</h2>
            <p>Card content</p>
        </div>
    }
    """
    
    # 创建注释处理器
    processor = CommentProcessor()
    
    # 提取注释
    processed_source, comments = processor.extract_comments(test_source)
    
    print("提取的注释:")
    for i, comment in enumerate(comments):
        print(f"  {i+1}. {comment.type.value}: {comment.content}")
    
    print("\n处理后的源代码:")
    print(processed_source)
    
    # 生成不同语言的注释
    print("\n生成的HTML注释:")
    html_comments = processor.generate_comments("html", comments)
    print(html_comments)
    
    print("\n生成的CSS注释:")
    css_comments = processor.generate_comments("css", comments)
    print(css_comments)
    
    print("\n生成的JavaScript注释:")
    js_comments = processor.generate_comments("js", comments)
    print(js_comments)
    
    # 测试多语言注释生成器
    print("\n多语言注释生成:")
    multi_gen = MultiLanguageCommentGenerator()
    results = multi_gen.generate_multi_language_comments(
        test_source, ["html", "css", "js"]
    )
    
    for language, result in results.items():
        print(f"\n{language.upper()} 结果:")
        print(result)

if __name__ == "__main__":
    main()