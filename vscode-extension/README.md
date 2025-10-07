# CHTL Language Support for VS Code

CHTL (超文本模板语言) 的 Visual Studio Code 扩展。

## 功能特性

### v0.1.0 (基础版)

- ✅ **语法高亮**
  - CHTL核心语法
  - HTML元素
  - CSS样式块
  - JavaScript脚本块
  - 注释（//、/**/、#）
  - 模板和自定义语法
  - 条件渲染（if/else if/else）
  - CHTL JS语法（{{}}, Listen, Delegate等）

- ✅ **括号匹配**
  - 自动关闭括号
  - 括号高亮

- ✅ **代码折叠**
  - 支持region标记

## 安装方法

### 方法1：从源码安装

1. 复制`vscode-extension`目录到VSCode扩展目录：

**Windows**:
```bash
cp -r vscode-extension ~/.vscode/extensions/chtl-language-support
```

**macOS/Linux**:
```bash
cp -r vscode-extension ~/.vscode/extensions/chtl-language-support
```

2. 重启VSCode

### 方法2：开发模式

1. 在VSCode中打开`vscode-extension`目录
2. 按F5启动扩展开发宿主
3. 在新窗口中打开.chtl文件

## 使用方法

1. 创建`.chtl`文件
2. 开始编写CHTL代码
3. 享受语法高亮！

## 示例

```chtl
// CHTL示例
html
{
    body
    {
        div
        {
            class: container;
            
            style
            {
                width: 100%;
                max-width: 1200px;
            }
            
            if
            {
                condition: {{html}}->width < 768px,
                display: block,
            }
            
            script
            {
                {{.container}}->Listen {
                    click: () => {
                        console.log('Clicked!');
                    }
                };
            }
            
            text
            {
                Hello, CHTL!
            }
        }
    }
}
```

## 路线图

### v0.1.0 (当前)
- ✅ 基础语法高亮
- ✅ 括号匹配
- ✅ 代码折叠

### v0.2.0 (计划中)
- 代码片段
- 代码提示
- 符号导航

### v0.3.0 (计划中)
- 实时预览
- 错误检测
- 格式化支持

### v1.0.0 (长期)
- 完整的IntelliSense
- 调试支持
- 集成编译器

## 贡献

欢迎贡献！请查看主项目的CONTRIBUTING.md。

## 许可证

MIT License

## 链接

- [CHTL 规范](../CHTL.md)
- [CHTL 项目](https://github.com/chtl-lang/chtl)
