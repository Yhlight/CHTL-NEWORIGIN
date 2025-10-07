# CHTL 语言支持说明

**版本**: v2.5.0-final  
**日期**: 2025-10-06

---

## 📚 三门语言共存

CHTL v2.5.0 完美支持三门语言在项目中和谐共存：

### 1. CHTL - 核心模板语言
- 用于：HTML 结构和 CSS 样式
- 特点：声明式、简洁、强大

### 2. CHTL JS - JavaScript 扩展语言
- 用于：DOM 操作和事件处理的增强语法
- 特点：声明式、类型安全、易用

### 3. JavaScript - 标准 JavaScript
- 用于：复杂逻辑、数据处理、API 调用
- 特点：完整的 ES6+ 支持

---

## 🎯 语言职权划分

根据 CHTL.md 规范（1278-1290 行）：

### CHTL 职权
- ✅ HTML 元素和结构
- ✅ CSS 样式（局部样式块）
- ✅ 模板系统
- ✅ 自定义元素
- ✅ 命名空间
- ✅ 导入系统
- ❌ **不处理** JS 动态特征

### CHTL JS 职权
- ✅ 增强选择器 `{{...}}`
- ✅ 事件处理（Listen, &->）
- ✅ 事件委托（Delegate）
- ✅ 动画（Animate）
- ✅ 路由（Router）
- ✅ 虚对象（Vir）
- ✅ 响应式值 `$var$`
- ❌ **不处理** 纯 JS 逻辑

### JavaScript 职权
- ✅ 变量和函数定义
- ✅ 类和对象
- ✅ 数组和数据操作
- ✅ Promise 和异步
- ✅ API 调用
- ✅ 复杂业务逻辑

---

## 📝 script 块中的语言使用

根据 CHTL.md 规范（2099 行）：

> 脚本（script）→ 由CHTL编译器、CHTL JS编译器及JS编译器共同管理

### script 块支持三种写法

#### 1. 纯 JavaScript

```chtl
script {
    // 标准 JS 代码
    const x = 10;
    const y = 20;
    
    function calculate() {
        return x + y;
    }
    
    class MyClass {
        constructor() {
            this.value = 42;
        }
    }
    
    // ES6+ 特性
    const arr = [1, 2, 3];
    const doubled = arr.map(x => x * 2);
    
    // Promise
    fetch('/api/data')
        .then(res => res.json())
        .then(data => console.log(data));
}
```

**处理方式**：
- ✅ 直接输出，不做任何转换
- ✅ 保持原有 JS 语法
- ✅ 支持所有 ES6+ 特性

#### 2. 纯 CHTL JS

```chtl
script {
    // CHTL JS 增强选择器
    {{#box}}->textContent = 'Hello';
    
    // CHTL JS 事件绑定
    {{.button}} &-> click: () => {
        alert('Clicked!');
    };
    
    // CHTL JS Listen 块
    {{.slider}}->Listen {
        input: (e) => {
            console.log(e.target.value);
        }
    };
    
    // CHTL JS Delegate
    {{.container}}->Delegate {
        target: {{.item}},
        click: handleClick
    };
}
```

**处理方式**：
- ✅ CHTL JS 编译器处理
- ✅ 转换为标准 JS
- ✅ `{{}}` → `querySelector()`
- ✅ `&->` → `addEventListener()`
- ✅ `->` → `.`

#### 3. **混合使用（推荐）**

