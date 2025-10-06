# CHTL v0.7.0 Quick Start Guide

## 🚀 快速开始（5分钟上手）

### 1. 构建项目
```bash
cd /workspace
python3 build.py all
```

### 2. 编译第一个CHTL文件
```bash
# 使用simple示例
./build/chtl examples/simple.chtl -o my_first.html

# 查看结果
cat my_first.html
```

### 3. 尝试表达式功能
创建`test.chtl`:
```chtl
div {
    style {
        width: 100px + 50px;
        height: 200px / 2;
        background: 100 > 50 ? red : blue;
    }
}
```

编译：
```bash
./build/chtl test.chtl -o test.html
```

### 4. 尝试属性引用
创建`reference.chtl`:
```chtl
div {
    id: box;
    style {
        width: 100px;
    }
}

div {
    style {
        width: box.width * 2;
    }
}
```

编译：
```bash
./build/chtl reference.chtl -o reference.html
```

---

## ⚡ 核心功能速查

### CSS算术运算
```chtl
style {
    width: 100px + 50px;        // → 150px
    height: 200px - 50px;       // → 150px
    margin: 10px * 2;           // → 20px
    padding: 100px / 4;         // → 25px
    z-index: 2 ** 3;            // → 8
}
```

### 条件表达式
```chtl
style {
    display: isVisible ? block : none;
    background: score > 80 ? green : red;
    width: 100 > 50 ? 200px : 100px;
}
```

### 属性引用
```chtl
div {
    id: base;
    style { width: 100px; }
}

div {
    style {
        width: base.width + 20px;          // 120px
        height: base.width * 2;            // 200px
        margin: (base.width + 50px) / 2;   // 75px
    }
}
```

### 增强选择器
```chtl
script {
    {{.box}}->addEventListener('click', fn);
    {{#myId}}->disabled = true;
    {{&}}->classList.add('active');
}
```

---

## 📚 更多示例

查看`examples/`目录：
- `simple.chtl` - 基础示例
- `advanced.chtl` - 高级特性
- `comprehensive_test.chtl` - 所有功能展示

---

## 🐛 遇到问题？

### 编译失败
```bash
# 清理并重新构建
python3 build.py clean
python3 build.py all
```

### 运行测试
```bash
python3 build.py test
# 或
./build/chtl_tests
```

### 查看详细信息
```bash
# 查看tokens
./build/chtl your_file.chtl --tokens

# 查看AST
./build/chtl your_file.chtl --ast
```

---

## 📖 学习路径

1. **第1天**: 基础语法（元素、属性、文本）
2. **第2天**: 样式块（CSS选择器、伪类）
3. **第3天**: 表达式（算术运算、条件）
4. **第4天**: 属性引用（组件化开发）
5. **第5天**: CHTL JS（增强选择器、脚本）

---

## 🎯 下一版本

**v0.75.0-alpha** (即将推出):
- 模板系统
- 代码复用能力

敬请期待！

---

Happy Coding! 🎨✨
