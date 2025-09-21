#!/usr/bin/env python3
"""
测试CHTL JS编译器功能
"""

from chtl_js_compiler import CHTLJSLexer, CHTLJSParser, CHTLJSGenerator

def test_script_loader():
    """测试脚本加载器"""
    print("🧪 测试脚本加载器")
    print("=" * 50)
    
    lexer = CHTLJSLexer()
    
    test_code = """
    ScriptLoader {
        load: "./module1.js";
        load: "./module2.js";
    }
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = CHTLJSParser(tokens)
    ast = parser.parse()
    
    print("AST节点:")
    for node in ast:
        print(f"  {node}")
    
    print("代码生成...")
    generator = CHTLJSGenerator(parser.context)
    js = generator.generate(ast)
    
    print("生成的JavaScript:")
    print(js)
    
    # 验证结果
    assert "module1.js" in js
    assert "module2.js" in js
    assert "createElement" in js
    
    print("✅ 脚本加载器测试通过")
    print()

def test_listen():
    """测试事件监听器"""
    print("🧪 测试事件监听器")
    print("=" * 50)
    
    lexer = CHTLJSLexer()
    
    test_code = """
    {{button}}->Listen {
        click: () => {
            console.log('Button clicked!');
        },
        mouseover: () => {
            console.log('Mouse over button');
        }
    }
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = CHTLJSParser(tokens)
    ast = parser.parse()
    
    print("AST节点:")
    for node in ast:
        print(f"  {node}")
    
    print("代码生成...")
    generator = CHTLJSGenerator(parser.context)
    js = generator.generate(ast)
    
    print("生成的JavaScript:")
    print(js)
    
    # 验证结果
    assert "addEventListener" in js
    assert "click" in js
    assert "mouseover" in js
    
    print("✅ 事件监听器测试通过")
    print()

def test_animate():
    """测试动画"""
    print("🧪 测试动画")
    print("=" * 50)
    
    lexer = CHTLJSLexer()
    
    test_code = """
    {{box}}->Animate {
        duration: 2000;
        easing: "ease-in-out";
        begin: {
            opacity: 0;
            transform: "scale(0.5)";
        };
        end: {
            opacity: 1;
            transform: "scale(1)";
        };
        loop: 3;
        direction: "alternate";
        delay: 500;
        callback: "onAnimationComplete";
    }
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = CHTLJSParser(tokens)
    ast = parser.parse()
    
    print("AST节点:")
    for node in ast:
        print(f"  {node}")
    
    print("代码生成...")
    generator = CHTLJSGenerator(parser.context)
    js = generator.generate(ast)
    
    print("生成的JavaScript:")
    print(js)
    
    # 验证结果
    assert "animate" in js
    assert "duration" in js
    assert "easing" in js
    assert "opacity" in js
    
    print("✅ 动画测试通过")
    print()

def test_delegate():
    """测试事件委托"""
    print("🧪 测试事件委托")
    print("=" * 50)
    
    lexer = CHTLJSLexer()
    
    test_code = """
    Delegate {
        parent: "#container";
        target: ["button", "a", "input"];
        events: {
            click: () => {
                console.log('Delegated click');
            };
            change: () => {
                console.log('Delegated change');
            };
        };
    }
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = CHTLJSParser(tokens)
    ast = parser.parse()
    
    print("AST节点:")
    for node in ast:
        print(f"  {node}")
    
    print("代码生成...")
    generator = CHTLJSGenerator(parser.context)
    js = generator.generate(ast)
    
    print("生成的JavaScript:")
    print(js)
    
    # 验证结果
    assert "addEventListener" in js
    assert "container" in js
    assert "button" in js
    
    print("✅ 事件委托测试通过")
    print()