```chtl
script {
    // 1. 纯 JS - 定义数据和逻辑
    let count = 0;
    let isActive = false;
    
    function updateCounter() {
        count++;
        console.log('Count:', count);
    }
    
    function toggleActive() {
        isActive = !isActive;
        return isActive;
    }
    
    // 2. CHTL JS - DOM 选择和引用
    const counter = {{#counter}};
    const button = {{.toggle-btn}};
    const items = {{.item}};
    
    // 3. 混合 - CHTL JS 事件 + JS 逻辑
    button &-> click: () => {
        updateCounter();  // JS 函数调用
        const active = toggleActive();  // JS 函数调用
        counter->textContent = count;  // CHTL JS 语法
        button->classList.toggle('active');  // 混合
    };
    
    // 4. CHTL JS Listen 块 + JS 逻辑
    {{.slider}}->Listen {
        input: (e) => {
            const value = parseInt(e.target.value);  // 纯 JS
            updateSomething(value);  // JS 函数
            {{#display}}->textContent = value;  // CHTL JS
        }
    };
    
    // 5. 纯 JS - 定时器和异步
    setInterval(() => {
        if (count < 100) {
            updateCounter();
            {{#counter}}->textContent = count;  // CHTL JS 在 JS 中
        }
    }, 1000);
    
    // 6. 纯 JS - API 调用
    fetch('/api/data')
        .then(res => res.json())
        .then(data => {
            // 纯 JS 数据处理
            const processed = data.map(item => item.value);
            // CHTL JS DOM 更新
            {{#result}}->textContent = JSON.stringify(processed);
        });
}
```

**处理方式**：
- ✅ CHTL JS 语法被转换
- ✅ 纯 JS 代码保持不变
- ✅ 两者完美共存

---

## 🔧 编译器处理流程

### 完整流程图

```
script 块内容
    │
    ├─→ CHTL 编译器接收
    │       │
    │       ├─→ 检测是否含 CHTL JS 语法
    │       │     ({{, Listen, Delegate, &->, Vir, $ 等)
    │       │
    │       ├─ 是 → 传递给 CHTL JS 编译器
    │       │         │
    │       │         ├─→ 处理 CHTL JS 语法
    │       │         │     {{}} → querySelector()
    │       │         │     &->  → addEventListener()
    │       │         │     ->   → .
    │       │         │     Listen → addEventListener
    │       │         │     等等
    │       │         │
    │       │         └─→ 纯 JS 代码保持不变
    │       │                │
    │       │                └─→ 输出标准 JS
    │       │
    │       └─ 否 → 直接输出（纯 JS）
    │
    └─→ 生成 <script> 标签
```

### 关键点

1. **自动检测**：
   - 检查是否含 CHTL JS 关键字
   - 如果有，使用 CHTL JS 编译器
   - 如果没有，直接输出

2. **选择性转换**：
   - CHTL JS 语法 → 转换为 JS
   - 纯 JS 语法 → 保持不变

3. **透明共存**：
   - 开发者可以自由混合使用
   - 编译器智能处理
   - 无需特殊标记

---

## 💡 最佳实践

### 推荐的混合使用模式

#### 模式 1：数据 + 交互分离

```chtl
script {
    // JS 部分 - 数据和逻辑
    const state = {
        count: 0,
        items: []
    };
    
    function addItem(item) {
        state.items.push(item);
        updateUI();
    }
    
    function updateUI() {
        // CHTL JS 部分 - DOM 更新
        {{#count}}->textContent = state.count;
        {{#items}}->innerHTML = state.items.map(i => `<li>${i}</li>`).join('');
    }
    
    // CHTL JS 部分 - 事件绑定
    {{.add-btn}} &-> click: () => {
        addItem('New Item');  // 调用 JS 函数
    };
}
```

#### 模式 2：初始化 + 事件处理

```chtl
script {
    // JS 部分 - 初始化配置
    const config = {
        apiUrl: 'https://api.example.com',
        timeout: 5000
    };
    
    async function fetchData() {
        const res = await fetch(config.apiUrl);
        return await res.json();
    }
    
    // CHTL JS 部分 - UI 绑定
    {{.load-btn}}->Listen {
        click: async () => {
            const data = await fetchData();  // JS 异步函数
            {{#output}}->textContent = JSON.stringify(data);
        }
    };
}
```

#### 模式 3：工具函数 + 声明式语法

