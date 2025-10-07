# CHTL 模块开发指南

## 概述

本指南将帮助你创建自己的CHTL模块（CMOD）和CHTL JS扩展模块（CJMOD）。

---

## CMOD开发

### 1. 模块结构

CMOD模块必须遵循以下目录结构：

```
ModuleName/
├── src/
│   ├── ModuleName.chtl    # 主模块文件（可选，如果有子模块）
│   ├── Component1.chtl     # 其他模块文件
│   └── SubModule/          # 子模块（可选）
│       ├── src/
│       │   └── SubModule.chtl
│       └── info/
│           └── SubModule.chtl
└── info/
    └── ModuleName.chtl     # 模块信息文件（必须）
```

### 2. 模块信息文件

`info/ModuleName.chtl` 必须包含 `[Info]` 块：

```chtl
[Info]
{
    name = "模块名称";
    version = "1.0.0";
    description = "模块描述";
    author = "作者名称";
    license = "MIT";
    dependencies = "";  // 逗号分隔的依赖列表
    category = "theme"; // 模块类别
    minCHTLVersion = "2.5.0";
    maxCHTLVersion = "3.0.0";
}
```

### 3. 导出表

可选的 `[Export]` 块定义向外暴露的内容：

```chtl
[Export]
{
    // 自定义样式组
    [Custom] @Style StyleName1, StyleName2;
    
    // 自定义元素
    [Custom] @Element ElementName1, ElementName2;
    
    // 变量组
    [Custom] @Var VarGroupName1, VarGroupName2;
    
    // 模板
    [Template] @Style TemplateName;
    [Template] @Element TemplateElementName;
}
```

### 4. 主模块文件示例

```chtl
[Namespace] ModuleName

// 定义变量组
[Custom] @Var ThemeColors
{
    primary: #FF6B9D;
    secondary: #FFD700;
    background: #FFFFFF;
}

// 定义样式组
[Custom] @Style Card
{
    background: ThemeColors(background);
    border-radius: 12px;
    padding: 20px;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
}

// 定义组件
[Custom] @Element MyComponent
{
    div
    {
        class: my-component;
        
        style
        {
            @Style Card;
            
            .my-component
            {
                width: 100%;
            }
        }
        
        div
        {
            class: my-component-header;
            text: "Component Header";
        }
        
        div
        {
            class: my-component-body;
            text: "Component Body";
        }
    }
}
```

### 5. 使用命名空间

推荐使用命名空间避免命名冲突：

```chtl
[Namespace] MyModule
{
    [Custom] @Style MyStyle
    {
        color: red;
    }
}
```

使用时：

```chtl
[Import] @Chtl from MyModule

div {
    style {
        @Style MyStyle from MyModule;
    }
}
```

---

## CJMOD开发

### 1. 模块结构

CJMOD模块结构：

```
CJModuleName/
├── src/
│   ├── function1.cpp       # C++实现文件
│   ├── function1.h         # 头文件（可选）
│   └── SubModule/          # 子模块（可选）
│       ├── src/
│       │   └── sub.cpp
│       └── info/
│           └── SubModule.chtl
└── info/
    └── CJModuleName.chtl   # 模块信息文件
```

### 2. CJMOD信息文件

```chtl
[Info]
{
    name = "模块名称-cjmod";
    version = "1.0.0";
    description = "CHTL JS扩展描述";
    author = "作者名称";
    license = "MIT";
    dependencies = "";
    category = "chtljs-extension";
    minCHTLVersion = "2.5.0";
    maxCHTLVersion = "3.0.0";
}
```

### 3. CJMOD API使用

基本示例：