def test_router():
    """测试路由"""
    print("🧪 测试路由")
    print("=" * 50)
    
    lexer = CHTLJSLexer()
    
    test_code = """
    Router {
        url: ["/", "/about", "/contact"];
        page: ["home.html", "about.html", "contact.html"];
        root: "#app";
        mode: "hash";
    }
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = CHTLJSParser(tokens)
    ast = parser.parse()
    
    print("AST节点:")
    for node in ast:
        print(f"  {node}")
    
    print("代码生成...")
    generator = CHTLJSGenerator(parser.context)
    js = generator.generate(ast)
    
    print("生成的JavaScript:")
    print(js)
    
    # 验证结果
    assert "router" in js
    assert "urls" in js
    assert "pages" in js
    assert "hash" in js
    
    print("✅ 路由测试通过")
    print()

def test_virtual_object():
    """测试虚对象"""
    print("🧪 测试虚对象")
    print("=" * 50)
    
    lexer = CHTLJSLexer()
    
    test_code = """
    Vir UserData {
        name: "John Doe";
        age: 30;
        email: "john@example.com";
        preferences: {
            theme: "dark";
            language: "en";
        };
        tags: ["developer", "javascript", "chtl"];
    }
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = CHTLJSParser(tokens)
    ast = parser.parse()
    
    print("AST节点:")
    for node in ast:
        print(f"  {node}")
    
    print("代码生成...")
    generator = CHTLJSGenerator(parser.context)
    js = generator.generate(ast)
    
    print("生成的JavaScript:")
    print(js)
    
    # 验证结果
    assert "UserData" in js
    assert "John Doe" in js
    assert "developer" in js
    
    print("✅ 虚对象测试通过")
    print()

def test_enhanced_selector():
    """测试增强选择器"""
    print("🧪 测试增强选择器")
    print("=" * 50)
    
    lexer = CHTLJSLexer()
    
    test_code = """
    {{.my-class[data-id="123"]}}
    {{#header > .nav}}
    {{button:not(.disabled)}}
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = CHTLJSParser(tokens)
    ast = parser.parse()
    
    print("AST节点:")
    for node in ast:
        print(f"  {node}")
    
    print("代码生成...")
    generator = CHTLJSGenerator(parser.context)
    js = generator.generate(ast)
    
    print("生成的JavaScript:")
    print(js)
    
    # 验证结果
    assert "my-class" in js
    assert "header" in js
    assert "button" in js
    
    print("✅ 增强选择器测试通过")
    print()

def test_responsive_value():
    """测试响应式值"""
    print("🧪 测试响应式值")
    print("=" * 50)
    
    lexer = CHTLJSLexer()
    
    test_code = """
    $screenWidth$
    $userTheme$
    $isMobile$
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = CHTLJSParser(tokens)
    ast = parser.parse()
    
    print("AST节点:")
    for node in ast:
        print(f"  {node}")
    
    print("代码生成...")
    generator = CHTLJSGenerator(parser.context)
    js = generator.generate(ast)
    
    print("生成的JavaScript:")
    print(js)
    
    # 验证结果
    assert "screenWidth" in js
    assert "userTheme" in js
    assert "isMobile" in js
    
    print("✅ 响应式值测试通过")
    print()

def test_mixed_features():
    """测试混合功能"""
    print("🧪 测试混合功能")
    print("=" * 50)
    
    lexer = CHTLJSLexer()
    
    test_code = """
    ScriptLoader {
        load: "./utils.js";
        load: "./components.js";
    }
    
    {{.container}}->Listen {
        click: () => {
            console.log('Container clicked');
        };
    }
    
    {{.fade-in}}->Animate {
        duration: 1000;
        begin: { opacity: 0; };
        end: { opacity: 1; };
    }
    
    Vir AppConfig {
        version: "1.0.0";
        debug: true;
        apiUrl: "https://api.example.com";
    }
    
    $windowWidth$
    $isLoggedIn$
    """
    
    print("词法分析...")
    tokens = lexer.tokenize(test_code)
    
    print("语法分析...")
    parser = CHTLJSParser(tokens)
    ast = parser.parse()
    
    print("AST节点:")
    for node in ast:
        print(f"  {node}")
    
    print("代码生成...")
    generator = CHTLJSGenerator(parser.context)
    js = generator.generate(ast)
    
    print("生成的JavaScript:")
    print(js)
    
    # 验证结果
    assert "utils.js" in js
    assert "addEventListener" in js
    assert "animate" in js
    assert "AppConfig" in js
    assert "windowWidth" in js
    
    print("✅ 混合功能测试通过")
    print()

def main():
    """主测试函数"""
    print("🚀 开始CHTL JS编译器功能测试")
    print("=" * 60)
    
    try:
        test_script_loader()
        test_listen()
        test_animate()
        test_delegate()
        test_router()
        test_virtual_object()
        test_enhanced_selector()
        test_responsive_value()
        test_mixed_features()
        
        print("🎉 所有CHTL JS编译器功能测试通过！")
        print("=" * 60)
        
    except Exception as e:
        print(f"❌ 测试失败: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()