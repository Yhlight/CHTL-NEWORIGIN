#ifndef CHTL_MODULE_INFO_H
#define CHTL_MODULE_INFO_H

#include "../../Common.h"

namespace CHTL {
namespace CMOD {

// 模块类型
enum class ModuleType {
    CMOD,       // CHTL模块
    CJMOD,      // CHTL JS扩展模块
    Hybrid      // CMOD + CJMOD混合模块
};

// 模块信息
struct ModuleInfo {
    String name;
    String version;
    String description;
    String author;
    String license;
    Vector<String> dependencies;  // 依赖的模块列表
    String category;
    String minCHTLVersion;
    String maxCHTLVersion;
    ModuleType type;
    
    // 文件路径
    String modulePath;        // 模块文件夹路径
    String infoFilePath;      // info文件路径
    String srcPath;           // src路径
    
    ModuleInfo() : type(ModuleType::CMOD) {}
    
    // 验证模块信息的完整性
    bool isValid() const {
        return !name.empty() && !version.empty();
    }
    
    // 检查CHTL版本兼容性
    bool isCompatibleWith(const String& chtlVersion) const {
        // 简单的版本比较逻辑
        if (!minCHTLVersion.empty() && chtlVersion < minCHTLVersion) {
            return false;
        }
        if (!maxCHTLVersion.empty() && chtlVersion > maxCHTLVersion) {
            return false;
        }
        return true;
    }
};

// 导出表项
struct ExportEntry {
    enum class Type {
        CustomStyle,
        CustomElement,
        CustomVar,
        TemplateStyle,
        TemplateElement,
        TemplateVar,
        OriginHtml,
        OriginStyle,
        OriginJavaScript,
        OriginCustom,
        Configuration
    };
    
    Type type;
    String name;
    String customTypeName;  // 对于自定义Origin类型
    
    ExportEntry(Type t, const String& n, const String& custom = "")
        : type(t), name(n), customTypeName(custom) {}
};

// 模块导出表
struct ModuleExports {
    Vector<ExportEntry> entries;
    
    void addExport(ExportEntry::Type type, const String& name, 
                   const String& customType = "") {
        entries.emplace_back(type, name, customType);
    }
    
    // 检查是否导出指定名称的项
    bool hasExport(const String& name) const {
        for (const auto& entry : entries) {
            if (entry.name == name) {
                return true;
            }
        }
        return false;
    }
    
    // 获取所有指定类型的导出
    Vector<String> getExportsByType(ExportEntry::Type type) const {
        Vector<String> result;
        for (const auto& entry : entries) {
            if (entry.type == type) {
                result.push_back(entry.name);
            }
        }
        return result;
    }
};

// 子模块信息
struct SubModuleInfo {
    String name;
    String path;
    ModuleInfo info;
    
    SubModuleInfo(const String& n, const String& p) 
        : name(n), path(p) {}
};

// 完整的模块数据
struct ModuleData {
    ModuleInfo info;
    ModuleExports exports;
    Vector<SubModuleInfo> subModules;
    HashMap<String, String> sourceFiles;  // 文件名 -> 文件内容
    
    // 是否有子模块
    bool hasSubModules() const {
        return !subModules.empty();
    }
    
    // 查找子模块
    Optional<SubModuleInfo> findSubModule(const String& name) const {
        for (const auto& sub : subModules) {
            if (sub.name == name) {
                return sub;
            }
        }
        return std::nullopt;
    }
};

} // namespace CMOD
} // namespace CHTL

#endif // CHTL_MODULE_INFO_H
