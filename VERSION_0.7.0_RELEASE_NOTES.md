# CHTL v0.7.0-alpha Release Notes

**Release Date**: 2025-10-06  
**Codename**: "Expression & Reference"  
**Status**: Alpha Release

## 🎉 重大更新

### 新功能

#### 1. 完整的表达式系统 ⭐⭐⭐⭐⭐
支持在CSS中使用算术运算、条件表达式和属性引用！

**算术运算**：
```chtl
div {
    style {
        width: 100px + 50px;      // → 150px
        height: 200px - 50px;     // → 150px
        margin: 10px * 2;         // → 20px
        padding: 100px / 4;       // → 25px
        z-index: 2 ** 3;          // → 8
    }
}
```

**条件表达式**：
```chtl
div {
    style {
        display: 100 > 50 ? block : none;     // → block
        background: 1 ? red : blue;           // → red
    }
}
```

**属性引用**：
```chtl
div {
    id: box;
    style {
        width: 100px;
    }
}

div {
    style {
        width: box.width + 20px;              // → 120px
        height: .box.height * 2;              // 支持类选择器
    }
}
```

#### 2. 增强选择器转换 ⭐⭐⭐⭐⭐
CHTL JS的增强选择器现在完全工作！

```chtl
script {
    {{.card}}->addEventListener('click', fn);
    // → document.querySelector('.card').addEventListener('click', fn);
    
    {{#myId}}->disabled = true;
    // → document.getElementById('myId').disabled = true;
    
    {{button[0]}}->classList.add('active');
    // → document.querySelectorAll('button')[0].classList.add('active');
}
```

#### 3. 上下文感知&引用 ⭐⭐⭐⭐
`{{&}}`现在能智能解析为父元素！

```chtl
button {
    id: submit;
    
    script {
        {{&}}->disabled = true;
        // → document.getElementById('submit').disabled = true;
    }
}
```

### Bug修复

1. ✅ 修复增强选择器未转换为JavaScript的bug
2. ✅ 修复箭头操作符`->`未转换的bug
3. ✅ 修复生成器注释输出到JavaScript的bug
4. ✅ 修复内联样式未输出到HTML的bug

### 改进

1. ✅ 改进Parser，支持复杂表达式解析
2. ✅ 改进Generator，支持两遍生成（收集+输出）
3. ✅ 改进SaltBridge，支持属性查找
4. ✅ 改进错误处理和位置追踪

## 📊 统计数据

- **代码行数**: ~7,500行C++
- **测试用例**: 34个
- **测试断言**: 217个
- **通过率**: 100%
- **编译警告**: 0
- **功能完成度**: 70%

## 🔧 API变更

### 新增API

#### SaltBridge
```cpp
// 注册元素及其属性
void registerElementWithProperties(
    const String& tag, 
    const String& id, 
    const String& className,
    const HashMap<String, String>& properties
);

// 获取元素属性值
Optional<String> getElementProperty(
    const String& selector, 
    const String& property
);

// 清理注册表
void clearRegistry();
```

#### ExpressionParser
```cpp
// 解析CSS表达式值
String parseExpressionValue();
```

### 破坏性变更

无破坏性变更。所有现有API保持兼容。

## 🐛 已知问题

1. ⚠️ 属性引用只支持向前引用（后面的元素引用前面的元素）
   - **原因**: 即时注册机制
   - **影响**: 中等
   - **计划**: v0.8考虑两遍解析

2. ⚠️ 模板未实现展开
   - **状态**: 结构已解析，展开逻辑未实现
   - **影响**: 中等
   - **计划**: v0.75实现

3. ⚠️ 高级CHTL JS特性未实现
   - **状态**: Listen, Delegate, Animate, Router未实现
   - **影响**: 低（基础功能可用）
   - **计划**: v0.80-v0.93实现

## 📚 文档

### 新增文档
- CURRENT_STATUS_ANALYSIS.md - 项目状态分析
- SESSION_SUMMARY.md - 开发会话总结
- DEVELOPMENT_PROGRESS_REPORT.md - 进展报告
- MILESTONE_ACHIEVEMENT.md - 里程碑成就
- FINAL_DEVELOPMENT_REPORT.md - 最终开发报告
- IMPLEMENTATION_ROADMAP.md - 实施路线图

### 更新文档
- PROJECT_STATUS.md - 项目状态更新
- README.md - 主要功能说明

## 🚀 迁移指南

从之前版本迁移到v0.7.0-alpha：

### 无需更改
所有现有的CHTL代码保持兼容，无需修改。

### 可选升级
您现在可以使用新功能：

1. **使用算术运算**替代固定值：
```chtl
// 之前
style {
    width: 150px;  // 手动计算
}

// 现在
style {
    width: 100px + 50px;  // 自动计算
}
```

2. **使用条件表达式**替代多个规则：
```chtl
// 之前
.large { width: 1200px; }
.small { width: 800px; }

// 现在
style {
    width: $isLarge$ ? 1200px : 800px;
}
```

3. **使用属性引用**实现组件化：
```chtl
// 之前
div { style { width: 120px; } }  // 手动维护

// 现在
div { id: base; style { width: 100px; } }
div { style { width: base.width + 20px; } }  // 自动跟随
```

## 🙏 致谢

感谢所有为CHTL项目做出贡献的开发者！

特别感谢：
- CHTL设计团队 - 优秀的语言设计
- 测试驱动开发理念 - 保证代码质量
- C++社区 - 提供强大的工具和库

## 📞 反馈与支持

- **Issues**: GitHub Issues
- **Discussions**: GitHub Discussions
- **Documentation**: /docs目录
- **Examples**: /examples目录

## 🔮 下一版本预告

**v0.75.0-alpha** (预计1周后):
- 模板系统完整实现
- 代码复用能力大幅提升
- 更多示例和文档

**v0.80.0-beta** (预计2周后):
- Listen {}声明式监听
- CHTL JS功能大幅增强

**v1.0.0** (预计2个月后):
- 所有CHTL.md规范功能
- 完整的模块系统
- 生产级质量

---

**Happy Coding with CHTL!** 🎨✨

---

**更新日志**:
- 2025-10-06: v0.7.0-alpha首次发布
