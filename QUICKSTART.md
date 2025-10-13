# CHTL 快速开始指南

## 🚀 5分钟上手CHTL

### 1. 构建项目

#### Linux/macOS
```bash
./build.sh
```

#### Windows
```cmd
build.bat
```

### 2. 创建你的第一个CHTL文件

创建 `hello.chtl`:

```chtl
html {
    head {
        text { 我的第一个CHTL页面 }
    }
    
    body {
        div {
            id: greeting;
            class: welcome;
            
            style {
                font-size: 24px;
                color: #007bff;
                text-align: center;
                padding: 50px;
            }
            
            text { 你好，CHTL！ }
        }
    }
}
```

### 3. 编译

```bash
./build/chtl hello.chtl
```

将生成 `output.html`！

### 4. 高级用法

#### 生成内联HTML（所有CSS/JS都在一个文件中）
```bash
./build/chtl --inline hello.chtl
```

#### 生成带完整HTML结构的文件
```bash
./build/chtl --default-struct hello.chtl
```

#### 自定义输出
```bash
./build/chtl -o dist --output index hello.chtl
```
生成 `dist/index.html`

## 📝 CHTL语法速查

### 基本元素
```chtl
div {
    text { 这是文本 }
}
```

### 属性
```chtl
div {
    id: myDiv;           // 使用:
    class = container;   // 或使用=（CE对等式）
}
```

### 无修饰字面量
```chtl
div {
    class: container;    // 不需要引号
    data-value: 100;     // 数字也不需要引号
}
```

### 局部样式
```chtl
div {
    style {
        width: 100px;
        height: 200px;
        background-color: red;
    }
}
```

### 嵌套元素
```chtl
html {
    body {
        div {
            span {
                text { 嵌套内容 }
            }
        }
    }
}
```

### 注释
```chtl
// 这是注释，不会出现在HTML中

/* 
   多行注释
   也不会出现在HTML中
*/

# 生成器注释，会出现在HTML中
```

## 🎯 示例项目

查看 `examples/example.chtl` 获取更多示例。

编译示例：
```bash
./build/chtl examples/example.chtl
```

## 🧪 运行测试

```bash
cd build
./chtl_test
```

## 📚 更多信息

- 完整文档：[README_ZH.md](README_ZH.md)
- 项目总结：[PROJECT_SUMMARY.md](PROJECT_SUMMARY.md)
- 完整规格：[CHTL.md](CHTL.md)

## ❓ 常见问题

### Q: 编译失败怎么办？
A: 确保使用C++20兼容的编译器（GCC 10+, Clang 10+, MSVC 2019+）

### Q: 如何查看帮助？
A: 运行 `./build/chtl --help`

### Q: 生成的HTML在哪里？
A: 默认在当前目录的 `output.html`

### Q: 如何查看调试信息？
A: 使用 `--debug` 选项：`./build/chtl --debug hello.chtl`

## 🎉 下一步

1. 尝试修改 `examples/example.chtl`
2. 创建自己的CHTL项目
3. 阅读完整文档了解高级特性
4. 贡献代码或报告问题

Happy coding with CHTL! 🚀