```chtl
script {
    // JS 工具函数
    const utils = {
        formatDate: (date) => new Date(date).toLocaleDateString(),
        formatCurrency: (amount) => `$${amount.toFixed(2)}`,
        debounce: (fn, delay) => {
            let timer;
            return (...args) => {
                clearTimeout(timer);
                timer = setTimeout(() => fn(...args), delay);
            };
        }
    };
    
    // CHTL JS 增强选择器
    const input = {{#search-input}};
    const results = {{.results}};
    
    // 混合使用
    const debouncedSearch = utils.debounce((value) => {
        fetch(`/api/search?q=${value}`)
            .then(res => res.json())
            .then(data => {
                results->innerHTML = data.map(item => 
                    `<div>${item.name}</div>`
                ).join('');
            });
    }, 300);
    
    // CHTL JS 事件
    input &-> input: (e) => {
        debouncedSearch(e.target.value);
    };
}
```

---

## 🚫 注意事项

### CHTL JS 不是 JS 的超集

**重要**：CHTL JS 是独立的语言，不支持所有 JS 语法。

```chtl
script {
    // ❌ 错误：CHTL JS 不支持 ES6 import
    import { something } from 'module';
    
    // ✅ 正确：使用 CHTL JS 的 ScriptLoader
    ScriptLoader {
        load: ./module.cjjs
    };
    
    // ❌ 错误：CHTL JS 不支持 async/await（在 CHTL JS 特有语法中）
    Listen {
        click: async () => { ... }  // 这里的async/await是JS的，可以用
    };
    
    // ✅ 正确：在纯 JS 部分使用
    async function loadData() {
        const data = await fetch('/api');
        return data;
    }
}
```

### 语法冲突处理

#### -> 操作符

```chtl
script {
    // CHTL JS 的 ->（会被转换为 .）
    {{box}}->style.color = 'red';
    
    // JS 的箭头函数 =>（不会被转换）
    const fn = (x) => x * 2;
    
    // 混合使用
    items.map(item => {
        return {{#template}}->cloneNode(true);  // CHTL JS
    });
}
```

#### & 符号

```chtl
script {
    // CHTL JS 的 &（上下文引用）
    {{&}}->addEventListener('click', handler);
    
    // CHTL JS 的 &->（事件绑定操作符）
    {{box}} &-> click: handler;
    
    // JS 的 &&（逻辑与，不会被转换）
    if (x > 0 && y < 100) {
        console.log('Valid');
    }
    
    // JS 的位运算 &（不会被转换）
    const flags = 0b1010 & 0b1100;
}
```

---

## 🎨 实际应用示例

### 完整的混合语言应用

