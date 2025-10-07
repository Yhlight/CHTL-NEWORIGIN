# CHTL v2.5.0 模块使用指南

**版本**: v2.5.0-final  
**日期**: 2025-10-06

---

## 📚 目录

1. [模块系统概述](#模块系统概述)
2. [官方模块](#官方模块)
3. [使用模块](#使用模块)
4. [创建自己的模块](#创建自己的模块)
5. [CJMOD 扩展开发](#cjmod-扩展开发)
6. [最佳实践](#最佳实践)

---

## 模块系统概述

CHTL v2.5.0 引入了完整的模块系统，包括：

### CMOD - CHTL 模块
- 用于打包和分发 CHTL 组件
- 包含样式组、元素、变量组等
- 支持子模块和命名空间
- 使用 libzip 压缩为 .cmod 文件

### CJMOD - CHTL JS 扩展模块
- 用于扩展 CHTL JS 语法
- 基于强大的 CJMOD API
- 支持虚对象（Vir）
- C++ 实现，无缝集成

---

## 官方模块

CHTL v2.5.0 提供了两个精心设计的官方模块：

### 🌸 珂朵莉（Chtholly）模块

**主题**: 粉红色系，温暖可爱  
**类型**: CMOD + CJMOD 混合模块

#### CMOD 组件

**变量组**:
- `ChthollyColors` - 粉红色系变量
- `ChthollyFonts` - 字体变量
- `ChthollySpacing` - 间距变量
- `ChthollyAnimations` - 动画变量

**样式组**:
- `ChthollyTheme` - 主题样式
- `ChthollyCard` - 卡片组件
- `ChthollyButton` - 按钮组件
- `ChthollyAccordion` - 手风琴
- `ChthollyGallery` - 四叶窗相册
- `ChthollyMemo` - 备忘录
- `ChthollyNote` - 暖色笔记
- `ChthollyProgress` - 进度条
- `ChthollySakura` - 樱花雨
- `ChthollyMouse` - 鼠标特效
- `ChthollyParallax` - 视差滚动
- `ChthollyContextMenu` - 右键菜单

**元素**:
- `ChthollyPage` - 页面布局
- `ChthollyAccordionItem` - 手风琴项
- `ChthollyGalleryItem` - 相册项
- `ChthollyProgressBar` - 进度条组件

#### CJMOD 组件

- **PrintMylove** - 图片转字符画
  - 支持 ASCII 和 Pixel 两种模式
  - 支持尺寸和缩放控制
  
- **INeverAway** - 标记函数系统
  - 支持状态区分的同名重载
  - 虚对象（Vir）支持
  
- **util...end** - 条件监视表达式
  - 监视条件变化
  - 条件满足时触发

### 🌟 由比滨结衣（Yuigahama）模块

**主题**: 橙色系，活力阳光  
**类型**: CMOD 模块

#### CMOD 组件

**变量组**:
- `YuigahamaColors` - 橙色系变量
- `YuigahamaFonts` - 字体变量（含 Quicksand）
- `YuigahamaSpacing` - 间距变量
- `YuigahamaAnimations` - 动画变量（含弹跳效果）

**样式组**:
- `YuigahamaTheme` - 主题样式
- `YuigahamaCard` - 卡片组件
- `YuigahamaButton` - 圆润按钮
- `YuigahaMusicPlayer` - 音乐播放器样式
- `YuigahamaAccordion` - 手风琴
- `YuigahamaGallery` - 四叶窗相册（3D效果）
- `YuigahamaMemo` - 备忘录
- `YuigahamaNote` - 笔记
- `YuigahamaSakura` - 樱花雨
- `YuigahamaMouse` - 鼠标特效

**元素**:
- `YuigahamaPage` - 页面布局
- `YuigahaMusicPlayerWidget` - 完整音乐播放器
- `YuigahamaGalleryItem` - 3D相册项

---

## 使用模块

### 基础用法

#### 1. 导入模块

```chtl
[Import] @Chtl from chtl::Chtholly
```

#### 2. 使用样式组

```chtl
div {
    style {
        @Style ChthollyCard;
    }
}
```

#### 3. 使用元素

```chtl
body {
    @Element ChthollyPage;
}
```

#### 4. 使用变量组

```chtl
div {
    style {
        color: ChthollyColors(primary);
        padding: ChthollySpacing(lg);
    }
}
```

### 完整示例

```chtl
use html5;

[Import] @Chtl from chtl::Chtholly

html {
    head {
        title { text: "我的珂朵莉应用"; }
    }
    
    body {
        // 使用页面布局
        @Element ChthollyPage;
        
        // 标题卡片
        div {
            style { @Style ChthollyCard; }
            
            h1 {
                text: "欢迎来到珂朵莉的世界";
                style {
                    color: ChthollyColors(primaryDark);
                    font-size: ChthollyFonts(size2xl);
                }
            }
            
            p {
                text: "这是一个使用珂朵莉模块的示例页面。";
            }
            
            // 交互按钮
            button {
                text: "点击我";
                style { @Style ChthollyButton; }
                
                script {
                    {{&}} &-> click: () => {
                        alert('你好，珂朵莉！');
                    };
                }
            }
        }
        
        // 进度条
        h2 { text: "进度条示例"; }
        @Element ChthollyProgressBar;
        
        // 手风琴
        h2 { text: "手风琴示例"; }
        @Element ChthollyAccordionItem {
            div {
                class: accordion-header;
                text: "点击展开";
            }
            div {
                class: accordion-content;
                p { text: "这是内容"; }
            }
        }
    }
}
```

### 使用两个模块

```chtl
use html5;

[Import] @Chtl from chtl::Chtholly
[Import] @Chtl from chtl::Yuigahama

body {
    // 珂朵莉主题
    style { @Style ChthollyTheme; }
    
    // 珂朵莉卡片
    div {
        style { @Style ChthollyCard; }
        text: "珂朵莉的粉红世界";
    }
    
    // 由比滨卡片
    div {
        style { @Style YuigahamaCard; }
        text: "由比滨的橙色世界";
    }
    
    // 由比滨音乐播放器
    @Element YuigahaMusicPlayerWidget;
}
```

### 使用 CJMOD 扩展

#### PrintMylove - 图片转字符画

```chtl
script {
    // ASCII 模式
    const asciiArt = PrintMylove {
        url: "photo.jpg",
        mode: "ASCII",
        scale: 0.5
    };
    console.log(asciiArt);
    
    // Pixel 模式（彩色）
    const pixelArt = PrintMylove {
        url: "avatar.png",
        mode: "Pixel",
        width: "200px",
        height: "200px"
    };
    console.log(pixelArt);
}
```

#### INeverAway - 标记函数

```chtl
script {
    // 定义标记函数
    Vir MyFuncs = INeverAway {
        // 状态 A 的 log 函数
        log<A>: function(msg) {
            console.log('[A] ' + msg);
        },
        
        // 状态 B 的 log 函数
        log<B>: function(msg) {
            console.error('[B] ' + msg);
        },
        
        // 普通函数
        info: function(msg) {
            console.info(msg);
        }
    };
    
    // 使用
    MyFuncs->log<A>('消息A');
    MyFuncs->log<B>('消息B');
    MyFuncs->info('普通消息');
}
```

#### util...end - 条件监视

```chtl
script {
    let count = 0;
    
    // 监视 count 变化
    util count > 5 -> change {
        console.log('count 发生变化：' + count);
    } -> end {
        console.log('count 达到目标！');
    };
    
    // 模拟变化
    setInterval(() => {
        count++;
    }, 1000);
}
```

---

## 创建自己的模块

### CMOD 模块结构

```
MyModule/
├── src/
│   ├── MyModule.chtl        (主模块文件)
│   └── Components.chtl      (其他文件)
└── info/
    └── MyModule.chtl        (模块信息)
```

### 模块信息文件

创建 `info/MyModule.chtl`:

```chtl
[Info]
{
    name = "my-module";
    version = "1.0.0";
    description = "我的自定义模块";
    author = "Your Name";
    license = "MIT";
    dependencies = "";
    category = "custom";
    minCHTLVersion = "2.5.0";
    maxCHTLVersion = "3.0.0";
}

[Export]
{
    [Custom] @Style MyStyle1, MyStyle2;
    [Custom] @Element MyElement1;
    [Custom] @Var MyColors;
}
```

### 主模块文件

创建 `src/MyModule.chtl`:

```chtl
[Namespace] MyModule

[Custom] @Var MyColors
{
    primary: #007bff;
    secondary: #6c757d;
}

[Custom] @Style MyStyle1
{
    background: MyColors(primary);
    color: white;
    padding: 16px;
    border-radius: 8px;
}

[Custom] @Element MyElement1
{
    div {
        style { @Style MyStyle1; }
        text: "我的自定义元素";
    }
}
```

### 使用自己的模块

```chtl
[Import] @Chtl from ./MyModule

body {
    @Element MyElement1;
}
```

---

## CJMOD 扩展开发

### CJMOD 模块结构

```
MyCJMOD/
├── src/
│   ├── Feature1.cpp
│   ├── Feature1.h
│   └── Feature2.cpp
└── info/
    └── MyCJMOD.chtl
```

### CJMOD API 示例

创建 `src/PowerOperator.cpp`:

```cpp
#include "CJMODApi.h"

using namespace CHTL::CJMOD;

// 实现 ** 幂运算符
Arg processPowerOperator(const String& code) {
    // 1. 定义语法模式
    Arg pattern = Syntax::analyze("$ ** $");
    
    // 2. 绑定参数处理函数
    pattern.bind("$", [](const String& value) {
        return value;  // 直接返回值
    });
    
    // 3. 扫描代码提取参数
    Arg result = CJMODScanner::scan(pattern, "**", code);
    
    // 4. 填充参数值
    pattern.fillValue(result);
    
    // 5. 转换为 JavaScript
    String jsCode = "Math.pow(" + 
                    pattern[0].getValue() + ", " + 
                    pattern[2].getValue() + ")";
    pattern.transform(jsCode);
    
    // 6. 导出结果
    return pattern;
}

// 注册语法
void registerPowerOperator() {
    CJMODRegistry::SyntaxInfo info;
    info.pattern = "$ ** $";
    info.description = "幂运算符";
    info.examples = {"3 ** 4  // Math.pow(3, 4)"};
    
    CJMODRegistry::registerSyntax("**", info);
}
```

### CJMOD 信息文件

创建 `info/MyCJMOD.chtl`:

```chtl
[Info]
{
    name = "my-cjmod";
    version = "1.0.0";
    description = "我的CHTL JS扩展模块";
    author = "Your Name";
    license = "MIT";
    dependencies = "";
    category = "chtl-js-extension";
    minCHTLVersion = "2.5.0";
    maxCHTLVersion = "3.0.0";
}
```

---

## 最佳实践

### 1. 模块命名

- CMOD: 使用有意义的名称，如 `MyTheme`, `UIComponents`
- CJMOD: 使用描述性名称，如 `MathExtensions`, `DOMHelpers`

### 2. 版本控制

遵循语义化版本：
- 主版本：不兼容的 API 修改
- 次版本：向下兼容的功能性新增
- 修订号：向下兼容的问题修正

### 3. 依赖管理

```chtl
[Info]
{
    dependencies = "module1@^1.0.0, module2@~2.1.0";
}
```

### 4. 命名空间

始终使用命名空间避免冲突：

```chtl
[Namespace] MyModule

[Custom] @Style MyStyle { ... }
```

### 5. 导出管理

只导出公共 API：

```chtl
[Export]
{
    // 导出公共样式
    [Custom] @Style PublicStyle1, PublicStyle2;
    
    // 不导出内部样式（不在Export中列出）
}
```

### 6. 文档

为每个导出项添加注释：

```chtl
// ChthollyCard - 优雅的卡片组件
// 特点：12px圆角，柔和阴影，粉红边框
[Custom] @Style ChthollyCard
{
    ...
}
```

---

## 配置模块路径

编辑 `.chtlrc`:

```ini
# 官方模块路径
chtl.module.official.path=./module

# 用户模块路径
chtl.module.user.path=./my-modules

# 模块搜索路径（多个路径用分号分隔）
chtl.module.search.paths=./module;./my-modules;./node_modules

# 启用模块缓存
chtl.module.cache.enabled=true

# 缓存目录
chtl.module.cache.path=./.chtl_cache/modules
```

---

## 示例：完整的模块化应用

```chtl
use html5;

// 导入两个官方模块
[Import] @Chtl from chtl::Chtholly
[Import] @Chtl from chtl::Yuigahama

html {
    head {
        title { text: "我的模块化应用"; }
    }
    
    body {
        // 使用珂朵莉主题
        style { @Style ChthollyTheme; }
        
        // 页头（珂朵莉风格）
        div {
            style {
                @Style ChthollyCard;
                text-align: center;
                margin-bottom: ChthollySpacing(xl);
            }
            
            h1 {
                text: "欢迎使用 CHTL v2.5.0";
                style {
                    color: ChthollyColors(primaryDark);
                }
            }
        }
        
        // 内容区域（由比滨风格）
        div {
            style {
                @Style YuigahamaCard;
                margin-bottom: YuigahamaSpacing(lg);
            }
            
            h2 {
                text: "模块化开发";
                style {
                    color: YuigahamaColors(primary);
                }
            }
            
            p {
                text: "使用官方模块快速构建美观的界面！";
            }
        }
        
        // 音乐播放器（由比滨特色）
        @Element YuigahaMusicPlayerWidget {
            script {
                // 初始化播放器数据
                {{.song-title}}->textContent = "Scarborough Fair";
                {{.song-artist}}->textContent = "莎拉·布莱曼";
            }
        }
        
        // CJMOD 扩展使用
        script {
            // PrintMylove
            const art = PrintMylove {
                url: "logo.png",
                mode: "Pixel",
                scale: 0.3
            };
            console.log(art);
            
            // INeverAway
            Vir Logger = INeverAway {
                info<console>: function(msg) {
                    console.log('[INFO] ' + msg);
                },
                
                warn<console>: function(msg) {
                    console.warn('[WARN] ' + msg);
                }
            };
            
            Logger->info<console>('应用已启动');
            
            // util...end
            let loadProgress = 0;
            util loadProgress >= 100 -> change {
                console.log('加载中: ' + loadProgress + '%');
            } -> end {
                console.log('加载完成！');
            };
            
            // 模拟加载
            setInterval(() => {
                if (loadProgress < 100) {
                    loadProgress += 10;
                }
            }, 500);
        }
    }
}
```

---

## 高级用法

### 子模块导入

```chtl
// 导入特定子模块
[Import] @Chtl from chtl::Chtholly.SubModule

// 导入所有子模块
[Import] @Chtl from chtl::Chtholly.*
```

### 精确导入

```chtl
// 只导入特定样式组
[Import] [Custom] @Style ChthollyCard from chtl::Chtholly

// 使用别名
[Import] [Custom] @Style ChthollyCard from chtl::Chtholly as PinkCard
```

### 混合模块使用

```chtl
// 导入 CMOD 部分
[Import] @Chtl from chtl::Chtholly

// 导入 CJMOD 部分（如果分离打包）
[Import] @CJmod from chtl::Chtholly-JS
```

---

## 常见问题

### Q: 如何知道模块有哪些组件？

A: 查看模块的 `info/*.chtl` 文件中的 `[Export]` 块。

### Q: 两个模块的样式冲突怎么办？

A: 使用命名空间的 `from` 语法：

```chtl
div {
    style {
        @Style ChthollyCard from Chtholly;
    }
}
```

### Q: 如何更新模块？

A: 重新下载模块并覆盖到模块目录，或使用未来版本的模块管理器。

### Q: 可以创建私有模块吗？

A: 可以！将模块放在用户模块目录（默认 `./module`）即可。

---

## 模块开发工具

### 打包模块

```cpp
#include "CMODSystem/ModulePacker.h"

ModulePacker packer;
packer.packModule("./MyModule", "./MyModule.cmod");
```

### 验证模块结构

```cpp
ModulePacker packer;
bool valid = packer.validateModuleStructure("./MyModule");
```

### 生成 JSON 清单

```cpp
ModulePacker packer;
String manifest = packer.generateJSONManifest(moduleData);
```

---

## 配置选项

在 `.chtlrc` 中配置模块行为：

```ini
# 模块设置
chtl.module.official.path=./module
chtl.module.user.path=./my-modules
chtl.module.cache.enabled=true
chtl.module.cache.path=./.chtl_cache/modules
chtl.module.search.paths=./module;./my-modules;./node_modules

# 官方模块开关
chtl.module.chtholly.enabled=true
chtl.module.chtholly.theme=default

chtl.module.yuigahama.enabled=true
chtl.module.yuigahama.theme=default
```

---

## 未来功能

### v2.6.0 计划
- 模块版本管理器
- 自动依赖安装
- 模块更新检查

### v3.0.0 展望
- 模块市场/仓库
- 在线模块浏览器
- VSCode 模块管理扩展
- 可视化模块编辑器

---

## 技术支持

- **文档**: https://docs.chtl.dev/modules
- **示例**: https://github.com/chtl-lang/chtl/tree/main/examples
- **Issues**: https://github.com/chtl-lang/chtl/issues

---

**CHTL - 模块化让开发更优雅！** 🌟

**珂朵莉**: "世界上最幸福的女孩" 🌸  
**由比滨结衣**: "温柔可爱的现充" 🌟