```cpp
#include "CJMODApi.h"

using namespace CHTL::CJMOD;

extern "C" {

// 初始化函数
void InitMyFunction() {
    // 绑定为CHTL JS函数
    CHTLJSFunction::bindVirtualObject("MyFunction");
}

// 处理函数调用
std::string ProcessMyFunction(const std::string& code) {
    // 创建模式
    Arg pattern = Syntax::analyze("MyFunction { param1: $!_, param2: $?_ }");
    
    // 绑定参数处理
    pattern.bind("param1", [](const std::string& value) {
        return value;  // 必需参数
    });
    
    pattern.bind("param2", [](const std::string& value) {
        if (value.empty()) return "\"default\"";  // 可选参数默认值
        return value;
    });
    
    // 扫描代码
    CJMODScanner::setSourceCode(code);
    Arg scanned = CJMODScanner::scan(pattern, "MyFunction");
    
    // 填充值
    pattern.fillValue(scanned);
    
    // 生成JS代码
    std::ostringstream output;
    output << "(function() {\n";
    output << "    // 你的JS代码\n";
    output << "    const param1 = " << pattern[0].value << ";\n";
    output << "    const param2 = " << pattern[1].value << ";\n";
    output << "    // ...\n";
    output << "})()";
    
    pattern.transform(output.str());
    CJMODGenerator::exportResult(pattern);
    
    return pattern.getResult();
}

} // extern "C"
```

### 4. 占位符类型

- `$` - 普通占位符
- `$?` - 可选占位符
- `$!` - 必需占位符
- `$_` - 无序占位符
- `...` - 不定参数

---

## 模块打包

### 打包为.cmod文件

使用CHTL编译器或打包脚本：

```bash
# 使用编译器
chtl pack MyModule/ -o MyModule.cmod

# 使用Python脚本
python pack_module.py MyModule/ MyModule.cmod
```

### 打包为.cjmod文件

```bash
# 使用编译器
chtl pack-cjmod MyCJModule/ -o MyCJModule.cjmod

# 使用Python脚本
python pack_cjmod.py MyCJModule/ MyCJModule.cjmod
```

---

## 模块分发

### 1. 目录结构（开发中）

在开发时，将模块放置在：

```
project/
├── module/              # 用户模块目录
│   ├── MyModule/
│   └── AnotherModule/
└── my-app.chtl
```

### 2. 打包文件分发

分发打包后的 `.cmod` 或 `.cjmod` 文件：

```
官方模块目录: /path/to/chtl/module/
用户模块目录: ./module/

支持的结构：
- 无序: .cmod, .chtl, .cjmod混杂
- 有序: CMOD/和CJMOD/文件夹分类
```

---

## 最佳实践

### 1. 命名规范
- 使用PascalCase命名模块和组件
- 使用描述性名称
- 避免过于通用的名称

### 2. 文档
- 在`README.md`中说明模块用途
- 提供使用示例
- 列出所有导出的组件

### 3. 版本管理
- 遵循语义化版本（SemVer）
- 在更新时更新版本号
- 记录变更历史

### 4. 依赖管理
- 尽量减少依赖
- 明确声明依赖
- 测试依赖兼容性

### 5. 测试
- 创建测试用例
- 测试所有导出的组件
- 测试跨平台兼容性

---

## 示例模块

参考官方模块获取灵感：

1. **Chtholly（珂朵莉）模块**
   - 位置: `Module/Chtholly/`
   - 类型: CMOD + CJMOD
   - 包含: UI组件 + CHTL JS扩展

2. **Yuigahama（由比滨结衣）模块**
   - 位置: `Module/Yuigahama/`
   - 类型: CMOD
   - 包含: UI组件主题

---

## 常见问题

### Q: 模块找不到？
A: 检查模块路径配置：
- 官方模块: `./module`
- 用户模块: `./module` 或 `.chtlrc` 中配置的路径

### Q: 导出不生效？
A: 确保：
- `[Export]` 块格式正确
- 导出的名称与实际定义匹配
- 使用正确的导入语法

### Q: CJMOD编译错误？
A: 检查：
- C++17标准支持
- 包含正确的头文件
- API使用是否正确

---

## 资源链接

- [CHTL.md](CHTL.md) - 完整语言规范
- [官方模块源码](Module/) - 学习示例
- [.chtlrc](.chtlrc) - 配置文件示例

---

**Happy Module Development!** 🚀

创建美丽的CHTL模块，与社区分享你的作品！