```chtl
use html5;

[Import] @Chtl from chtl::Chtholly

html {
    head {
        title { text: "三语言共存示例"; }
    }
    
    body {
        // CHTL 语言 - 结构和样式
        @Element ChthollyPage;
        
        div {
            id: app;
            
            style {
                // CHTL 语言 - 局部样式
                @Style ChthollyCard;
                
                .app {
                    padding: ChthollySpacing(xl);
                }
            }
            
            h1 { text: "计数器应用"; }
            
            div {
                id: counter;
                text: "0";
            }
            
            button {
                class: increment-btn;
                text: "+1";
            }
            
            button {
                class: decrement-btn;
                text: "-1";
            }
            
            button {
                class: reset-btn;
                text: "重置";
            }
            
            script {
                // =====================================
                // 纯 JavaScript 部分 - 数据和逻辑
                // =====================================
                
                // 数据模型
                let state = {
                    count: 0,
                    history: [],
                    maxCount: 100
                };
                
                // 业务逻辑
                function increment() {
                    if (state.count < state.maxCount) {
                        state.count++;
                        state.history.push({ action: 'increment', time: Date.now() });
                        return true;
                    }
                    return false;
                }
                
                function decrement() {
                    if (state.count > 0) {
                        state.count--;
                        state.history.push({ action: 'decrement', time: Date.now() });
                        return true;
                    }
                    return false;
                }
                
                function reset() {
                    state.count = 0;
                    state.history = [];
                }
                
                // 工具函数
                const formatCount = (count) => {
                    return count.toString().padStart(3, '0');
                };
                
                // =====================================
                // CHTL JS 部分 - DOM 选择
                // =====================================
                
                const counterDisplay = {{#counter}};
                const incrementBtn = {{.increment-btn}};
                const decrementBtn = {{.decrement-btn}};
                const resetBtn = {{.reset-btn}};
                
                // =====================================
                // 混合部分 - 事件处理
                // =====================================
                
                // 使用 CHTL JS 事件绑定 + JS 逻辑
                incrementBtn &-> click: () => {
                    if (increment()) {  // JS 函数
                        counterDisplay->textContent = formatCount(state.count);  // CHTL JS
                        console.log('Count increased to', state.count);  // JS
                    }
                };
                
                decrementBtn &-> click: () => {
                    if (decrement()) {
                        counterDisplay->textContent = formatCount(state.count);
                    }
                };
                
                // 使用 CHTL JS Listen 块
                resetBtn->Listen {
                    click: () => {
                        reset();  // JS 函数
                        counterDisplay->textContent = formatCount(state.count);  // CHTL JS
                        console.log('Counter reset');  // JS
                    }
                };
                
                // =====================================
                // 纯 JS 部分 - 初始化和定时器
                // =====================================
                
                // 初始化
                document.addEventListener('DOMContentLoaded', () => {
                    counterDisplay.textContent = formatCount(state.count);
                });
                
                // 自动保存历史
                setInterval(() => {
                    if (state.history.length > 0) {
                        localStorage.setItem('history', JSON.stringify(state.history));
                    }
                }, 5000);
                
                // =====================================
                // 纯 JS 部分 - API 交互
                // =====================================
                
                async function saveToServer() {
                    try {
                        const response = await fetch('/api/counter', {
                            method: 'POST',
                            headers: { 'Content-Type': 'application/json' },
                            body: JSON.stringify(state)
                        });
                        const data = await response.json();
                        console.log('Saved:', data);
                    } catch (error) {
                        console.error('Save failed:', error);
                    }
                }
                
                // 每分钟自动保存
                setInterval(saveToServer, 60000);
            }
        }
    }
}
```

---

## 🛠️ 编译器实现原理

### 当前实现（完美支持三语言）

#### CHTLGenerator (CHTL 编译器)

```cpp
void JsGenerator::collectScripts(...) {
    if (node->getType() == NodeType::Script) {
        String content = scriptNode->getContent();
        
        // 检测是否包含 CHTL JS 语法
        bool hasCHTLJS = (
            content.find("{{") != String::npos ||
            content.find("Listen") != String::npos ||
            content.find("Delegate") != String::npos ||
            content.find("&->") != String::npos ||
            // ... 其他 CHTL JS 关键字
        );
        
        if (hasCHTLJS) {
            // 有 CHTL JS 语法，传递给 CHTL JS 编译器
            CHTLJSGenerator jsGen(config);
            content = jsGen.generate(content);
        }
        // 否则直接输出（纯 JS）
        
        output += content;
    }
}
```

#### CHTLJSGenerator (CHTL JS 编译器)

```cpp
String CHTLJSGenerator::generate(const String& chtljsCode) {
    String result = chtljsCode;
    
    // 只处理 CHTL JS 特有语法
    result = processVirDeclarations(result);     // Vir
    result = processScriptLoaderBlocks(result);  // ScriptLoader
    result = processListenBlocks(result);        // Listen
    result = processDelegateBlocks(result);      // Delegate
    result = processEventBindOperators(result);  // &->
    result = processEnhancedSelectors(result);   // {{}}
    result = processArrowOperator(result);       // ->
    result = processAmpersand(result);           // &
    
    // 纯 JS 代码在这些处理中不会被改变
    
    return result;
}
```

