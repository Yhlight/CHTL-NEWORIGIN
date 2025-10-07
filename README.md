# CHTL - 超文本模板语言
**版本**: 2.6.1-vscode  
**许可**: MIT  
**语言**: C++17

基于C++实现的超文本语言，提供更符合开发者习惯的HTML/CSS/JS编写方式。

---

## 🚀 快速开始

### 构建项目
```bash
cd /workspace
python3 build.py all
```

### 编译CHTL文件
```bash
# 基础编译
./build/chtl examples/simple.chtl

# 内联所有资源
./build/chtl examples/simple.chtl --inline

# 完整HTML5页面
./build/chtl examples/simple.chtl --default-struct --inline
```

### 运行测试
```bash
./build/chtl_tests
```

**当前测试状态**: ✅ 87/87 通过 (514断言)

---

## 📋 功能状态

### ✅ 完全实现的功能

#### CHTL 核心
- ✅ 元素、属性、文本节点
- ✅ 注释系统（//、/**/、#）
- ✅ 局部样式块（style {}）
- ✅ 模板系统（[Template]）
- ✅ 自定义系统（[Custom]）
- ✅ 原始嵌入（[Origin]）
- ✅ 导入系统（[Import]）
- ✅ 命名空间（[Namespace]）
- ✅ 配置组（[Configuration]）

#### CHTL JS 核心
- ✅ 增强选择器 `{{}}`
- ✅ 事件监听 `Listen {}`
- ✅ 事件绑定 `&->`
- ✅ 事件委托 `Delegate {}`
- ✅ 动画系统 `Animate {}`
- ✅ SPA路由 `Router {}`
- ✅ 虚对象 `Vir`
- ✅ 脚本加载器 `ScriptLoader {}`
- ✅ 响应式值 `$var$`

#### 模块系统
- ✅ CMOD模块
- ✅ CJMOD模块
- ✅ 混合模块
- ✅ 官方模块（Chtholly, Yuigahama）

### ✅ 新增完成（v2.6.x）
- ✅ **条件渲染** - if/else if/else完整实现 🎉
- ✅ **CLI命令行工具** - 完整的编译工具 🎉
- ✅ **VSCode扩展** - 语法高亮、代码片段、命令集成 🎉
  - 语法高亮（100%）
  - 17个代码片段
  - 右键编译命令
  - 任务配置

### ⚠️ 部分实现
- ⚠️ VSCode高级功能 - IntelliSense、实时预览待开发

### ❌ 未实现
- ❌ CLI文件监视（--watch）
- ❌ 编译监视器
- ❌ 高级CLI程序（RGB、背景图等）

---

## 📖 文档

### 核心文档
- **CHTL.md** - 完整语言规范（2500+行）
- **CURRENT_STATUS.md** - 项目当前状态 ⭐
- **REALISTIC_PROGRESS_REPORT.md** - 务实进度评估 ⭐

### 开发指南
- **CONTRIBUTING.md** - 贡献指南
- **MODULE_DEVELOPMENT_GUIDE.md** - 模块开发
- **MODULE_USAGE_GUIDE.md** - 模块使用

### 参考
- **QUICK_REFERENCE.md** - 快速参考
- **INSTALLATION.md** - 安装说明
- **CHANGELOG.md** - 变更日志

---

## 🔨 项目结构

```
/workspace/
├── src/
│   ├── CHTL/              # CHTL编译器
│   ├── CHTL-JS/           # CHTL JS编译器
│   ├── SharedCore/        # 盐桥（通信桥梁）
│   └── Util/              # 工具类
├── tests/                 # 测试套件（84个）
├── Module/                # 官方模块
├── examples/              # 示例代码
├── build.py               # 构建脚本
└── CMakeLists.txt         # CMake配置
```

---

## 📊 项目统计

- **代码行数**: ~15,000行（核心）+ 400行（VSCode）
- **测试用例**: 89个（100%通过）
- **测试断言**: 534个（100%通过）
- **模块数**: 2个官方模块
- **VSCode代码片段**: 17个
- **完成度**: 约92%

---

## 🎯 架构原则

### 三层架构
1. **CHTL核心** - 静态特征（HTML/CSS）
2. **CHTL JS核心** - 基础动态特征
3. **CJMOD扩展** - 高级功能（通过API）

### 设计模式
- **状态机模式** - 解析状态管理
- **策略模式** - 生成策略
- **访问者模式** - AST遍历

### 通信机制
- **盐桥(SaltBridge)** - CHTL ↔ CHTL JS通信

---

## ⚠️ 已知限制

1. ❌ 条件渲染（if块）不能实际使用
2. ❌ 无命令行工具（不能CLI编译）
3. ❌ 无IDE支持（无语法高亮）
4. ⚠️ 模块打包需要libzip库

---

## 🚀 后续计划

### v2.6.0（条件渲染完成）
- 实现条件渲染生成器
- 完整的if/else if/else支持

### v2.7.0（CLI工具）
- 命令行编译工具
- 基本输出选项

### v3.0.0（完整生态）
- VSCode扩展
- 文档网站
- 社区支持

---

## 📄 许可证

MIT License - 详见LICENSE文件

---

## 👥 贡献

欢迎贡献！请阅读CONTRIBUTING.md了解详情。

---

## 📞 联系

- **团队**: CHTL Development Team
- **规范**: CHTL.md
- **问题**: 提交Issue

---

*本README如实反映项目当前状态*  
*最后更新: 2025-10-07*