### 关键设计

1. **选择性处理**：
   - 只转换 CHTL JS 语法
   - 不触碰纯 JS 代码

2. **智能检测**：
   - 检测 CHTL JS 关键字
   - 自动决定是否需要处理

3. **语法边界**：
   - `{{}}` 只在 CHTL JS 中有意义
   - `&->` 只在 CHTL JS 中有意义
   - `=>` 只在 JS 中有意义
   - `&&` 在 JS 中是逻辑运算符

---

## ✅ 测试验证

### 混合脚本测试

```cpp
TEST_CASE("Mixed CHTL JS and Pure JS") {
    String mixed = R"(
        // 纯 JS
        const count = 0;
        function update() { count++; }
        
        // CHTL JS
        {{#counter}}->textContent = count;
        
        {{.btn}} &-> click: () => {
            update();  // JS 函数
            {{#counter}}->textContent = count;  // CHTL JS
        };
    )";
    
    CHTLJSGenerator gen;
    String result = gen.generate(mixed);
    
    // 验证 JS 保留
    REQUIRE(result.find("const count = 0") != String::npos);
    REQUIRE(result.find("function update") != String::npos);
    
    // 验证 CHTL JS 转换
    REQUIRE(result.find("querySelector") != String::npos);
    REQUIRE(result.find("addEventListener") != String::npos);
}
```

**测试结果**: ✅ 全部通过

---

## 📖 语言特性对照表

| 特性 | CHTL | CHTL JS | JavaScript |
|------|------|---------|------------|
| HTML 结构 | ✅ | ❌ | ❌ |
| CSS 样式 | ✅ | ❌ | ❌ |
| 增强选择器 `{{}}` | ❌ | ✅ | ❌ |
| 事件绑定 `&->` | ❌ | ✅ | ❌ |
| Listen 块 | ❌ | ✅ | ❌ |
| Delegate 块 | ❌ | ✅ | ❌ |
| 箭头操作符 `->` | ❌ | ✅ | ❌ |
| 虚对象 Vir | ❌ | ✅ | ❌ |
| 变量定义 | ❌ | ❌ | ✅ |
| 函数定义 | ❌ | ❌ | ✅ |
| 类定义 | ❌ | ❌ | ✅ |
| 箭头函数 `=>` | ❌ | ❌ | ✅ |
| Promise/async | ❌ | ❌ | ✅ |
| 模板字符串 | ❌ | ❌ | ✅ |

---

## 🎯 总结

### ✅ CHTL 完美支持三门语言

1. **CHTL** - HTML 和 CSS
2. **CHTL JS** - DOM 和事件增强
3. **JavaScript** - 逻辑和数据处理

### ✅ script 块完全支持

- ✅ 可以写纯 JS
- ✅ 可以写纯 CHTL JS
- ✅ **可以混合使用**（推荐）

### ✅ 编译器智能处理

- ✅ 自动检测语法类型
- ✅ 选择性转换
- ✅ 纯 JS 完全保留
- ✅ CHTL JS 转换为 JS
- ✅ 无需手动标记

### ✅ 完全符合 CHTL.md 规范

- ✅ 2099 行："脚本由 CHTL、CHTL JS 及 JS 编译器共同管理"
- ✅ 1283 行："script 块由 CHTL JS 编译器解析"
- ✅ 1290 行："CHTL JS 完全独立于 JS"
- ✅ 2107 行："除局部script外，其他script禁止使用CHTL语法"

---

**CHTL v2.5.0 完美支持 CHTL + CHTL JS + JS 三门语言！** ✨

**建议使用模式**：
- JS 处理数据和逻辑
- CHTL JS 处理 DOM 和事件
- CHTL 处理结构和样式

**这种分离带来**：
- 清晰的职责划分
- 更好的代码组织
- 强大的表达能力
